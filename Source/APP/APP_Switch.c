#include "YK_APP.h"



#define APP_SWITCH_PRINTF(...)    printf(__VA_ARGS__)


/*******抽象保存计数到FLASH的函数********/
#if (CUR_PROJECT_TYPE == PROJECT_TYPE_BLE)/*BLE*/
    #define SAVE_COUNT_TO_FLASH()    {\
    	                                 gst_AppMemDB.ResetCnt = gst_AppSwitchTb.ucResetCnt;\
                                         APP_MEM_Fast_Save();\
                                     }

#elif (CUR_PROJECT_TYPE == PROJECT_TYPE_MESH) /*MESH*/								 
    #define SAVE_COUNT_TO_FLASH()    save_on_flash(RESET_COUNTER)
#endif        
    
/*******抽象软件定时器的函数********/
#if (CUR_PROJECT_TYPE == PROJECT_TYPE_BLE)/*BLE*/
    #define APP_Switch_TIMER_START()    {\
    	                                    gst_AppSwitchTb.StartTimer = TRUE;\
                                            gst_AppSwitchTb.ulTimeCnt = 0;\
                                        }
	
								 
    #define APP_Switch_TIMER_STOP()     {gst_AppSwitchTb.StartTimer = FALSE;}
    
#elif (CUR_PROJECT_TYPE == PROJECT_TYPE_MESH) /*MESH*/								 
    #define APP_Switch_TIMER_START()    {\
    	                                    app_timer_create(&m_Switch_timer, APP_TIMER_MODE_REPEATED, Switch_timer_handler);\
                                            app_timer_stop(m_Switch_timer);\
								            app_timer_start(m_Switch_timer, HAL_MS_TO_RTC_TICKS(SWITCH_TIMER_PER), NULL);/*1S周期性*/  \
                                        }
						 
    #define APP_Switch_TIMER_STOP()    app_timer_stop(m_Switch_timer)
#endif

		
								 
/*******抽象配网标志********/								 
#if (CUR_PROJECT_TYPE == PROJECT_TYPE_BLE)/*BLE*/
    #define    PROVISIONED_STATUS    gst_AppMemDB.Provision    
#elif (CUR_PROJECT_TYPE == PROJECT_TYPE_MESH) /*MESH*/	
    extern bool Get_Device_Provisioned_Status(void);
    #define    PROVISIONED_STATUS    Get_Device_Provisioned_Status()    
#endif

/*抽象 喂狗 函数*/
#if (BSP_WDT_USE > 0u)
    #define WDT_CLEAR()     BSP_WDT_Refresh()   
#else
    #define WDT_CLEAR()  
#endif

 

/*抽象 串口初始化 函数*/
#define HAL_UART_Init()    BSP_UART_Init(UART_CHNL_HCI)

/*抽象  SPI初始化 函数*/
#define HAL_SPI_Init() BSP_SPI_Init(SPI_CHNL_FLASH)
   

			
#if (CUR_PROJECT_TYPE == PROJECT_TYPE_MESH) /*MESH*/
APP_TIMER_DEF(m_Switch_timer);
#endif


static void Update_Light_Default_State(void);
static void Remove_Provisioned_Process(void);
static void Switch_timer_handler(void * p_context);

APP_SWITCH_TB gst_AppSwitchTb;


/*****************************************************************************
Function name :  Update_Light_Default_State
Description:	 设置灯的状态为默认状态
Input:	         void
Return:          void
Notes:    抽象灯的默认状态的函数
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
static void Update_Light_Default_State(void)
{
	U32 ulValue;
    SPROM_INFO_DB stSpromInfoDB;
	
	SYS_UnlockReg();
    FMC_Open();
    FMC_ENABLE_ISP();
    FMC_ENABLE_SP_UPDATE();
    FMC_ENABLE_AP_UPDATE();
	
	memset(&stSpromInfoDB, 0x00, sizeof(SPROM_INFO_DB));
    ulValue = FMC_Read(SPROM_INFO_ADDR);
	FMC_Erase(SPROM_INFO_ADDR);
	memcpy(&stSpromInfoDB, &ulValue, sizeof(SPROM_INFO_DB));
	
#if (CUR_PROJECT_TYPE == PROJECT_TYPE_BLE)/*BLE*/
	U16 tmp = 0;
	if((OTA_WITH_OFF_FLAG == stSpromInfoDB.OTAWithOff) && (stSpromInfoDB.IsSwitch != SWITCH_FW_FLAG))/*关灯下的OTA*/
	{
        gst_AppGoogleTB.DeviceStatus.OnOff = 0x00;//开关状态为0
        gst_AppGoogleTB.DeviceStatus.Lightness = 0x00;
	}
	else/*设置灯的默认状态*/
	{
		if(gst_AppMemDB.LightMode)
		{
			APP_Light_SetLevel(gst_AppMemDB.Level, 0);
			APP_Light_SetTemperature(gst_AppMemDB.Temperature, 0, 0);
		}
		else
		{
			
	    #if (HW_RGB_ENABLE == 1)
			APP_Light_SetRGB(gst_AppMemDB.R, gst_AppMemDB.G, gst_AppMemDB.B, gst_AppMemDB.Level, 0);
	    #endif
			
		}
		gst_AppGoogleTB.DeviceStatus.OnOff = 0x01;//开关状态为1
        gst_AppGoogleTB.DeviceStatus.Lightness = gst_AppMemDB.Level;
	}

	if(gst_AppMemDB.Temperature == 0)
	{
        tmp = YK_TEMPERATURE_MIN;
	}
	else if(gst_AppMemDB.Temperature == 100)
	{
        tmp = YK_TEMPERATURE_MAX;
	}
	else
	{
        tmp = (YK_TEMPERATURE_MAX - YK_TEMPERATURE_MIN)*gst_AppMemDB.Temperature/100;
	}
	
	gst_AppGoogleTB.DeviceStatus.Temperature_H = tmp >> 8;
    gst_AppGoogleTB.DeviceStatus.Temperature_L = tmp & 0xff;
	
	gst_AppGoogleTB.DeviceStatus.R = gst_AppMemDB.R;//RGB更新状态
    gst_AppGoogleTB.DeviceStatus.G = gst_AppMemDB.G;
    gst_AppGoogleTB.DeviceStatus.B = gst_AppMemDB.B;

#elif (CUR_PROJECT_TYPE == PROJECT_TYPE_MESH) /*MESH*/
    if((OTA_WITH_OFF_FLAG == stSpromInfoDB.OTAWithOff) && (stSpromInfoDB.IsSwitch != SWITCH_FW_FLAG))/*关灯下的OTA*/
	{
	    void Set_light_To_Off(void);
	    Set_light_To_Off();
	}
	else
	{
	    update_light_state();
	}
#endif	

}

/*****************************************************************************
Function name :  Remove_Provisioned_Process
Description:	 解除绑定
Input:	         void
Return:          void
Notes:    
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
static void Remove_Provisioned_Process(void)
{
	APP_SWITCH_PRINTF("Remove_Provisioned_Process \r\n");
    
#if (CUR_PROJECT_TYPE == PROJECT_TYPE_BLE)/*BLE*/
    APP_MEM_Factory_Reset();
    
#elif (CUR_PROJECT_TYPE == PROJECT_TYPE_MESH) /*MESH*/
    ((tc_flash_erase_handler)SVC_TC_FLASH_ERASE)((uint32_t *)0x3C000,1024*4);
    ((tc_flash_erase_handler)SVC_TC_FLASH_ERASE)((uint32_t *)0x3D400,1024*1);
    ((tc_flash_erase_handler)SVC_TC_FLASH_ERASE)((uint32_t *)USER_FLASH_START_ADDR, USER_FLASH_SIZE);
#endif

    SYSTEM_RESET();/*解除绑定后重启设备*/         
}


/*****************************************************************************
Function name :  Switch_timer_handler
Description:	 Switch 定时器回调函数
Input:	         p_context->形参
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
static void Switch_timer_handler(void * p_context)
{
	gst_AppSwitchTb.ucRstTimeCnt++;
	//APP_SWITCH_PRINTF("\r\ns_ucShortCnt=%d \r\n", gst_AppSwitchTb.ucRstTimeCnt);

    /***************5S内复位则保持计数**************/
	if(RESET_TIME_THR == gst_AppSwitchTb.ucRstTimeCnt)
	{
        APP_SWITCH_PRINTF("Clear ucResetCnt \r\n");

        gst_AppSwitchTb.ucResetCnt = 0;
        SAVE_COUNT_TO_FLASH();

        APP_Switch_TIMER_STOP();
	}

}
		
/*****************************************************************************
Function name :  APP_Switch_Process
Description:	 切换固件流程处理
Input:	         void
Return:          void
Notes: 长按1次+短按4次触发切换固件流程
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void APP_Switch_Process(void)
{
    SWITCH_ERR Err;
    
#if (CUR_PROJECT_TYPE == PROJECT_TYPE_BLE)/*BLE*/
    memset(&gst_AppSwitchTb, 0x00, sizeof(APP_SWITCH_TB));
    gst_AppSwitchTb.ucResetCnt = gst_AppMemDB.ResetCnt;/*复位计数*/
#endif

    APP_SWITCH_PRINTF("ucResetCnt = %d\r\n", gst_AppSwitchTb.ucResetCnt);

    WDT_CLEAR();
    
    if(gst_AppSwitchTb.ucResetCnt < 4)
    {
        gst_AppSwitchTb.ucResetCnt++;
        SAVE_COUNT_TO_FLASH(); 
    }
    else/*开关次数达到5次*/
    {
        gst_AppSwitchTb.ucResetCnt = 0;
		SAVE_COUNT_TO_FLASH();
	
        /*首先判断当前是否处于绑定整体，如果是绑定状态，则执行解绑；如果是未绑定状态，则执行切换*/
		if(PROVISIONED_STATUS)
		{
            Remove_Provisioned_Process();
        }
        else
        {
			LIGHT_SWITCH_ON();/*切换固件前，常亮*/

			SYS_UnlockReg();
			 /*提高主频*/
            CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_DPLL_52M, CLK_CLKDIV_HCLK1);
			
		    HAL_UART_Init();/*重新初始化串口和SPI*/
			HAL_SPI_Init();
			
		    SYS_LockReg();
		
            DISABLE_IRQ();/*关闭全局中断*/
			
		#if (CUR_PROJECT_TYPE == PROJECT_TYPE_BLE)/*BLE*/
            APP_TC_OTA_FW_Switch_Proc(SWITCH_FW_TYPE_MESH, &Err);
        
		#elif (CUR_PROJECT_TYPE == PROJECT_TYPE_MESH) /*MESH*/
		    APP_TC_OTA_FW_Switch_Proc(SWITCH_FW_TYPE_BLE, &Err);
		#endif
        
            SYSTEM_RESET();/*切换失败则进行重启*/     
        }
    }

#if (CUR_PROJECT_TYPE == PROJECT_TYPE_BLE)/*BLE*/	
	/*适当增加延时，保持和MESH一样的亮灯速度*/
	Delay_ms(100);
#endif

    APP_Switch_TIMER_START(); 

/*******如果未配网，则闪烁3次********/
	APP_SWITCH_PRINTF("\r\nPROVISIONED_STATUS = %d\r\n", PROVISIONED_STATUS);
	if(PROVISIONED_STATUS)
	{
        Update_Light_Default_State();	
	}
	else
	{
         /*BLE工程跑产测的时候前5S常亮; MESH工程在start那边闪烁*/
	#if (CUR_PROJECT_TYPE == PROJECT_TYPE_BLE)/*BLE*/	
	    if(TRUE == gst_AppFactoryTestTB.EnableFactoryTest)
	    {
			Update_Light_Default_State();	
	    }
    #endif
	}	
}

#if (CUR_PROJECT_TYPE == PROJECT_TYPE_BLE)/*BLE*/
/*****************************************************************************
Function name :  APP_TC_OTA_TimeProc
Description:	 固件切换 模块的时基函数，需要在定时器中定期调用
Input:	         ulPeriod->函数被调用的周期（ms）		
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void APP_Switch_TimeProc(U32 ulPeriod)
{
    if(FALSE == gst_AppSwitchTb.StartTimer)
    {
        return;
    }

    gst_AppSwitchTb.ulTimeCnt += ulPeriod;

    if(gst_AppSwitchTb.ulTimeCnt >= SWITCH_TIMER_PER)
    {
        gst_AppSwitchTb.ulTimeCnt = 0;
        Switch_timer_handler(PNULL);
    }
}
#endif


