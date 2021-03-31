/*
 * @Author: yj 
 * @email: shiliu.yang@tuya.com
 * @LastEditors: yj 
 * @file name: tuya_bldc_led.h
 * @Description: 
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2021-02-22 15:10:00
 * @LastEditTime: 2021-02-22 15:10:00
 */

#ifndef __TUYA_BLDC_LED_H__
#define __TUYA_BLDC_LED_H__

#include "tuya_cloud_com_defs.h"

VOID_T fan_led_init(VOID_T);
VOID_T fan_mode_led_set(VOID_T);
VOID_T fan_led_all_off(VOID_T);
VOID_T fan_speed_led_set(UINT8_T cur_gear);
VOID_T fan_local_timing_led_set(UINT8_T hour);

#endif /* __TUYA_BLDC_LED_H__ */
