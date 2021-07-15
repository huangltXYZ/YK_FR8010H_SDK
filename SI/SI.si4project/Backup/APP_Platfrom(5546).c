#include "YK_APP.h"

#define APP_Platfrom_PRINTF(...)    printf(__VA_ARGS__)


/*复位清零定时器*/
#define APP_RESET_TIMEOUT      (TmrSeconds(5))/*延时5S写入数据*/

os_timer_t s_Reset_Timer;
static void Reset_Timer_Handler(void *arg);

static void Reset_Process(void);
static void BLE_Init(void);
static void MESH_Init(void);




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
	if(gst_AppMemDB.ResetCnt < 4)
    {
        gst_AppMemDB.ResetCnt++;
        APP_MEM_Fast_Save(); 
    }
	else
	{
		APP_MEM_Factory_Reset();
		SYSTEM_RESET();/*解除绑定后重启设备*/        
	}
}
/*****************************************************************************
Function name :  BLE_Init
Description:	 BLE相关初始化
Input:           void
Return: 	     void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   ########  build this module
******************************************************************************/
static void BLE_Init(void)
{
    // Initialize security related settings.
    gap_security_param_t param =
    {
        .mitm = false,
        .ble_secure_conn = false,
        .io_cap = GAP_IO_CAP_NO_INPUT_NO_OUTPUT,
        .pair_init_mode = GAP_PAIRING_MODE_WAIT_FOR_REQ,
        .bond_auth = false,
        .password = 0,
    };
    
    gap_security_param_init(&param);

    gap_bond_manager_init(BLE_BONDING_INFO_SAVE_ADDR, BLE_REMOTE_SERVICE_SAVE_ADDR, 8, true);
    //gap_bond_manager_delete_all();
    
    mac_addr_t addr;
    gap_address_get(&addr);
    co_printf("Local BDADDR: 0x%2X%2X%2X%2X%2X%2X\r\n", addr.addr[0], addr.addr[1], addr.addr[2], addr.addr[3], addr.addr[4], addr.addr[5]);
    
    // Adding services to database
    Google_Gatt_Add_Service();  
	
	TC_OTA_gatt_add_service();
}

/*****************************************************************************
Function name :  MESH_Init
Description:	 MESH相关初始化
Input:           void
Return: 	     void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   ########  build this module
******************************************************************************/
static void MESH_Init(void)
{
	Light_Default_Status_Init();
	
	app_mesh_led_init();
	pb_remote_central_init();
	TC_OTA_gatt_add_service();
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
        MESH_Init();
		BLE_Init();
		
		APP_Google_Init();
	}
    else if(PROV_TYPE_WITH_GOOGLE == gst_AppMemDB.Provision)
	{
		BLE_Init();
		
		APP_Google_Init();
	}
	else if(PROV_TYPE_WITH_AMANZON == gst_AppMemDB.Provision)
	{
		MESH_Init();
	}
	else/*异常情况，进行参数纠正*/
	{
		gst_AppMemDB.Provision = PROV_TYPE_NONE;
	    APP_MEM_Fast_Save();
		
        MESH_Init();
        BLE_Init();		
		
		APP_Google_Init();
	}
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


