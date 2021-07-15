#include "YK_APP.h"

#define APP_Platfrom_PRINTF(...)    printf(__VA_ARGS__)


/*复位清零定时器*/
#define APP_RESET_TIMEOUT      (TmrSeconds(5))/*延时5S写入数据*/

os_timer_t s_Reset_Timer;
static void Reset_Timer_Handler(void *arg);

static void Reset_Process(void);


/*****************************************************************************
Function name :  Reset_Timer_Handler
Description:	 复位清零定时器回调函数
Input:	         arg->回调参数    
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
static void Reset_Timer_Handler(void *arg)
{
	APP_Platfrom_PRINTF("ResetCnt Clear\r\n");
	gst_AppMemDB.ResetCnt = 0;
	APP_MEM_Fast_Save();
}


/*****************************************************************************
Function name :  Reset_Process
Description:	 重置处理
Input:           void
Return: 	     void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   ########  build this module
******************************************************************************/
static void Reset_Process(void)
{
	APP_Platfrom_PRINTF("ResetCnt = %d\r\n", gst_AppMemDB.ResetCnt);
	if(gst_AppMemDB.ResetCnt < 4)
    {
        gst_AppMemDB.ResetCnt++;
        APP_MEM_Fast_Save(); 
    }
	else
	{
		APP_Platfrom_PRINTF("########### Reset_Process, Provision = %d\r\n", gst_AppMemDB.Provision);
		
	    //flash_erase(MESH_INFO_STORE_ADDR, 0x1000);
	    mesh_info_clear();

		APP_MEM_Factory_Reset();
		SYSTEM_RESET();/*解除绑定后重启设备*/        
	}
}


/*****************************************************************************
Function name :  APP_Platfrom_Init
Description:	 根据设备的配网状态，进行不同的初始化配置
Input:           void
Return: 	     void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   ########  build this module
******************************************************************************/
void APP_Platfrom_Init(void)
{
	/*重置处理*/
	Reset_Process();
	
	/*创建复位清零软件定时器 单次5s*/
	os_timer_init(&s_Reset_Timer, Reset_Timer_Handler, PNULL);	
	os_timer_start(&s_Reset_Timer, APP_RESET_TIMEOUT, false);
	
	if(PROV_TYPE_NONE == gst_AppMemDB.Provision)
	{
        APP_MESH_Init();
		APP_BLE_Init();
		
		APP_Google_Init();
	}
    else if(PROV_TYPE_WITH_GOOGLE == gst_AppMemDB.Provision)
	{
		APP_BLE_Init();
		
		APP_Google_Init();
	}
	else if(PROV_TYPE_WITH_AMANZON == gst_AppMemDB.Provision)
	{
		APP_MESH_Init();
	}
	else/*异常情况，进行参数纠正*/
	{
		gst_AppMemDB.Provision = PROV_TYPE_NONE;
	    APP_MEM_Fast_Save();
		
        APP_MESH_Init();
        APP_BLE_Init();		
		
		APP_Google_Init();
	}
}

/*****************************************************************************
Function name :  APP_Platfrom_Set_Mesh_Default_Light_Status
Description:	 当设备从PROV_TYPE_NONE或者PROV_TYPE_WITH_GOOGLE切到PROV_TYPE_WITH_AMANZON时，执行改函数
Input:           void
Return: 	     void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   ########  build this module
******************************************************************************/
void APP_Platfrom_Set_Mesh_Default_Light_Status(void)
{
	APP_Platfrom_PRINTF("%s\r\n",  __func__);
    MEM_DB_MESH.dtt = 0x00;
	
	MEM_DB_MESH.onpowerup = GENERIC_ON_POWERUP_RESTORE;
	
    MEM_DB_MESH.lightness_default   = LIGHTNESS_MAX;
    MEM_DB_MESH.lightness_last      = LIGHTNESS_MAX;;
    MEM_DB_MESH.lightness_target    = LIGHTNESS_MAX;
    MEM_DB_MESH.lightness_range_min = LIGHTNESS_MIN;
    MEM_DB_MESH.lightness_range_max = LIGHTNESS_MAX;
    MEM_DB_MESH.lightness_delta     = 0;
    
    MEM_DB_MESH.temp_default   = TEMP_MAX;
    MEM_DB_MESH.temp_target    = TEMP_MAX;
    MEM_DB_MESH.temp_range_min = TEMP_MIN;
    MEM_DB_MESH.temp_range_max = TEMP_MAX;
    MEM_DB_MESH.duv_default = DELTA_UV_DEF;
    MEM_DB_MESH.duv_target  = DELTA_UV_DEF;
    
    MEM_DB_MESH.hue_target    = HSL_HUE_MAX;
    MEM_DB_MESH.hue_range_min = HSL_HUE_MIN;
    MEM_DB_MESH.hue_range_max = HSL_HUE_MAX;
    
    MEM_DB_MESH.saturation_target    = HSL_SATURATION_MAX;
    MEM_DB_MESH.saturation_range_min = HSL_SATURATION_MIN;
    MEM_DB_MESH.saturation_range_max = HSL_SATURATION_MAX;	
	
    Light_Status_Default_Init();
	APP_MEM_Fast_Save();	
}

/*****************************************************************************
Function name :  APP_Platfrom_Task
Description:	 Platfrom模块任务处理函数
Input:           void
Return: 	     void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   ########  build this module
******************************************************************************/
void APP_Platfrom_Task(void)
{

}


/*****************************************************************************
Function name :  APP_Platfrom_TimeProc
Description:	 Platfrom模块定时器回调函数
Input:           void
Return: 	     void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   ########  build this module
******************************************************************************/
void APP_Platfrom_TimeProc(U32 ulPeriod)
{


}


/*****************************************************************************
Function name :  APP_Platfrom_Set_Provision_Status
Description:	 设平台配网状态   
Input:	         ProvType->设备配网类型
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void APP_Platfrom_Set_Provision_Status(PROV_TYPE ProvType)
{
	if(ProvType > PROV_TYPE_WITH_AMANZON)
	{
		return;
	}
	
	gst_AppMemDB.Provision = ProvType;
	APP_MEM_Fast_Save();
}

/*****************************************************************************
Function name :  APP_Platfrom_Get_Provision_Status
Description:	 获取设备配网状态   
Input:	         void
Return:          设备配网类型
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
PROV_TYPE APP_Platfrom_Get_Provision_Status(void)
{
	return gst_AppMemDB.Provision;
}


