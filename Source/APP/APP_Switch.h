#ifndef __APP_SWITCH_H
#define __APP_SWITCH_H

#ifdef __cplusplus
 extern "C" {
#endif


#define SWITCH_TIMER_PER               (1000U)/*1S����*/
#define RESET_TIME_THR                 (5U)/*5S�ڸ�λ*/
#define LIGHT_SWITCH_ON_VALUE          (100U)/*�л��̼�ʱ�������*/

/*******�����л��̼�ǰ�����ȵĺ����������ȱ�����100%�����ֺ�BootLoader����һ��********/								 
#if (CUR_PROJECT_TYPE == PROJECT_TYPE_BLE)/*BLE*/
    #define    LIGHT_SWITCH_ON()     {APP_Light_SetLevel(LIGHT_SWITCH_ON_VALUE, 0); APP_Light_SetTemperature(0, 0, 0);}

#elif (CUR_PROJECT_TYPE == PROJECT_TYPE_MESH) /*MESH*/
    #define    LIGHT_SWITCH_ON()     update_light_cw(LIGHT_SWITCH_ON_VALUE, 100)
#endif

/*APP TC OTA ������ƿ�*/    
typedef struct
{
    /*ע������Ԫ�ط��ڽṹ�忪ͷ������洢*/
    U8 ucResetCnt;/*��λ����*/

    U8 ucRstTimeCnt;/*��λʱ�����*/
    
#if (CUR_PROJECT_TYPE == PROJECT_TYPE_BLE)/*BLE*/
    BOOL StartTimer;/*���������ʱ��*/
    U32 ulTimeCnt;/*����һ���ʱ��*/ 
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

