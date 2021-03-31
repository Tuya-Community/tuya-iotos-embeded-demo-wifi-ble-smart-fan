/*
 * @Author: yj 
 * @email: shiliu.yang@tuya.com
 * @LastEditors: yj 
 * @file name: tuya_bldc_led.c
 * @Description: 
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2021-02-22 15:10:00
 * @LastEditTime: 2021-02-22 15:10:00
 */

#include "tuya_gpio.h"
#include "tuya_led.h"
#include "uni_log.h"

#include "tuya_bldc_led.h"
#include "tuya_bldc_system.h"

/* 除WIFI指示灯(Pin_1)外，所有指示灯集合 */
/*                              D1             D2           D3          D4      MODE_WIND   MODE_SLEEP   MODE_NATURE   */
STATIC INT_T g_fan_led[] = {TY_GPIOA_16, TY_GPIOA_20, TY_GPIOA_22, TY_GPIOA_23, TY_GPIOA_0, TY_GPIOA_28, TY_GPIOA_17};
/* 风速指示灯 */
STATIC INT_T g_fan_speed_led[] = {TY_GPIOA_16, TY_GPIOA_20, TY_GPIOA_22, TY_GPIOA_23};
/* 本地定时指示灯 */
STATIC INT_T g_fan_timer_led[] = {TY_GPIOA_16, TY_GPIOA_20, TY_GPIOA_22, TY_GPIOA_23};

/* LED 句柄 */
#define LED_HANDLE_D1   0
#define LED_HANDLE_D2   1
#define LED_HANDLE_D3   2
#define LED_HANDLE_D4   3
#define LED_HANDLE_MODE_WIND    4
#define LED_HANDLE_MODE_SLEEP   5
#define LED_HANDLE_MODE_NATURE  6
LED_HANDLE fan_led_handle[(SIZEOF(g_fan_led) / SIZEOF(g_fan_led[0]))];

/**
 * @Function: fan_led_init
 * @Description: 初始化led
 * @Input:  none
 * @Output: none
 * @Return: none
 * @Others: 
 */
VOID_T fan_led_init(VOID_T) 
{
    INT_T i;
    OPERATE_RET op_ret = OPRT_OK;

    op_ret = tuya_gpio_inout_set(TY_GPIOA_0, FALSE);
    if (op_ret != OPRT_OK) {
        PR_ERR("inout set error: %d", op_ret);
    }

    for (i=0; i<(SIZEOF(g_fan_led) / SIZEOF(g_fan_led[0])); i++) {
        op_ret = tuya_create_led_handle(g_fan_led[i], FALSE, &fan_led_handle[i]);
        if (op_ret != OPRT_OK) {
            PR_ERR("key_init err:%d", op_ret);
            return;
        }
        tuya_set_led_light_type(fan_led_handle[i], OL_HIGH, 0, 0xffff); //关闭 LED
    }
}

VOID_T fan_led_all_off(VOID_T)
{
    INT_T i;
    for (i=0; i<(SIZEOF(g_fan_led) / SIZEOF(g_fan_led[0])); i++) {
        tuya_set_led_light_type(fan_led_handle[i], OL_HIGH, 0, 0xffff); //关闭 LED
    }
}

/**
 * @Function: fan_speed_led_set
 * @Description: 根据风扇档数设置LED
 * @Input:  none
 * @Output: none
 * @Return: none
 * @Others: 
 */
VOID_T fan_speed_led_set(UINT8_T cur_gear)
{
    INT_T i;
    //UINT8_T cur_gear_temp = cur_gear + 1;
    if (cur_gear > 8) {
        cur_gear = 8;
    }
    PR_NOTICE("enter fan speed led set");
    PR_NOTICE("current gear %d", cur_gear);
    /* 关闭所有灯 */
    for (i=0; i<4; i++) {
        tuya_set_led_light_type(fan_led_handle[i], OL_HIGH, 0, 0xffff); //关闭 LED
    }

    /* 常量代表*2 */
    for (i=0; i<(cur_gear/2); i++) {
        tuya_set_led_light_type(fan_led_handle[i], OL_LOW, 0, 0xffff); //打开 LED
    }

    /* 闪烁代表*1 */
    if (cur_gear%2) {
        tuya_set_led_light_type(fan_led_handle[i], OL_FLASH_HIGH, 500, 0xffff); //LED 闪烁
    }
    
    /* 不亮代表*0 */
}

/**
 * @Function: fan_mode_led_set
 * @Description: 根据风扇模式设置LED
 * @Input:  none
 * @Output: none
 * @Return: none
 * @Others: 
 */
VOID_T fan_mode_led_set(VOID_T)
{
    tuya_set_led_light_type(fan_led_handle[LED_HANDLE_MODE_WIND], OL_HIGH, 0, 0xffff); //关闭 LED
    tuya_set_led_light_type(fan_led_handle[LED_HANDLE_MODE_SLEEP], OL_HIGH, 0, 0xffff); //关闭 LED
    tuya_set_led_light_type(fan_led_handle[LED_HANDLE_MODE_NATURE], OL_HIGH, 0, 0xffff); //关闭 LED

    if (fan_state.mode == SLEEP_MODE) {
        tuya_set_led_light_type(fan_led_handle[LED_HANDLE_MODE_SLEEP], OL_LOW, 0, 0xffff); //打开 LED
    } else if (fan_state.mode == NATURAL_MODE) {
        tuya_set_led_light_type(fan_led_handle[LED_HANDLE_MODE_NATURE], OL_LOW, 0, 0xffff); //打开 LED
    } else {
        tuya_set_led_light_type(fan_led_handle[LED_HANDLE_MODE_WIND], OL_LOW, 0, 0xffff); //打开 LED
    }
}

VOID_T fan_local_timing_led_set(UINT8_T hour)
{
    INT_T i;

    /* 关闭所有灯 */
    for (i=0; i<4; i++) {
        tuya_set_led_light_type(fan_led_handle[i], OL_HIGH, 0, 0xffff); //关闭 LED
    }

    if (hour > 4) {
        PR_ERR("local timing input value error, hour : %d", hour);
        return;
    }

    /* 本地定时灯显示 */
    for (i=0; i<hour; i++) {
        tuya_set_led_light_type(fan_led_handle[i], OL_LOW, 0, 0xffff); //打开 LED
    }
}
