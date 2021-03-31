/*
 * @Author: yj 
 * @email: shiliu.yang@tuya.com
 * @LastEditors: yj 
 * @file name: tuya_bldc_system.h
 * @Description: 
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2021-02-22 15:10:00
 * @LastEditTime: 2021-02-22 15:10:00
 */

#ifndef __TUYA_BLDC_SYSTEM_H__
#define __TUYA_BLDC_SYSTEM_H__

#include "tuya_cloud_com_defs.h"

/* software timer */
#define NATURAL_MODE_TIMER  1
#define SLEEP_MODE_TIMER    2
#define SHUTDOWN_TIMER      3

/* 最大档位 */
#define MAX_GEAR_NUMBER 8

// 工作模式 枚举
typedef UCHAR_T         FAN_WORK_MODE_T; 
#define NORMAL_MODE     0 
#define NATURAL_MODE    1 
#define SLEEP_MODE      2

//设备状态结构体
typedef struct {
    /* BLDC 风扇 dp点  */
    UINT8_T dp_id_switch;
    UINT8_T dp_id_mode;
    UINT8_T dp_id_speed;
    UINT8_T dp_id_bright;

    /* 风扇状态定义 */
    BOOL_T          on_off;
    FAN_WORK_MODE_T mode;
    UINT_T          speed;
    UINT_T          bright;

    /* 本地定时 */
    UINT8_T         local_timing; /* local_timing=0,时间到；local_timing=0xff,无定时； */

}FAN_STATE_T;

extern FAN_STATE_T  fan_state;
extern FAN_STATE_T  fan_default_state;

UINT8_T get_cur_gear(VOID_T);
VOID_T  change_fan_state(VOID_T);
VOID_T  fan_init(VOID_T);
VOID_T  fan_turn_off(VOID_T);

#endif /* __TUYA_BLDC_SYSTEM_H__ */
