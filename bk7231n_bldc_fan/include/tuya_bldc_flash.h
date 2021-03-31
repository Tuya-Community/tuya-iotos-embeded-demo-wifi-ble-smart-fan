/*
 * @Author: yj 
 * @email: shiliu.yang@tuya.com
 * @LastEditors: yj 
 * @file name: tuya_bldc_flash.h
 * @Description: 
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2021-02-22 15:10:00
 * @LastEditTime: 2021-02-22 15:10:00
 */

#ifndef __TUYA_BLDC_FLASH_H__
#define __TUYA_BLDC_FLASH_H__

#include "tuya_cloud_com_defs.h"

VOID_T erase_flash_fan_state(VOID_T);
VOID_T read_flash_fan_state(VOID_T);
VOID_T write_flash_fan_state(VOID_T);

#endif /* __TUYA_BLDC_FLASH_H__ */
