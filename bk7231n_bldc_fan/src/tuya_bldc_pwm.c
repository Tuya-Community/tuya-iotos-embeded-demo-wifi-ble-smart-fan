/*
 * @Author: yj 
 * @email: shiliu.yang@tuya.com
 * @LastEditors: yj 
 * @file name: tuya_bldc_pwm.c
 * @Description: 
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2021-02-22 15:10:00
 * @LastEditTime: 2021-02-22 15:10:00
 */

#include "soc_pwm.h"
#include "tuya_gpio.h"

#include "tuya_bldc_pwm.h"

/**
 * @Function: fan_pwm_init
 * @Description: pwm 初始化，设置led调光和电扇pwm控制输出
 * @Input:  none
 * @Output: none
 * @Return: none
 * @Others: 
 */
VOID_T fan_pwm_init(VOID_T)
{
    UCHAR_T pwm_gpio_list[] = {LED_DIMMER_GPIO, BLDC_PWM_GPIO};
    UCHAR_T pwm_num = (SIZEOF(pwm_gpio_list) / SIZEOF(pwm_gpio_list[0]));

    opSocPwmInit(LED_DIMMER_PWM_FREQUENCY, LED_DIMMER_PWM_BRIGHT, pwm_num, pwm_gpio_list, TRUE, TRUE);
    opSocPwmInit(BLDC_PWM_FREQUENCY, (BLDC_PWM_FAN_OFF), pwm_num, pwm_gpio_list, TRUE, FALSE);
}

/**
 * @Function: fan_speed_set
 * @Description: 风速速度设置
 * @Input:  none
 * @Output: none
 * @Return: none
 * @Others: 
 */
VOID_T fan_speed_set(UINT_T speed)
{
    UINT_T  fan_speed_pwm_duty_cycle = 0;

    if (speed <= 0) {
        vSocPwmSetDuty(BLDC_PWM_ID, (BLDC_PWM_FAN_OFF));
        return;
    }

    //由于电机在30%以下工作时间过长会出现异常，这里对 PWM 输出进行一些处理，使输出的 PWM 在 30%-99% 之间
    fan_speed_pwm_duty_cycle = (UINT_T)(BLDC_PWM_FAN_MIN + ((BLDC_PWM_FAN_MAX - BLDC_PWM_FAN_MIN) * (speed / 100.0)));

    //输出的PWM占空比极性为高，电机需要的占空比极性为低。这里取一下反
    vSocPwmSetDuty(BLDC_PWM_ID, (fan_speed_pwm_duty_cycle));

    return;
}
/**
 * @Function: fan_led_dimmer
 * @Description: LED 灯亮度设置函数， light(0-1000) 越大灯越亮
 * @Input:  none
 * @Output: none
 * @Return: none
 * @Others: 
 */
VOID_T fan_led_dimmer(UINT16_T light) 
{
    vSocPwmSetDuty(LED_DIMMER_ID, (LED_DIMMER_PWM_FREQUENCY - light));
}

