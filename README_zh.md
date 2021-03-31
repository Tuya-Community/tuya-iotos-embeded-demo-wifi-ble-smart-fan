# Tuya IoTOS Embeded Demo WiFi & BLE Smart-Fan

[English](./README.md) | [中文](./README_zh.md)

## 简介 

本Demo通过涂鸦智能云平台、涂鸦智能APP、IoTOS Embeded WiFi &Ble SDK实现一款智能风扇，使用涂鸦WiFi/WiFi+BLE系列模组，实现远程数据监测。

已实现功能：

+ 本地定时，最长4小时。
+ 云定时，可随意设置。
+ 三种风扇模式（正常风，自然风：忽大忽小间隔10秒，睡眠风：每隔一小时自动降档，降到一档不再下降）
+ APP控制，断电记忆
+ APP可调整LED亮度




## 快速上手 

### 编译与烧录
+ 下载[Tuya IoTOS Embeded WiFi & BLE sdk](https://github.com/tuya/tuya-iotos-embeded-sdk-wifi-ble-bk7231n) 

+ 下载Demo至SDK目录的apps目录下 

  ```bash
  $ cd apps
  $ git clone https://github.com/Tuya-Community/tuya-iotos-embeded-demo-wifi-ble-smart-fan.git
  ```
  
+ 在SDK根目录下执行以下命令开始编译：

  ```bash
  sh build_app.sh apps/bk7231n_bldc_fan bk7231n_bldc_fan 1.0.0 
  ```

+ 固件烧录授权相关信息请参考：[Wi-Fi + BLE 系列模组烧录授权](https://developer.tuya.com/cn/docs/iot/device-development/burn-and-authorization/burn-and-authorize-wifi-ble-modules/burn-and-authorize-wb-series-modules?id=Ka78f4pttsytd) 

 

 ### 文件介绍 

```bash
├── include
│   ├── common
│   │   ├── light_printf.h
│   │   ├── light_tools.h
│   │   └── light_types.h
│   ├── soc
│   │   ├── soc_flash.h
│   │   ├── soc_gpio.h
│   │   ├── soc_pwm.h
│   │   └── soc_timer.h
│   ├── tuya_bldc_flash.h
│   ├── tuya_bldc_key.h
│   ├── tuya_bldc_led.h
│   ├── tuya_bldc_pwm.h
│   ├── tuya_bldc_system.h
│   ├── tuya_bldc_timing.h
│   ├── tuya_device.h
│   └── tuya_dp_process.h
└── src
    ├── common
    │   └── light_tools.c
    ├── soc					//
    │   ├── soc_flash.c
    │   ├── soc_gpio.c
    │   ├── soc_pwm.c
    │   └── soc_timer.c
    ├── tuya_bldc_flash.c	//断电记忆功能
    ├── tuya_bldc_key.c		//按键
    ├── tuya_bldc_led.c		//LED控制
    ├── tuya_bldc_pwm.c		//PWM控制
    ├── tuya_bldc_system.c	//智能风扇控制相关
    ├── tuya_bldc_timing.c	//本地定时功能文件
    ├── tuya_device.c		//应用入口文件
    └── tuya_dp_process.c	//DP点处理文件
```



 ### Demo信息 

|  产品ID  | bx4av5hcrcn9aaok |
| :------: | :--------------: |
| 芯片平台 |     BK7231N      |
| 编译环境 |                  |

  

### Demo入口

入口文件：tuya_device.c

重要函数：device_init()

+ 调用 tuya_iot_wf_soc_dev_init_param() 接口进行SDK初始化，配置了工作模式、配网模式，同时注册了各种回调函数并存入了固件key和PID。

+ 调用 tuya_iot_reg_get_wf_nw_stat_cb() 接口注册设备网络状态回调函数。

+ 调用应用层初始化函数 fan_init()

 

### DP点相关

+ 上报dp点接口: dev_report_dp_json_async()

| 函数名  | OPERATE_RET dev_report_dp_json_async(IN CONST CHAR_T *dev_id,IN CONST TY_OBJ_DP_S *dp_data,IN CONST UINT_T cnt) |
| ------- | ------------------------------------------------------------ |
| devid   | 设备id（若为网关、MCU、SOC类设备则devid = NULL;若为子设备，则devid = sub-device_id) |
| dp_data | dp结构体数组名                                               |
| cnt     | dp结构体数组的元素个数                                       |
| Return  | OPRT_OK: 成功  Other: 失败                                   |



### I/O 列表 

| BLDC 三明治开发板引脚 | 对应连接的CBU引脚 |
| :-------------------: | :---------------: |
|          VCC          |       3.3V        |
|          GND          |        GND        |
|         PWM1          |   PWM2/GPIOA_8    |

| 编码器 |     CBU     |
| :----: | :---------: |
|   N    | TY_GPIOA_6  |
|   A    | TY_GPIOA_26 |
|   B    | TY_GPIOA_9  |



## 相关文档

涂鸦Demo中心：https://developer.tuya.com/cn/demo/



## 技术支持

您可以通过以下方法获得涂鸦的支持:

- 开发者中心：https://developer.tuya.com
- 帮助中心: https://support.tuya.com/cn/help
- 技术支持工单中心: https://service.console.tuya.com
