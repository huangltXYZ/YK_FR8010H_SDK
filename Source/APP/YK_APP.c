#include "YK_APP.h"

#define APP_PRINTF(...)    printf(__VA_ARGS__)

/*********************������Ʊ�************************************/
APP_Task_TCB AppTaskTCB[APP_TASK_MAX] = 
{
	{ NULL, APP_MEM_Task,    "APP_MEM_Task"    },/*APP_MEM_Task*/
    { NULL, APP_Google_Task, "APP_Google_Task" },/*APP_Google_Task*/
	{ NULL, APP_TC_OTA_Task, "APP_TC_OTA_Task" },/*APP_TC_OTA_Task*/
};


static void APP_TaskCreate(void);
//static void APP_KernelObjCreate(void);


/*����APP����*/
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

///*�����ں˶���*/
//static void APP_KernelObjCreate(void)
//{


//}

void YK_APP_Init(void)
{
	/*MEMģ���ʼ��*/
	APP_MEM_Init();
	
	//APP_MEM_Test();
	
	/*TC OTA �����ʼ��*/
    APP_TC_OTA_Init();  
	
	/*�豸����ƽ̨��ʼ��*/
	APP_Platfrom_Init();
    
	/*Light����ģ���ʼ��*/
	APP_Light_Init();	
	/*����APP����*/
	APP_TaskCreate();	
}






void YK_APP_Task(void)
{
	/*********TC OTA ����***************/
	//APP_TC_OTA_Task();	
}

    
/*************ϵͳʱ��ʱ���ص�����**********************/
void YK_APP_TimeProc(U32 ulPeriod)
{
//	static U16 usCnt = 0;
//	
//	if(++usCnt >= 1000)
//	{
//		usCnt -= 1000;
//	    APP_PRINTF("123\r\n");
//	}
//	/*MEMģ��ʱ��*/
//	APP_MEM_TimeProc(ulPeriod);
	
	/*Googleģ��ʱ��*/
	APP_Google_TimeProc(ulPeriod);
	
	/*Mesh Tidģ��ʱ��*/
	Mesh_Tid_TimeProc(ulPeriod);
}

/*����������¼���־��*/
void YK_APP_Set_Evt(U16 TaskId, U16 Evt)
{
	os_event_t cmd_event;
	
	cmd_event.event_id  = Evt;
	cmd_event.param     = PNULL;
	cmd_event.param_len = 0;
	
	os_msg_post(TaskId, &cmd_event);
}

