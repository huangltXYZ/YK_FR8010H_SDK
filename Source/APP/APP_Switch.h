#ifndef __APP_SWITCH_H
#define __APP_SWITCH_H

#ifdef __cplusplus
 extern "C" {
#endif


#define SWITCH_TIMER_PER               (1000U)/*1S周期*/
#define RESET_TIME_THR                 (5U)/*5S内复位*/
#define LIGHT_SWITCH_ON_VALUE          (100U)/*切换固件时候的亮度*/

/*******抽象切换固件前的亮度的函数，此亮度必须是100%，保持和BootLoader亮度一致********/								 
#if (CUR_PROJECT_TYPE == PROJECT_TYPE_BLE)/*BLE*/
    #define    LIGHT_SWITCH_ON()     {APP_Light_SetLevel(LIGHT_SWITCH_ON_VALUE, 0); APP_Light_SetTemperature(0, 0, 0);}

#elif (CUR_PROJECT_TYPE == PROJECT_TYPE_MESH) /*MESH*/
    #define    LIGHT_SWITCH_ON()     update_light_cw(LIGHT_SWITCH_ON_VALUE, 100)
#endif

/*APP TC OTA 任务控制块*/    
typedef struct
{
    /*注：这里元素放在结构体开头，方便存储*/
    U8 ucResetCnt;/*复位计数*/

    U8 ucRstTimeCnt;/*复位时间计数*/
    
#if (CUR_PROJECT_TYPE == PROJECT_TYPE_BLE)/*BLE*/
    BOOL StartTimer;/*标记启动定时器*/
    U32 ulTimeCnt;/*控制一秒的时序*/ 
#endif   
  
}APP_SWITCH_TB;


void APP_Switch_Process(void);

#if (CUR_PROJECT_TYPE == PROJECT_TYPE_BLE)/*BLE*/
void APP_Switch_TimeProc(U32 ulPeriod);
#endif

extern APP_SWITCH_TB gst_AppSwitchTb;

#ifdef __cplusplus
}
#endif
  
   
#endif

