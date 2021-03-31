/*
 * @Author: yj 
 * @email: shiliu.yang@tuya.com
 * @LastEditors: yj 
 * @file name: tuya_bldc_system.c
 * @Description: 
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2021-02-22 15:10:00
 * @LastEditTime: 2021-02-22 15:10:00
 */
#include "tuya_hal_thread.h"
#include "tuya_hal_system.h"
#include "uni_thread.h"
#include "uni_log.h"
#include "tuya_gpio.h"
#include "soc_timer.h"
#include "soc_pwm.h"

#include "tuya_bldc_system.h"
#include "tuya_bldc_pwm.h"
#include "tuya_bldc_key.h"
#include "tuya_bldc_led.h"
#include "tuya_dp_process.h"

//风扇在按键上的档位
UINT_T g_fan_speed_gear[] = {10, 20, 35, 55, 60, 75, 85, 100};

//自然风模式，定时器，标志
#define NATURAL_SPEED_CHANGE_TIME   15 //单位：秒
static UINT8_T natural_speed_low_flag = 0x00;

//睡眠风模式
#define SLEEP_SPEED_CHANGE_TIME (60 * 60) //单位：秒

FAN_STATE_T fan_default_state = {
    .dp_id_switch = DP_ID_SWITCH,
    .dp_id_mode = DP_ID_MODE,
    .dp_id_speed = DP_ID_SPEED,
    .dp_id_bright = DP_ID_BRIGHT,

    .on_off = FALSE,
    .mode   = NORMAL_MODE,
    .speed  = 10,
    .bright = 2,
    .local_timing = 0xFF,
};

FAN_STATE_T  fan_state = {
    .dp_id_switch = DP_ID_SWITCH,
    .dp_id_mode = DP_ID_MODE,
    .dp_id_speed = DP_ID_SPEED,
    .dp_id_bright = DP_ID_BRIGHT,

    .on_off = FALSE,
    .mode   = NORMAL_MODE,
    .speed  = 10,
    .bright = 2,
    .local_timing = 0xFF,
};

 /***********************************************************
 *   Function:  get_cur_gear
 *   Input:     none
 *   Output:    none
 *   Return:    none
 *   Notice:    得到当前风速档位 
 ***********************************************************/
UINT8_T get_cur_gear(VOID_T)
{
    INT_T i;
    UINT8_T ret_cur_gear;

    for (i=0; i<(SIZEOF(g_fan_speed_gear)/SIZEOF(g_fan_speed_gear[0])); i++) {
        if (fan_state.speed <= g_fan_speed_gear[i]) {
            ret_cur_gear = i;
            break;
        }
    }

    return ret_cur_gear;
}

 /***********************************************************
 *   Function:  fan_mode_normal
 *   Input:     none
 *   Output:    none
 *   Return:    none
 *   Notice:    普通模式 
 ***********************************************************/
static VOID_T fan_mode_normal(VOID_T)
{
    INT_T opRet = LIGHT_OK;

    //关闭睡眠模式的定时器，防止干扰普通模式的运行
    opRet = opSocSWTimerStop(SLEEP_MODE_TIMER);
    if (opRet != LIGHT_OK) {
        PR_ERR("stop sleep timer error");
    }

    //关闭自然模式的定时器，防止干扰普通模式的运行
    opRet = opSocSWTimerStop(NATURAL_MODE_TIMER);
    if (opRet != LIGHT_OK) {
        PR_ERR("stop natural timer error");
    }

    fan_speed_set(fan_state.speed);
    PR_NOTICE("+++ normal mode fan_state.speed : %d", fan_state.speed);
}

 /***********************************************************
 *   Function:  fan_mode_natural_task
 *   Input:     none
 *   Output:    none
 *   Return:    none
 *   Notice:    自然风模式 回调函数
 ***********************************************************/
static VOID_T fan_mode_natural_timer_cb(VOID_T)
{
    //如果关机，不执行任何操作
    if (fan_state.on_off == FALSE) {
        opSocSWTimerStop(NATURAL_MODE_TIMER);
        return;
    }

    if (natural_speed_low_flag) {
        PR_NOTICE("natural mode low speed");
        fan_speed_set(1);
    } else {
        PR_NOTICE("natural mode high speed");
        fan_speed_set(fan_state.speed);
    }
    natural_speed_low_flag = ~(natural_speed_low_flag);
    opSocSWTimerStart(NATURAL_MODE_TIMER, NATURAL_SPEED_CHANGE_TIME * 1000, fan_mode_natural_timer_cb);
}

 /***********************************************************
 *   Function:  fan_mode_natural
 *   Input:     none
 *   Output:    none
 *   Return:    none
 *   Notice:    自然风模式 
 ***********************************************************/
static VOID_T fan_mode_natural(VOID_T)
{
    INT_T opRet = LIGHT_OK;

    //关闭睡眠模式的定时器，防止干扰自然模式的运行 
    opRet = opSocSWTimerStop(SLEEP_MODE_TIMER);
    if (opRet != LIGHT_OK) {
        PR_ERR("stop sleep timer error");
    }

    natural_speed_low_flag = ~(0x00);
    fan_speed_set(fan_state.speed);

    opSocSWTimerStart(NATURAL_MODE_TIMER, NATURAL_SPEED_CHANGE_TIME * 1000, fan_mode_natural_timer_cb);
}

 /***********************************************************
 *   Function:  fan_sleep_mode_task
 *   Input:     none
 *   Output:    none
 *   Return:    none
 *   Notice:    睡眠风模式 定时器回调任务
 ***********************************************************/
static VOID_T fan_sleep_mode_task(VOID_T)
{
    UINT8_T cur_gear;

    PR_NOTICE("enter fan_sleep_mode_task!");
    //判断当前是不是最低档。若为最低档，不再降速 
    if (fan_state.speed <= g_fan_speed_gear[0]) {
        fan_speed_set(g_fan_speed_gear[0]);
        change_fan_state();
        opSocSWTimerStop(SLEEP_MODE_TIMER);
        return;
    }

    cur_gear = get_cur_gear();
    PR_NOTICE("current gear is %d.", cur_gear);
    fan_state.speed = g_fan_speed_gear[--cur_gear];

    //改变档位转速
    fan_speed_set(fan_state.speed);
    fan_speed_led_set(get_cur_gear()+1);
    PR_NOTICE("speed change to %d.", fan_state.speed);
    //写入风扇状态到falsh中
    write_flash_fan_state();

    //启动睡眠模式，1h 减一档
    opSocSWTimerStart(SLEEP_MODE_TIMER, SLEEP_SPEED_CHANGE_TIME * 1000, fan_sleep_mode_task);
}

 /***********************************************************
 *   Function:  fan_mode_sleep
 *   Input:     none
 *   Output:    none
 *   Return:    none
 *   Notice:    睡眠风模式 
 ***********************************************************/
static VOID_T fan_mode_sleep(VOID_T)
{
    UINT8_T cur_gear;
    INT_T opRet = LIGHT_OK;
    SHORT_T i;

    //关闭自然模式的定时器，防止干扰睡眠模式模式的运行 
    opRet = opSocSWTimerStop(NATURAL_MODE_TIMER);
    if (opRet != LIGHT_OK) {
        PR_ERR("stop sleep timer error");
    }
    opRet = opSocSWTimerStop(SLEEP_MODE_TIMER);
    if (opRet != LIGHT_OK) {
        PR_ERR("stop sleep timer error");
    }

    //判断当前档位
    cur_gear = get_cur_gear();
    fan_state.speed = g_fan_speed_gear[cur_gear];
    //改变档位转速
    fan_speed_set(fan_state.speed);
    PR_NOTICE("speed change to %d.", fan_state.speed);
    //写入风扇状态到falsh中
    write_flash_fan_state();

    opSocSWTimerStart(SLEEP_MODE_TIMER, SLEEP_SPEED_CHANGE_TIME * 1000, fan_sleep_mode_task);
}

VOID_T fan_turn_off(VOID_T) 
{
    fan_led_all_off();
    /* 停止所有定时器 */
    opSocSWTimerStop(SLEEP_MODE_TIMER);
    opSocSWTimerStop(NATURAL_MODE_TIMER);
    opSocSWTimerStop(SHUTDOWN_TIMER);
    /* 关闭风扇 */
    fan_speed_set(0);
    PR_NOTICE("shutdown fen");
    /* 相关参数设置 */
    fan_state.on_off = FALSE;
    fan_state.local_timing = 0xFF;
    write_flash_fan_state();
}

VOID_T change_fan_state(VOID_T)
{
    if (fan_state.on_off == FALSE) {
        fan_turn_off();
        hw_report_all_dp_status();
        PR_NOTICE("stop sleep & natural timer");
        return;
    }

    if (fan_state.bright == 1) {
        fan_led_dimmer(100);
    } else {
        fan_led_dimmer(900);
    }

    if (fan_state.mode == SLEEP_MODE) {
        PR_NOTICE("enter sleep mode"); 
        fan_mode_sleep(); 
    } else if (fan_state.mode == NATURAL_MODE) {
        PR_NOTICE("enter natural mode"); 
        fan_mode_natural(); 
    } else {
        PR_NOTICE("enter normal mode");
        fan_mode_normal();  
    }

    hw_report_all_dp_status();
    
    /* speed LED set */
    fan_speed_led_set(get_cur_gear()+1);
    fan_mode_led_set();

    return;
}

VOID_T fan_init(VOID_T)
{
    /* PWM init */
    fan_pwm_init();

    /* LED init */
    fan_led_init();

    /* KEY init */
    fan_key_init();

    fan_local_timing_shutdown();
    change_fan_state();
    
}
