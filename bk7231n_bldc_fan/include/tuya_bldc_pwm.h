/*
 * @Author: yj 
 * @email: shiliu.yang@tuya.com
 * @LastEditors: yj 
 * @file name: tuya_bldc_pwm.h
 * @Description: 
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2021-02-22 15:10:00
 * @LastEditTime: 2021-02-22 15:10:00
 */

#ifndef __TUYA_BLDC_PWM_H__
#define __TUYA_BLDC_PWM_H__

#include "tuya_cloud_com_defs.h"

/* led PWM */
#define LED_DIMMER_PWM_FREQUENCY   1000
#define LED_DIMMER_PWM_BRIGHT  100
#define LED_DIMMER_PWM_DARK    900
/* led 调光 IO 口 */
#define LED_DIMMER_GPIO TY_GPIOA_7
#define LED_DIMMER_ID  0

/* BLDC pwm */
#define BLDC_PWM_FREQUENCY  1000

//输出 PWM 占空比
#define BLDC_PWM_FAN_OFF    (5 * 10.0)
#define BLDC_PWM_FAN_MIN    (30 * 10.0)
#define BLDC_PWM_FAN_MAX    (99 * 10.0)
/* fan speed pwm out */
#define BLDC_PWM_GPIO   TY_GPIOA_8
#define BLDC_PWM_ID  1

VOID_T fan_pwm_init(VOID_T);
VOID_T fan_speed_set(UINT_T speed);
#endif /* __TUYA_BLDC_PWM_H__ */
