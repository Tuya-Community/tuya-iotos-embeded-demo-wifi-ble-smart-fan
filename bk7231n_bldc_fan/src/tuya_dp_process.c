/*
 * @Author: yj 
 * @email: shiliu.yang@tuya.com
 * @LastEditors: yj 
 * @file name: tuya_dp_process.c
 * @Description: 
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2021-02-22 15:10:00
 * @LastEditTime: 2021-02-22 15:10:00
 */

#include "uni_log.h"
#include "tuya_cloud_wifi_defs.h"
#include "tuya_dp_process.h"
#include "tuya_bldc_system.h"

/**
 * @Function: hw_report_all_dp_status
 * @Description: 上报所有 dp 点
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 
 */
VOID_T hw_report_all_dp_status(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    INT_T dp_cnt = 0;
    dp_cnt = 4;

    /* 没有连接到路由器，退出 */
    GW_WIFI_NW_STAT_E wifi_state = STAT_LOW_POWER;
    get_wf_gw_nw_status(&wifi_state);
    if (wifi_state <= STAT_AP_STA_DISC || wifi_state == STAT_STA_DISC) {
        return;
    }

    TY_OBJ_DP_S *dp_arr = (TY_OBJ_DP_S *)Malloc(dp_cnt*SIZEOF(TY_OBJ_DP_S));
    if(NULL == dp_arr) {
        PR_ERR("malloc failed");
        return;
    }

    memset(dp_arr, 0, dp_cnt*SIZEOF(TY_OBJ_DP_S));

    dp_arr[0].dpid = fan_state.dp_id_switch;
    dp_arr[0].type = PROP_BOOL;
    dp_arr[0].time_stamp = 0;
    dp_arr[0].value.dp_bool = fan_state.on_off;

    dp_arr[1].dpid = fan_state.dp_id_mode;
    dp_arr[1].type = PROP_ENUM;
    dp_arr[1].time_stamp = 0;
    dp_arr[1].value.dp_enum = fan_state.mode;

    dp_arr[2].dpid = fan_state.dp_id_speed;
    dp_arr[2].type = PROP_VALUE;
    dp_arr[2].time_stamp = 0;
    dp_arr[2].value.dp_value = fan_state.speed;

    dp_arr[3].dpid = fan_state.dp_id_bright;
    dp_arr[3].type = PROP_VALUE;
    dp_arr[3].time_stamp = 0;
    dp_arr[3].value.dp_value = fan_state.bright;

    op_ret = dev_report_dp_json_async(NULL ,dp_arr,dp_cnt);
    Free(dp_arr);
    if(OPRT_OK != op_ret) {
        PR_ERR("dev_report_dp_json_async relay_config data error,err_num",op_ret);
    }

    PR_DEBUG("dp_query report_all_dp_data");
}

VOID_T deal_dp_proc(IN CONST TY_OBJ_DP_S *root)
{
    UCHAR_T dpid;

    dpid = root->dpid;
    PR_NOTICE("dpid:%d",dpid);
    PR_NOTICE("root->value.dp_bool:%d",root->value.dp_bool);

    switch(dpid) {
        case DP_ID_SWITCH:
            fan_state.on_off = root->value.dp_bool;
        break;
            
        case DP_ID_MODE:
            fan_state.mode = root->value.dp_enum;            
        break;
        
        case DP_ID_SPEED:
            fan_state.speed = root->value.dp_value;
        break;

        case DP_ID_BRIGHT:
            fan_state.bright = root->value.dp_value;
        break;

        default:
        break;
    }

    change_fan_state();

}

