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

#ifndef __TUYA_DP_PROCESS_H__
#define __TUYA_DP_PROCESS_H__

#include "tuya_cloud_com_defs.h"

#define DP_ID_SWITCH    1
#define DP_ID_MODE      2
#define DP_ID_SPEED     3
#define DP_ID_BRIGHT    16

VOID_T hw_report_all_dp_status(VOID_T);
VOID_T deal_dp_proc(IN CONST TY_OBJ_DP_S *root);

#endif /* __TUYA_DP_PROCESS_H__ */

