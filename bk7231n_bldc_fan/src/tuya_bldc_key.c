/*
 * @Author: yj 
 * @email: shiliu.yang@tuya.com
 * @LastEditors: yj 
 * @file name: tuya_bldc_key.c
 * @Description: 
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2021-02-22 15:10:00
 * @LastEditTime: 2021-02-22 15:10:00
 */

#include "tuya_bldc_key.h"
#include "tuya_gpio.h"
#include "tuya_key.h"
#include "uni_log.h"
#include "soc_timer.h"
#include "tuya_bldc_system.h"
#include "tuya_bldc_led.h"
#include "BkDriverGpio.h"

#define KEY_TIMER TY_GPIOA_14
#define KEY_POWER TY_GPIOA_15

#define KEY_ROTARY_N  TY_GPIOA_6
#define KEY_ROTARY_A  TY_GPIOA_26
#define KEY_ROTARY_B  TY_GPIOA_9

KEY_USER_DEF_S KEY_DEF_T;

extern UINT_T g_fan_speed_gear[];

STATIC VOID_T knod_key_cb(VOID_T)
{
    INT8_T current_gear;
    //如果关机，不执行任何操作
    if (fan_state.on_off == FALSE) {
        return;
    }

    BkGpioFinalize(KEY_ROTARY_A);

    //得到当前档位
    current_gear = get_cur_gear();

    if(tuya_gpio_read(KEY_ROTARY_A) != tuya_gpio_read(KEY_ROTARY_B)) {
        PR_DEBUG("A != B"); //顺时针方向
        current_gear++;
        if (current_gear > (MAX_GEAR_NUMBER-1)) {
            current_gear = (MAX_GEAR_NUMBER-1);
        }
        fan_state.speed = g_fan_speed_gear[current_gear];

    } else {
        PR_DEBUG("A == B"); //逆时针方向
        current_gear--;
        if (current_gear < 0) {
            current_gear = 0;
        }
        fan_state.speed = g_fan_speed_gear[current_gear];
    }

    /* 改变风扇状态：风速，模式，LED */
    change_fan_state();
    write_flash_fan_state();
    
    PR_DEBUG("fan current_gear is : %d", current_gear);

    /* 旋钮正反转检测初始化 */
    BkGpioEnableIRQ(KEY_ROTARY_A, IRQ_TRIGGER_FALLING_EDGE, knod_key_cb, NULL);
}

STATIC VOID_T key_press_cb(TY_GPIO_PORT_E port,PUSH_KEY_TYPE_E type,INT_T cnt)
{
    PR_DEBUG("port: %d, type: %d, cnt: %d", port, type, cnt);

    /* 旋钮按键 */
    if (port == KEY_ROTARY_N) {
        if (fan_state.on_off == FALSE) {
            return;
        }
        switch (type) {
            case NORMAL_KEY:
                PR_DEBUG("knod press.");

                if (fan_state.mode == NORMAL_MODE) {
                    fan_state.mode = NATURAL_MODE;
                } else if (fan_state.mode == NATURAL_MODE) {
                    fan_state.mode =SLEEP_MODE;
                } else {
                    fan_state.mode = NORMAL_MODE;
                }
                change_fan_state();
                break;
            case LONG_KEY:
                PR_DEBUG("knod long press.");
                /* 复位，删除所有用户信息，恢复到默认模式 */
                fan_state = fan_default_state;
                change_fan_state();
                write_flash_fan_state();
                break;

            case SEQ_KEY:
                PR_DEBUG("knod SEQ press, the count is %d.", cnt);
                break;

            default:
                break;
        }
    } 

    /* 定时按键 */
    if (port == KEY_TIMER) {
        if (fan_state.on_off == FALSE) {
            return;
        }
        switch (type) {
            case NORMAL_KEY:
                PR_DEBUG("timer press.");
                if (fan_state.local_timing == 0xFF) {
                    fan_state.local_timing = 1;
                } else if (fan_state.local_timing >= 4) {
                    fan_state.local_timing = 0xFF; //取消定时
                } else {
                    fan_state.local_timing++;
                }
                fan_local_timing_shutdown();
                break;
            case LONG_KEY:
                PR_DEBUG("timer long press.");

                break;
            case SEQ_KEY:
                PR_DEBUG("timer SEQ press, the count is %d.", cnt);

                break;
            default:
                break;
        }
    } 

    /* 开关按键 */
    if (port == KEY_POWER) {
        switch (type) {
            case NORMAL_KEY:
                if (fan_state.on_off == FALSE) {
                    fan_state.on_off = TRUE;
                    PR_DEBUG("Turn on");
                } else {
                    fan_state.on_off = FALSE;
                    PR_DEBUG("Turn off");
                }
                change_fan_state();
                break;
            case LONG_KEY:
                PR_DEBUG("power long press.");

                break;
            case SEQ_KEY:
                PR_DEBUG("power SEQ press, the count is %d.", cnt);

                break;
            default:
                break;
        }
    } 
    write_flash_fan_state();
}

VOID_T fan_key_init(VOID_T)
{
    OPERATE_RET opRet;

    tuya_gpio_inout_set(KEY_ROTARY_A, TRUE);
    tuya_gpio_inout_set(KEY_ROTARY_B, TRUE);

    /* 旋钮正反转检测初始化 */
    BkGpioEnableIRQ(KEY_ROTARY_A, IRQ_TRIGGER_FALLING_EDGE, knod_key_cb, NULL);

    opRet = key_init(NULL, 0, 0);
    if (opRet != OPRT_OK) {
        PR_ERR("key_init err:%d", opRet);
        return;
    }

    memset(&KEY_DEF_T, 0, SIZEOF(KEY_DEF_T));
    KEY_DEF_T.port = KEY_ROTARY_N;
    KEY_DEF_T.long_key_time = 3000;
    KEY_DEF_T.low_level_detect = TRUE;
    KEY_DEF_T.lp_tp = LP_ONCE_TRIG;
    KEY_DEF_T.call_back = key_press_cb;
    KEY_DEF_T.seq_key_detect_time = 400;
    opRet = reg_proc_key(&KEY_DEF_T);
    if (opRet != OPRT_OK) {
        PR_ERR("reg_proc_key err:%d", opRet);
        return;
    }

    KEY_DEF_T.port = KEY_TIMER;
    opRet = reg_proc_key(&KEY_DEF_T);
    if (opRet != OPRT_OK) {
        PR_ERR("reg_proc_key err:%d", opRet);
        return;
    }

    KEY_DEF_T.port = KEY_POWER;
    KEY_DEF_T.long_key_time = 10000;
    opRet = reg_proc_key(&KEY_DEF_T);
    if (opRet != OPRT_OK) {
        PR_ERR("reg_proc_key err:%d", opRet);
        return;
    }
}
