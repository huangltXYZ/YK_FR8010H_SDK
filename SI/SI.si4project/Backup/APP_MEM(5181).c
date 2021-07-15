#include "YK_APP.h"


#define APP_MEM_PRINTF(...)    //printf(__VA_ARGS__)

/*重启定时器*/
os_timer_t s_Save_Timer;
static void Save_Timer_Handler(void *arg);

APP_MEM_DB gst_AppMemDB;/*整个系统掉电保存的内容*/

APP_MEM_TB gst_AppMemTB;


/*默认参数*/
const APP_MEM_DB st_MemDef = 
{
    .Provision  = PROV_TYPE_NONE,/*配网标志*/ 
    .ResetCnt   = 0,             /*复位次数*/
	.OTAWithOff = 0,             /*默认开灯*/
	.Light = 
	{
		.ble = 
		{
            .Level       = 100,                /*亮度等级*/
            .Temperature = YK_TEMPERATURE_MIN, /*色温*/
            .R           = 0,                  /*RGB 数值*/
            .G           = 0,
            .B           = 0,
            .LightMode   = LIGHT_MODE_CW,      /*灯运行模式 RGB or CW*/
		},
	},
    .ucCheckSum  = 0xFF,/*在写入的时候会计算校验*/
};

static void APP_MEM_VALUE_PRINTF(void);
static BOOL MEM_Erase(void);
static BOOL MEM_Read(void);
static BOOL MEM_Write(void);


/*****************************************************************************
Function name :  MEM_Erase
Description:	 擦除用户存储的扇区，除最后一个扇区
Input:	         void
Return:          TRUE->擦除成功; FALSE->擦除失败
Notes:    
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
static BOOL MEM_Erase(void)
{
	APP_MEM_PRINTF("MEM_Erase\r\n");
	
	flash_erase(NVM_UESR_DATA_ADDR, NVM_UESR_DATA_SIZE);

    gst_AppMemTB.usCurIndex = 0;

    return TRUE;
}

/*****************************************************************************
Function name :  MEM_Read
Description:	 读取指定索引的数据块内容
Input:	         void
Return:          TRUE->读取成功; FALSE->读取失败
Notes:    
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
static BOOL MEM_Read(void)
{
    U16 usIndex;
    U8 ReadBuff[RECORD_DATA_LEN];
    U8 ucPt;
    U8 ucCheckSum = 0;/*校验和*/

	//APP_MEM_PRINTF("MEM_Read\r\n");
	
    usIndex = gst_AppMemTB.usCurIndex - 1;/*获取当前指针索引的前一个*/
    
    /*读取RECORD_DATA_LEN字节*/
    flash_read(NVM_UESR_DATA_ADDR + (usIndex) * RECORD_DATA_LEN, RECORD_DATA_LEN, ReadBuff);
	
    /*校验数据*/
    for(ucPt = 0; ucPt < RECORD_DATA_LEN - 1; ucPt++)
    {
        ucCheckSum += ReadBuff[ucPt];
    }
    ucCheckSum ^= 0xFF;
    ucCheckSum += 0x5A;
    
    if(ucCheckSum == ReadBuff[RECORD_DATA_LEN - 1])
    {
        memcpy(&gst_AppMemDB, ReadBuff, RECORD_DATA_LEN);
        
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*****************************************************************************
Function name :  MEM_Write
Description:	 在当前索引存储数据
Input:	         void
Return:          TRUE->读取成功; FALSE->读取失败
Notes:    
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
static BOOL MEM_Write(void)
{
	U8 ReadBuff[RECORD_DATA_LEN];
    U8 WriteBuff[RECORD_DATA_LEN];

    U8 ucPt;
    U16 usIndex;
    U8 ucCheckSum = 0;/*校验和*/

	APP_MEM_PRINTF("MEM_Write\r\n");
	
    /*计算校验和*/
    memcpy(WriteBuff, &gst_AppMemDB, RECORD_DATA_LEN);

    for(ucPt = 0; ucPt < RECORD_DATA_LEN - 1; ucPt++)
    {
        ucCheckSum += WriteBuff[ucPt];
    }
    /*进行异或处理，防止全0等特数值通过校验*/
    ucCheckSum ^= 0xFF;
    ucCheckSum += 0x5A;
        
    gst_AppMemDB.ucCheckSum = ucCheckSum;
    WriteBuff[RECORD_DATA_LEN - 1] = ucCheckSum;
    
    /*判断是否是最后一条记录位置*/
    if(gst_AppMemTB.usCurIndex >= RECORD_DATA_NUM)
    {
        /*在备份扇区先写入当前数据，然后再擦除前面的存储记录扇区*/
		flash_erase(NVM_UESR_DATA_ADDR_BK, NVM_UESR_DATA_SIZE);

        /*写入RECORD_DATA_LEN字节*/
		flash_write(NVM_UESR_DATA_ADDR_BK, RECORD_DATA_LEN, WriteBuff);

        MEM_Erase();/*擦除前面的存储记录扇区*/
        /*在第0索引位置，存储数据*/
        gst_AppMemTB.usCurIndex = 0;
    }

    usIndex = gst_AppMemTB.usCurIndex;
    
    /*写入RECORD_DATA_LEN字节*/
	flash_write(NVM_UESR_DATA_ADDR + (usIndex) * RECORD_DATA_LEN, RECORD_DATA_LEN, WriteBuff);

	/*读取数据，比对是否写入成功*/
	flash_read(NVM_UESR_DATA_ADDR + (usIndex) * RECORD_DATA_LEN, RECORD_DATA_LEN, ReadBuff);

	if(0x00 == memcmp(WriteBuff, ReadBuff, RECORD_DATA_LEN))
	{
	    gst_AppMemTB.usCurIndex++;/*指向下一个存储记录索引*/
	    return TRUE;
	}
	else
	{
	    return FALSE;
	}   
}



/*****************************************************************************
Function name :  Save_Timer_Handler
Description:	 延时写定时器回调函数
Input:	         arg->回调参数    
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
static void Save_Timer_Handler(void *arg)
{
	YK_APP_Set_Evt(AppTaskTCB[APP_TASK_MEM].Handler, EVT_MEM_SAVE);
}

/*****************************************************************************
Function name :  APP_MEM_TimeProc
Description:	 APP MEM 模块的时基函数，需要在定时器中定期调用
Input:	         ulPeriod->函数被调用的周期（ms）		
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void APP_MEM_TimeProc(U32 ulPeriod)
{

}

/*****************************************************************************
Function name :  APP_MEM_Init
Description:	 APP MEM 模块的初始化函数，需要在上电初始化中调用
Input:	         void
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void APP_MEM_Init(void)
{
    U16 usPt = 0;
    
    memset((U8*)&gst_AppMemDB, 0, sizeof(gst_AppMemDB));	
    memset((U8*)&gst_AppMemTB, 0, sizeof(gst_AppMemTB));	

	/*创建延时保存软件定时器 单次3s*/
	os_timer_init(&s_Save_Timer, Save_Timer_Handler, PNULL);	
	
    /**************** 查找当前的存储索引 *************/
    /*依次读取每个记录的CRC*/
    for(usPt = 0; usPt < RECORD_DATA_NUM; usPt++)
    {
        gst_AppMemTB.usCurIndex = usPt + 1;/*Read函数内部会-1*/
        if(FALSE == MEM_Read())
        {
            /*喂狗*/
        #if BSP_WDT_USE
            BSP_WDT_Refresh();
        #endif
                
            /*如果 第0个扇区和备份扇区都CRC校验错误，则说明设备是第一次上电*/
            /*如果 备份扇区的数据是正确的，第0个索引数据不对，说明在卷绕的时候发生了掉电或者复位*/
            if(0x00 == usPt)
            {
                gst_AppMemTB.usCurIndex = RECORD_DATA_NUM + 1;/*索引指向备份扇区*/
                if(FALSE == MEM_Read())/*读取备份区域的数据*/
                {
                    APP_MEM_PRINTF("Device is First Run!\r\n");
                    memcpy((U8*)&gst_AppMemDB, (U8*)&st_MemDef, sizeof(gst_AppMemDB));
                }
                else
                {
                    /*已经从备份区域读出数据，这里不做其他处理*/
                    APP_MEM_PRINTF("Device Power Down During the Winding Process!\r\n"); 
                }

                /*刷新第0个索引位置 数据*/
                gst_AppMemTB.usCurIndex = 0;
                if(TRUE == MEM_Erase())
                {
                    MEM_Write();
                }
                /*打印数据*/
                APP_MEM_VALUE_PRINTF(); 
                
                return;
            }
            else
            {
                gst_AppMemTB.usCurIndex--;/*当前CRC校验错误，说明上一个索引是最终的数据*/
                MEM_Read();
                
                /*打印数据*/
                APP_MEM_VALUE_PRINTF(); 
                
                return;
            }
        }
    }
    /*如果用户记录区所有索引数据CRC都正确，那读取备份扇区的数据,*/
    /*如果备份区域数据正确,则表明在卷绕的时候掉电了，上次运行的时候备份区域数据来不及刷新到第0索引*/
    /*如果备份区域数据错误，则表明上次只写到最后一条的索引位置，没有发生卷绕，或者在写备份区域数据的时候掉电了，如果是这种情况我们也把那条数据作废，依旧采用用户索引的最后一条有效数据*/
    gst_AppMemTB.usCurIndex = RECORD_DATA_NUM + 1;/*索引指向备份扇区 + 1*/
    if(TRUE == MEM_Read())/*读取备份区域的数据*/
    {
        APP_MEM_PRINTF("Device Power Down During the Winding Process!\r\n"); 
        
        /*刷新第0个索引位置 数据*/
        gst_AppMemTB.usCurIndex = 0;
        if(TRUE == MEM_Erase())
        {
            MEM_Write();
        }
    }
    else
    {
		gst_AppMemTB.usCurIndex = RECORD_DATA_NUM;/*索引指向备份区域*/
        APP_MEM_PRINTF("Device In Winding Process or Wirte to ADDR_BK Power Down!\r\n"); 
        /*上次运行最后的那条记录在备份区域的时候，由于不完整，所以作废，采用倒数第二条数据（存储在最后一个索引的位置）*/
    }

	APP_MEM_VALUE_PRINTF();	
}

/*****************************************************************************
Function name :  APP_MEM_Task
Description:	 APP MEM 模块的任务函数
Input:	         param->任务消息
Return:          消息处理结果
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
int APP_MEM_Task(os_event_t *param)
{
	U8 Ret;	
	
	SHIELD_WARNING(Ret);
	
	switch(param->event_id)
	{
		case EVT_MEM_SAVE:
		{
		    /*喂狗*/
        #if BSP_WDT_USE
            BSP_WDT_Refresh();
        #endif
            
            if(TRUE == MEM_Write())
            {
                APP_MEM_PRINTF("MEM_Write Successfully!\r\n");
	    		APP_MEM_VALUE_PRINTF();
            }
            else
            {
                APP_MEM_PRINTF("MEM_Write Failure!\r\n");
            } 
		
		}break;
	
	}
	
	return EVT_CONSUMED;/*释放消息*/
}

/*****************************************************************************
Function name :  APP_MEM_VALUE_PRINTF
Description:	 打印从NVM中读取的数据
Input:           void
Return: 	     void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   ########  build this module
******************************************************************************/
static void APP_MEM_VALUE_PRINTF(void)
{
    U8 ucIndex;
    U8 ucLen;
    U8 MEM_Buff[RECORD_DATA_LEN];
    
    APP_MEM_PRINTF("\r\n");
    
    APP_MEM_PRINTF("usCurIndex = %d\r\n", gst_AppMemTB.usCurIndex);

    APP_MEM_PRINTF("MEM DATA: ");

    ucLen = RECORD_DATA_LEN;
    memcpy(MEM_Buff, &gst_AppMemDB , RECORD_DATA_LEN);
    for(ucIndex = 0; ucIndex < ucLen; ucIndex++)
    {
        APP_MEM_PRINTF("%2X ", *(MEM_Buff + ucIndex));
    }
	
    APP_MEM_PRINTF("\r\n");   
	
	APP_MEM_PRINTF("ResetCnt    = %d\r\n", gst_AppMemDB.ResetCnt);
	
	if(PROV_TYPE_NONE == gst_AppMemDB.Provision)
	{
		APP_MEM_PRINTF("Provision   = NONE\r\n");
		
		APP_MEM_PRINTF("Level       = %d\r\n", gst_AppMemDB.Light.ble.Level);
	    APP_MEM_PRINTF("Temperature = %d\r\n", gst_AppMemDB.Light.ble.Temperature);
	    APP_MEM_PRINTF("R           = %d\r\n", gst_AppMemDB.Light.ble.R);
	    APP_MEM_PRINTF("G           = %d\r\n", gst_AppMemDB.Light.ble.G);
	    APP_MEM_PRINTF("B           = %d\r\n", gst_AppMemDB.Light.ble.B);
	    APP_MEM_PRINTF("LightMode   = %d\r\n", gst_AppMemDB.Light.ble.LightMode);
	    APP_MEM_PRINTF("ucCheckSum  = %d\r\n", gst_AppMemDB.ucCheckSum);
	
	}
	else if(PROV_TYPE_WITH_GOOGLE == gst_AppMemDB.Provision)
	{
		APP_MEM_PRINTF("Provision   = Google\r\n");
		
		APP_MEM_PRINTF("Level       = %d\r\n", gst_AppMemDB.Light.ble.Level);
	    APP_MEM_PRINTF("Temperature = %d\r\n", gst_AppMemDB.Light.ble.Temperature);
	    APP_MEM_PRINTF("R           = %d\r\n", gst_AppMemDB.Light.ble.R);
	    APP_MEM_PRINTF("G           = %d\r\n", gst_AppMemDB.Light.ble.G);
	    APP_MEM_PRINTF("B           = %d\r\n", gst_AppMemDB.Light.ble.B);
	    APP_MEM_PRINTF("LightMode   = %d\r\n", gst_AppMemDB.Light.ble.LightMode);
	    APP_MEM_PRINTF("ucCheckSum  = %d\r\n", gst_AppMemDB.ucCheckSum);
	}
	
	else if(PROV_TYPE_WITH_AMANZON == gst_AppMemDB.Provision)
	{
		APP_MEM_PRINTF("Provision                     = Amazon\r\n");
		
		APP_MEM_PRINTF("dtt                           = %d\r\n", gst_AppMemDB.Light.mesh.dtt);
	    APP_MEM_PRINTF("onpowerup                     = %d\r\n", gst_AppMemDB.Light.mesh.onpowerup);
		
	    APP_MEM_PRINTF("lightness_default             = %d\r\n", gst_AppMemDB.Light.mesh.lightness_default);
	    APP_MEM_PRINTF("lightness_target              = %d\r\n", gst_AppMemDB.Light.mesh.lightness_target);
		APP_MEM_PRINTF("lightness_last                = %d\r\n", gst_AppMemDB.Light.mesh.lightness_last);
	    APP_MEM_PRINTF("lightness_range_min           = %d\r\n", gst_AppMemDB.Light.mesh.lightness_range_min);
	    APP_MEM_PRINTF("lightness_range_max           = %d\r\n", gst_AppMemDB.Light.mesh.lightness_range_max);

		
	    APP_MEM_PRINTF("temp_default                  = %d\r\n", gst_AppMemDB.Light.mesh.temp_default);
	    APP_MEM_PRINTF("temp_target                   = %d\r\n", gst_AppMemDB.Light.mesh.temp_target);
	    APP_MEM_PRINTF("temp_range_min                = %d\r\n", gst_AppMemDB.Light.mesh.temp_range_min);
		APP_MEM_PRINTF("temp_range_max                = %d\r\n", gst_AppMemDB.Light.mesh.temp_range_max);
		
	    APP_MEM_PRINTF("duv_default                   = %d\r\n", gst_AppMemDB.Light.mesh.duv_default);
	    APP_MEM_PRINTF("duv_target                    = %d\r\n", gst_AppMemDB.Light.mesh.duv_target);
		
		APP_MEM_PRINTF("hue_target                    = %d\r\n", gst_AppMemDB.Light.mesh.hue_target);
		APP_MEM_PRINTF("hue_range_min                 = %d\r\n", gst_AppMemDB.Light.mesh.hue_range_min);
	    APP_MEM_PRINTF("hue_range_max                 = %d\r\n", gst_AppMemDB.Light.mesh.hue_range_max);
		
		APP_MEM_PRINTF("saturation_target             = %d\r\n", gst_AppMemDB.Light.mesh.saturation_target);
		APP_MEM_PRINTF("saturation_range_min          = %d\r\n", gst_AppMemDB.Light.mesh.saturation_range_min);
	    APP_MEM_PRINTF("saturation_range_max          = %d\r\n", gst_AppMemDB.Light.mesh.saturation_range_max);		
		
		
	    APP_MEM_PRINTF("ucCheckSum                    = %d\r\n", gst_AppMemDB.ucCheckSum);	
	}
	else
	{
		APP_MEM_PRINTF("Provision = ERR\r\n");
	}
}


/*****************************************************************************
Function name :  APP_MEM_Test
Description:	 APP MEM 模块的测试函数，在正式代码中注释掉
Input:	         void
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void APP_MEM_Test(void)
{
	U16 usPt;
	
	gst_AppMemDB.Provision = PROV_TYPE_NONE;
	
	for(usPt = 0; usPt < RECORD_DATA_NUM + 10; usPt++)
	{
        /*喂狗*/
    #if BSP_WDT_USE
        BSP_WDT_Refresh();
    #endif
        
		
		gst_AppMemDB.Light.ble.Level = (U8)usPt;
		
		if(TRUE == MEM_Write())
        {
            APP_MEM_PRINTF("MEM_Write Successfully!\r\n");
			APP_MEM_VALUE_PRINTF();
        }
        else
        {
            APP_MEM_PRINTF("MEM_Write Failure!\r\n");
        }  
	}
}


/*****************************************************************************
Function name :  APP_MEM_Factory_Reset
Description:	 恢复出厂设置
Input:	         void
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void APP_MEM_Factory_Reset(void)
{
    memcpy((U8*)&gst_AppMemDB, (U8*)&st_MemDef, sizeof(gst_AppMemDB));

    /*喂狗*/
#if BSP_WDT_USE
    BSP_WDT_Refresh();
#endif
    
    /*刷新第0个索引位置 数据*/
    gst_AppMemTB.usCurIndex = 0;
    if(TRUE == MEM_Erase())
    {
        MEM_Write();
    }
    /*打印数据*/
    APP_MEM_VALUE_PRINTF(); 
}

/*****************************************************************************
Function name :  APP_MEM_Slow_Save
Description:	 慢速保存，需要等待3S
Input:	         void
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void APP_MEM_Slow_Save(void)
{
    os_timer_stop(&s_Save_Timer);
	os_timer_start(&s_Save_Timer, APP_MEM_WRITE_DELAY, false);
}

/*****************************************************************************
Function name :  APP_MEM_Fast_Save
Description:	 快速保存，不用等待3S
Input:	         void
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void APP_MEM_Fast_Save(void)
{
	/*喂狗*/
#if BSP_WDT_USE
   BSP_WDT_Refresh();
#endif
   
   if(TRUE == MEM_Write())
   {
	    APP_MEM_PRINTF("MEM_Write Successfully!\r\n");
        APP_MEM_VALUE_PRINTF();
   }
   else
   {
	   APP_MEM_PRINTF("MEM_Write Failure!\r\n");
   } 
}



