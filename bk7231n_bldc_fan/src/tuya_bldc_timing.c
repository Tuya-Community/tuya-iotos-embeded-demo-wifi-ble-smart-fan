/*
 * @Author: yj 
 * @email: shiliu.yang@tuya.com
 * @LastEditors: yj 
 * @file name: tuya_bldc_timing.c
 * @Description: 
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2021-02-22 15:10:00
 * @LastEditTime: 2021-02-22 15:10:00
 */

#include "uni_log.h"
#include "soc_timer.h"

#include "tuya_bldc_timing.h"
#include "tuya_bldc_flash.h"
#include "tuya_bldc_system.h"

#define SINGLE_TIMING (60*60) //60 min

VOID_T fan_timing_cd(VOID_T)
{
    fan_state.local_timing--;
    opSocSWTimerStop(SHUTDOWN_TIMER);
    if (fan_state.local_timing == 0 || fan_state.on_off == FALSE) {
        fan_turn_off();
    } else {
        PR_NOTICE("fan_state.local_timing ======== %d", fan_state.local_timing);
        write_flash_fan_state();
        opSocSWTimerStart(SHUTDOWN_TIMER, (SINGLE_TIMING*1000), fan_timing_cd);
        fan_local_timing_led_set(fan_state.local_timing);
    }
}

VOID_T fan_local_timing_shutdown(VOID_T)
{
    fan_local_timing_led_set(fan_state.local_timing);
    if (fan_state.local_timing > 4) { //无定时
        opSocSWTimerStop(SHUTDOWN_TIMER);
        return;
    }
    PR_NOTICE("run shutdown timer");
    opSocSWTimerStop(SHUTDOWN_TIMER);
    opSocSWTimerStart(SHUTDOWN_TIMER, (SINGLE_TIMING*1000), fan_timing_cd);
}
