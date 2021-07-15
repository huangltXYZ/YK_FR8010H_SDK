#ifndef __YK_APP_H
#define __YK_APP_H




#ifdef __cplusplus
 extern "C" {
#endif

/************通用头文件**************/
#include "Common.h"

/************工程配置头文件**************/
#include "YK_Light_Config.h"
#include "MEM_MAP.h"

/************驱动头文件**************/
#include "BSP.h"

/************SDK相关头文件**************/
#include "sig_mesh_info.h"

#include "gap_api.h"
#include "gatt_api.h"

#include "gatt_sig_uuid.h"


#include "mesh_api.h"

#include "os_timer.h"
#include "os_mem.h"
#include "os_task.h"
#include "os_msg_q.h"

#include "sys_utils.h"
#include "jump_table.h"

void app_mesh_led_init(void);
void pb_remote_central_init(void);
void simple_peripheral_init(void);


#include "model_common.h"

#include "light_status.h"

#include "generic_onoff_server.h"
#include "generic_onoff_common.h"
#include "generic_onoff_messages.h"

#include "generic_dtt_server.h"
#include "generic_dtt_common.h"
#include "generic_dtt_messages.h"

#include "generic_ponoff_setup_server.h"
#include "generic_ponoff_common.h"
#include "generic_ponoff_messages.h"

#include "generic_level_server.h"
#include "generic_level_common.h"
#include "generic_level_messages.h"

#include "light_lightness_setup_server.h"
#include "light_lightness_messages.h"
#include "light_lightness_utils.h"

#include "light_ctl_setup_server.h"
#include "light_ctl_messages.h"
#include "light_ctl_utils.h"

#include "light_hsl_setup_server.h"
#include "light_hsl_common.h"
#include "light_hsl_messages.h"

#include "light_hsl_saturation_server.h"

#include "light_hsl_hue_server.h"

#include "firmware_update_common.h"
#include "firmware_update_messages.h"
#include "firmware_update_server.h"
#include "firmware_update_procedure_server.h"

#include "blob_transfer_common.h"
#include "blob_transfer_server.h"
#include "blob_transfer_messages.h"
#include "blob_procedure_server.h"


#include "Google_Service.h"
#include "TC_OTA_Service.h"
/************应用层相关头文件**************/
#include "APP_MEM.h"
#include "APP_TC_OTA.h"

#include "APP_Google.h"
#include "APP_Light.h"
#include "APP_Platfrom.h"
#include "APP_MESH.h"
#include "APP_BLE.h"

/*********************系统低功耗参数************************************/    

	
/*********************队列************************************/



/*********************事件标志组位************************************/
/*MEM Event definition*/
typedef enum 
{
	EVT_MEM_SAVE = 0x00,	

	EVT_MEM_MAX,

}APP_Event_MEM;

/*Google Event definition*/
typedef enum 
{
	EVT_Google_RECV_FRAME = 0x00,	
    EVT_Google_REBOOT,  
	
	EVT_Google_MAX,

}APP_Event_Google;


/*TC_OTA Event definition*/
typedef enum 
{
	EVT_TC_OTA_RECV_FRAME = 0x00,
	
	EVT_TC_OTA_MAX,

}APP_Event_TC_OTA;

#define MESH_MODEL_LOG(...)    printf(__VA_ARGS__)  

/*应用任务ID*/
typedef enum 
{
	APP_TASK_MEM = 0,	
    APP_TASK_GOOGLE,
	APP_TASK_TC_OTA,
	APP_TASK_MAX,	
	
}APP_TASK_ID;


/*******任务控制块*********/
typedef struct
{
    U16            Handler;/*任务句柄*/
    os_task_func_t TaskFun;/**任务函数*/
    const char*    TaskName;/*任务名字*/
	
}APP_Task_TCB;


/*Mesh元素定义*/
typedef enum
{
    MESH_ELMENT_MAIN = 0,     /*主元素*/
    MESH_ELMENT_TEMP,         /*Temp元素*/
    MESH_ELMENT_HUE,          /*HUE元素*/
    MESH_ELMENT_SATURATION, /*SATURATION元素*/
    
    MESH_ELMENT_MAX,
    
}MESH_ELMENT;




extern APP_Task_TCB AppTaskTCB[APP_TASK_MAX];

void YK_APP_Init(void);    
void YK_APP_Task(void);
void YK_APP_TimeProc(U32 ulPeriod);

void YK_APP_Set_Evt(U16 TaskId, U16 Evt);

#ifdef __cplusplus
}
#endif
  
   
#endif


