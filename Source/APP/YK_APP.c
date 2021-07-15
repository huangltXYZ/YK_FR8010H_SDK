#include "YK_APP.h"

#define APP_PRINTF(...)    printf(__VA_ARGS__)

/*********************任务控制表************************************/
APP_Task_TCB AppTaskTCB[APP_TASK_MAX] = 
{
	{ NULL, APP_MEM_Task,    "APP_MEM_Task"    },/*APP_MEM_Task*/
    { NULL, APP_Google_Task, "APP_Google_Task" },/*APP_Google_Task*/
	{ NULL, APP_TC_OTA_Task, "APP_TC_OTA_Task" },/*APP_TC_OTA_Task*/
};


static void APP_TaskCreate(void);
//static void APP_KernelObjCreate(void);


/*创建APP任务*/
static void APP_TaskCreate(void)
{
    U8 ucIndex;
    
    for(ucIndex = 0; ucIndex < APP_TASK_MAX; ucIndex++)
    {
		AppTaskTCB[ucIndex].Handler = os_task_create(AppTaskTCB[ucIndex].TaskFun);
		
		if(0xFF == AppTaskTCB[ucIndex].Handler)
		{
			APP_PRINTF("APP_TaskCreate ucIndex = %d Error!\r\n", ucIndex);
		}
    }
}

///*创建内核对象*/
//static void APP_KernelObjCreate(void)
//{


//}

void YK_APP_Init(void)
{
	/*MEM模块初始化*/
	APP_MEM_Init();
	
	//APP_MEM_Test();
	
	/*TC OTA 任务初始化*/
    APP_TC_OTA_Init();  
	
	/*设备运行平台初始化*/
	APP_Platfrom_Init();
    
	/*Light控制模块初始化*/
	APP_Light_Init();	
	/*创建APP任务*/
	APP_TaskCreate();	
}






void YK_APP_Task(void)
{
	/*********TC OTA 任务***************/
	//APP_TC_OTA_Task();	
}

    
/*************系统时序定时器回调函数**********************/
void YK_APP_TimeProc(U32 ulPeriod)
{
//	static U16 usCnt = 0;
//	
//	if(++usCnt >= 1000)
//	{
//		usCnt -= 1000;
//	    APP_PRINTF("123\r\n");
//	}
//	/*MEM模块时基*/
//	APP_MEM_TimeProc(ulPeriod);
	
	/*Google模块时基*/
	APP_Google_TimeProc(ulPeriod);
	
	/*Mesh Tid模块时基*/
	Mesh_Tid_TimeProc(ulPeriod);
}

/*设置任务的事件标志组*/
void YK_APP_Set_Evt(U16 TaskId, U16 Evt)
{
	os_event_t cmd_event;
	
	cmd_event.event_id  = Evt;
	cmd_event.param     = PNULL;
	cmd_event.param_len = 0;
	
	os_msg_post(TaskId, &cmd_event);
}

