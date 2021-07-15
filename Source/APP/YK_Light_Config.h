#ifndef YK_Light_Config_H
#define YK_Light_Config_H

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

//<o> 产品型号 
//<1=> PID_TYPE_G_RGB_A19_8W  单芯片-谷歌-RGB-A19-8W 
//<2=> PID_TYPE_A_RGB_A19_8W  单芯片-亚马逊-RGB-A19-8W 
//<3=> PID_TYPE_G_CW_A19_8W   单芯片-谷歌-CW-A19-8W  
//<4=> PID_TYPE_A_CW_A19_8W   单芯片-亚马逊-CW-A19-8W 
//<5=> PID_TYPE_GA_RGB_A19_8W 双芯片-谷歌&亚马逊-RGB-A19-8W 
//<6=> PID_TYPE_GA_CW_A19_8W  双芯片-谷歌&亚马逊-CW-A19-8W

#define PID_TYPE 5

//<o> 支持的固件类型 
//<0=> TC_OTA_SUPPORT_FW_TYPE_G   谷歌
//<1=> TC_OTA_SUPPORT_FW_TYPE_A   亚马逊
//<2=> TC_OTA_SUPPORT_FW_TYPE_AG  双平台
#define SUPPORT_FW_TYPE 2

//<o> 当前运行的固件类型 
//<0=> TC_OTA_RUN_FW_TYPE_G 谷歌
//<1=> TC_OTA_RUN_FW_TYPE_A 亚马逊
#define RUN_FW_TYPE 0

/***************调光方式选择 PWM ****************/
#define DIMMER_WAY_PWM                (0x01)/*通过PWM方式调光*/
#define DIMMER_WAY_IIC_WITH_SM2135EJ  (0x02)/*通过IIC方式调光 SM2135EJ*/
#define DIMMER_WAY_IIC_WITH_BP1658CJ  (0x03)/*通过IIC方式调光 BP1658CJ*/
//<o> 调光方式选择 
//<1=> PWM调光方式
//<2=> IIC调光方式 通过SM2135EJ
//<3=> IIC调光方式 通过BP1658CJ
#define DIMMER 1

//<e> 是否支持RGB调光方式 
#define HW_RGB_ENABLE 1
//</e> 


/*色温范围,根据灯珠定义*/
// <o> 色温范围最小值
// <800-20000>
#define YK_TEMPERATURE_MIN   (2200U)

// <o> 色温范围最大值
// <800-20000>
#define YK_TEMPERATURE_MAX   (7500U)

/*WY 在2%-100%区间内调光*/
// <o> 冷暖模式下的最小线性调光系数
// <0-100>
#define YK_WY_LEVEL_MIN_COEFFICIENT   (2U)

// <o> 冷暖模式下的最大线性调光系数
// <0-100>
#define YK_WY_LEVEL_MAX_COEFFICIENT   (100U)


// <s>  设备名称
#define DEVICE_NAME  "ABCD"

#define DEVICE_NAME_LEN    sizeof(DEVICE_NAME)


// <o> Image Size
// <0-0x40000>
#define IMAGE_SIZE   (0x35000U)

// <o> Firmware Version
// <0-0xFFFFFFFF>
#define Firmware_Version   (0x1)

// <<< end of configuration section >>>

#endif
