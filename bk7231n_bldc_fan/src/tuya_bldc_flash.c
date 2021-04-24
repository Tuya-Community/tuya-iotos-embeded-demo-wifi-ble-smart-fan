/*
 * @Author: yj 
 * @email: shiliu.yang@tuya.com
 * @LastEditors: shiliu
 * @file name: tuya_bldc_flash.c
 * @Description: 
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2021-02-22 15:10:00
 * @LastEditTime: 2021-04-24 09:49:44
 */

#include "soc_flash.h"
#include "uni_log.h"

#include "tuya_bldc_flash.h"
#include "tuya_bldc_system.h"


//风扇状态存储偏移量
#define FAN_STATE_OFFSET        0x00
//风扇状态存储长度：数据头—风扇开关—风扇模式—风扇转速—本地定时时间—CRC_8校验
#define FAN_STATE_STORAGE_LEN   6   
//数据帧头
#define FAN_DATA_HEAD           0xFF
//电机状态在 flash 中的存放数据
#define FLASH_FAN_STATE_ON_OFF  1
#define FLASH_FAN_STATE_MODE    2
#define FLASH_FAN_STATE_SPEED   3
#define FLASH_FAN_STATE_TIMING  4

 /***********************************************************
 *   Function:  get_flash_fan_state
 *   Input:     none
 *   Output:    none
 *   Return:    none
 *   Notice:    从 flash 中读取电扇断电前状态，存到电扇状态结构体内 
 ***********************************************************/
VOID_T read_flash_fan_state(VOID_T)
{
    INT_T opRet, i;
    UCHAR_T fan_state_data_crc;
    UCHAR_T before_fan_power_off_state[FAN_STATE_STORAGE_LEN]; //断电前风扇状态

    opRet = uiSocFlashRead(SAVE_TYP1, FAN_STATE_OFFSET, FAN_STATE_STORAGE_LEN*SIZEOF(UCHAR_T), before_fan_power_off_state);
    if (opRet != FAN_STATE_STORAGE_LEN) {
        PR_ERR("read data error for flash");
        return;
    }

    //判断头部数据是否正确 
    if (before_fan_power_off_state[0] != FAN_DATA_HEAD) {
        PR_ERR("data head error");
        return;
    }

    fan_state_data_crc = get_crc_8(before_fan_power_off_state, (FAN_STATE_STORAGE_LEN - 1)*SIZEOF(UCHAR_T));
    //校验数据是否正确
    if (fan_state_data_crc != before_fan_power_off_state[FAN_STATE_STORAGE_LEN - 1]) { 
        PR_ERR("crc error, before_fan_power_off_state[%d] = %02x, crc data = %02x.", FAN_STATE_STORAGE_LEN - 1, before_fan_power_off_state[FAN_STATE_STORAGE_LEN - 1], fan_state_data_crc);
        return;
    }

    //将从 flash 读取到的数据，存放到结构体中
    fan_state.on_off    = before_fan_power_off_state[FLASH_FAN_STATE_ON_OFF];
    fan_state.mode      = before_fan_power_off_state[FLASH_FAN_STATE_MODE];
    fan_state.speed     = before_fan_power_off_state[FLASH_FAN_STATE_SPEED];
    fan_state.local_timing = before_fan_power_off_state[FLASH_FAN_STATE_TIMING];

    return;
}

 /***********************************************************
 *   Function:  write_flash_fan_state
 *   Input:     none
 *   Output:    none
 *   Return:    none
 *   Notice:    写电机状态到 flash 中  
 ***********************************************************/
VOID_T write_flash_fan_state(VOID_T) 
{
    INT_T opRet, i;
    UCHAR_T fan_state_buffer[FAN_STATE_STORAGE_LEN];

    fan_state_buffer[0] = FAN_DATA_HEAD;
    fan_state_buffer[1] = fan_state.on_off;
    fan_state_buffer[2] = fan_state.mode;
    fan_state_buffer[3] = fan_state.speed;
    fan_state_buffer[4] = fan_state.local_timing;
    fan_state_buffer[5] = get_crc_8(fan_state_buffer, (FAN_STATE_STORAGE_LEN - 1)*SIZEOF(UCHAR_T));
    
    for (i=0; i<FAN_STATE_STORAGE_LEN; i++) {
        PR_NOTICE(" +++ fan_state_buffer is [%d] : %02x", i, fan_state_buffer[i]);
    }

    opRet = opSocFlashWrite(SAVE_TYP1, FAN_STATE_OFFSET, fan_state_buffer, FAN_STATE_STORAGE_LEN * SIZEOF(UCHAR_T)); 
    if (opRet != LIGHT_OK) {
        PR_ERR("write flash error");
    }
    
    return;
}


 /***********************************************************
 *   Function:  erase_flash_fan_state
 *   Input:     none
 *   Output:    none
 *   Return:    none
 *   Notice:    恢复出厂设置  
 ***********************************************************/
VOID_T erase_flash_fan_state(VOID_T) 
{
    INT_T opRet, i;
    UCHAR_T fan_state_buffer[FAN_STATE_STORAGE_LEN];

    fan_state.on_off = FALSE;
    fan_state.mode   = NORMAL_MODE;
    fan_state.speed  = 1;

    fan_state_buffer[0] = FAN_DATA_HEAD;
    fan_state_buffer[1] = FALSE;    //fan_state.on_off
    fan_state_buffer[2] = NORMAL_MODE; //fan_state.mode
    fan_state_buffer[3] = 1;        //fan_state.speed
    fan_state_buffer[4] = 0xFF;     // fan_state.local_timing
    fan_state_buffer[5] = get_crc_8(fan_state_buffer, (FAN_STATE_STORAGE_LEN - 1)*SIZEOF(UCHAR_T));

    for (i=0; i<FAN_STATE_STORAGE_LEN; i++) {
        PR_NOTICE(" +++ fan_state_buffer is [%d] : %02x", i, fan_state_buffer[i]);
    }

    opRet = opSocFlashWrite(SAVE_TYP1, FAN_STATE_OFFSET, fan_state_buffer, FAN_STATE_STORAGE_LEN * SIZEOF(UCHAR_T)); 
    if (opRet != LIGHT_OK) {
        PR_ERR("write flash error");
    }

    return;
}
