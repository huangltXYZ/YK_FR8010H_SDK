#include "YK_APP.h"

#define APP_Platfrom_PRINTF(...)    printf(__VA_ARGS__)


/*��λ���㶨ʱ��*/
#define APP_RESET_TIMEOUT      (TmrSeconds(5))/*��ʱ5Sд������*/

os_timer_t s_Reset_Timer;
static void Reset_Timer_Handler(void *arg);

static void Reset_Process(void);
static void BLE_Init(void);
static void MESH_Init(void);




/*****************************************************************************
Function name :  Reset_Timer_Handler
Description:	 ��λ���㶨ʱ���ص�����
Input:	         arg->�ص�����    
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
static void Reset_Timer_Handler(void *arg)
{
	gst_AppMemDB.ResetCnt = 0;
	APP_MEM_Fast_Save();
}


/*****************************************************************************
Function name :  Reset_Process
Description:	 ���ô���
Input:           void
Return: 	     void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   ########  build this module
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
		flash_erase(MESH_INFO_STORE_ADDR, 0x1000);
		
		APP_MEM_Factory_Reset();
		SYSTEM_RESET();/*����󶨺������豸*/        
	}
}
/*****************************************************************************
Function name :  BLE_Init
Description:	 BLE��س�ʼ��
Input:           void
Return: 	     void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   ########  build this module
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
Description:	 MESH��س�ʼ��
Input:           void
Return: 	     void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   ########  build this module
******************************************************************************/
static void MESH_Init(void)
{
	Light_Status_Default_Init();
	
	app_mesh_led_init();
	pb_remote_central_init();
	TC_OTA_gatt_add_service();
}


/*****************************************************************************
Function name :  APP_Platfrom_Init
Description:	 �����豸������״̬�����в�ͬ�ĳ�ʼ������
Input:           void
Return: 	     void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   ########  build this module
******************************************************************************/
void APP_Platfrom_Init(void)
{
	/*���ô���*/
	Reset_Process();
	
	/*������λ���������ʱ�� ����5s*/
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
	else/*�쳣��������в�������*/
	{
		gst_AppMemDB.Provision = PROV_TYPE_NONE;
	    APP_MEM_Fast_Save();
		
        MESH_Init();
        BLE_Init();		
		
		APP_Google_Init();
	}
}

/*****************************************************************************
Function name :  APP_Platfrom_Set_Mesh_Default_Light_Status
Description:	 ���豸��PROV_TYPE_NONE����PROV_TYPE_WITH_GOOGLE�е�PROV_TYPE_WITH_AMANZONʱ��ִ�иĺ���
Input:           void
Return: 	     void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   ########  build this module
******************************************************************************/
void APP_Platfrom_Set_Mesh_Default_Light_Status(void)
{
    MEM_DB_MESH.dtt = 0x00;
	
	MEM_DB_MESH.onpowerup = GENERIC_ON_POWERUP_RESTORE;
	
    MEM_DB_MESH.lightness_default   = LIGHTNESS_MAX;
    MEM_DB_MESH.lightness_last      = LIGHTNESS_MAX;;
    MEM_DB_MESH.lightness_target    = LIGHTNESS_MAX;
    MEM_DB_MESH.lightness_range_min = LIGHTNESS_MIN;
    MEM_DB_MESH.lightness_range_max = LIGHTNESS_MAX;
    
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
Description:	 Platfromģ����������
Input:           void
Return: 	     void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   ########  build this module
******************************************************************************/
void APP_Platfrom_Task(void)
{

}


/*****************************************************************************
Function name :  APP_Platfrom_TimeProc
Description:	 Platfromģ�鶨ʱ���ص�����
Input:           void
Return: 	     void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   ########  build this module
******************************************************************************/
void APP_Platfrom_TimeProc(U32 ulPeriod)
{


}


/*****************************************************************************
Function name :  APP_Platfrom_Set_Provision_Status
Description:	 ��ƽ̨����״̬   
Input:	         ProvType->�豸��������
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
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
Description:	 ��ȡ�豸����״̬   
Input:	         void
Return:          �豸��������
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
PROV_TYPE APP_Platfrom_Get_Provision_Status(void)
{
	return gst_AppMemDB.Provision;
}


