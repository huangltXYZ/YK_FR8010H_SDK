#ifndef __APP_Google_Protocol_H
#define __APP_Google_Protocol_H

#ifdef __cplusplus
 extern "C" {
#endif
	
#define APP_GOOGLE_PROT_LEN_MIN  (2U)

#define APP_GOOGLE_BUFF_LEN      (50U)


#define HW 3
#define SW 1

/*谷歌音响控制的命令码*/
typedef enum
{
    GOOGLE_PROT_CMD_ONOFF = 0x00,  /*开关灯*/   
    GOOGLE_PROT_CMD_ADJ_BRI,/*调节亮度*/
    GOOGLE_PROT_CMD_ADJ_TMP,/*调节色温*/
    GOOGLE_PROT_CMD_BLINK,/*闪烁灯*/
    GOOGLE_PROT_CMD_PROVISION, /*绑定灯*/   
    GOOGLE_PROT_CMD_UNPROVISION,/*解绑灯*/
    GOOGLE_PROT_CMD_ADJ_RGB,/*设置RGB值*/

    GOOGLE_PROT_CMD_MAX,

}GOOGLE_PROT_CMD;


/*上报给 Google音响的设备能力 特性*/    
typedef struct
{
    U8   OnOff;          /*是否支持开关灯*/
	U8   Brightness;     /*是否支持亮度调节*/
	U8   Temperature;    /*是否支持色温调节*/
	U8   RGB;            /*是否支持RGB调节*/
    U16  TemperatureMin;/*色温最小值 大端*/
    U16  TemperatureMax;/*色温最大值 大端*/

}DEVICE_TRAITS;


/*上报给 Google音响的设备状态*/    
__packed typedef struct
{
    U8 OnOff;/*开关状态*/
	U8 Lightness; /*亮度状态*/
	U8 Temperature_H;  /*色温高字节*/
	U8 Temperature_L;  /*色温低字节*/
    U8 R;/*R Value*/
    U8 G;/*G Value*/
    U8 B;/*B Value*/

}DEVICE_STATUS;



/*APP Google 任务控制块*/    
typedef struct
{
    U8   TxBuff[APP_GOOGLE_BUFF_LEN];
	U8   ucTxLen;
	U8   RxBuff[APP_GOOGLE_BUFF_LEN];
	U8   ucRxLen;
    DEVICE_TRAITS *pDeviceTraits;/*设备能力*/
    DEVICE_STATUS DeviceStatus;/*设备状态*/
    U8 *pDeviceName;/*设备名称*/
    U8 DeviceNameLen;
    U8 DeviceVersion[2];/*设备版本*/

    U8 ConIdx;
    U8 AttIdx;

    U8 StartBlink;/*闪烁标志*/
    U8 BlinkoOnOffFlag;/*闪烁时灯的开关状态*/
    U16 usBlinkTimeCnt;
    
    
}APP_Google_TB;

extern APP_Google_TB gst_AppGoogleTB;

void APP_Google_TimeProc(U32 ulPeriod);
void APP_Google_Init(void);
int APP_Google_Task(os_event_t *param);
void APP_Google_Start_Advertising(void);

#ifdef __cplusplus
}
#endif
  
   
#endif

