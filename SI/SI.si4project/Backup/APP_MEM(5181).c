#include "YK_APP.h"


#define APP_MEM_PRINTF(...)    //printf(__VA_ARGS__)

/*������ʱ��*/
os_timer_t s_Save_Timer;
static void Save_Timer_Handler(void *arg);

APP_MEM_DB gst_AppMemDB;/*����ϵͳ���籣�������*/

APP_MEM_TB gst_AppMemTB;


/*Ĭ�ϲ���*/
const APP_MEM_DB st_MemDef = 
{
    .Provision  = PROV_TYPE_NONE,/*������־*/ 
    .ResetCnt   = 0,             /*��λ����*/
	.OTAWithOff = 0,             /*Ĭ�Ͽ���*/
	.Light = 
	{
		.ble = 
		{
            .Level       = 100,                /*���ȵȼ�*/
            .Temperature = YK_TEMPERATURE_MIN, /*ɫ��*/
            .R           = 0,                  /*RGB ��ֵ*/
            .G           = 0,
            .B           = 0,
            .LightMode   = LIGHT_MODE_CW,      /*������ģʽ RGB or CW*/
		},
	},
    .ucCheckSum  = 0xFF,/*��д���ʱ������У��*/
};

static void APP_MEM_VALUE_PRINTF(void);
static BOOL MEM_Erase(void);
static BOOL MEM_Read(void);
static BOOL MEM_Write(void);


/*****************************************************************************
Function name :  MEM_Erase
Description:	 �����û��洢�������������һ������
Input:	         void
Return:          TRUE->�����ɹ�; FALSE->����ʧ��
Notes:    
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
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
Description:	 ��ȡָ�����������ݿ�����
Input:	         void
Return:          TRUE->��ȡ�ɹ�; FALSE->��ȡʧ��
Notes:    
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
static BOOL MEM_Read(void)
{
    U16 usIndex;
    U8 ReadBuff[RECORD_DATA_LEN];
    U8 ucPt;
    U8 ucCheckSum = 0;/*У���*/

	//APP_MEM_PRINTF("MEM_Read\r\n");
	
    usIndex = gst_AppMemTB.usCurIndex - 1;/*��ȡ��ǰָ��������ǰһ��*/
    
    /*��ȡRECORD_DATA_LEN�ֽ�*/
    flash_read(NVM_UESR_DATA_ADDR + (usIndex) * RECORD_DATA_LEN, RECORD_DATA_LEN, ReadBuff);
	
    /*У������*/
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
Description:	 �ڵ�ǰ�����洢����
Input:	         void
Return:          TRUE->��ȡ�ɹ�; FALSE->��ȡʧ��
Notes:    
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
static BOOL MEM_Write(void)
{
	U8 ReadBuff[RECORD_DATA_LEN];
    U8 WriteBuff[RECORD_DATA_LEN];

    U8 ucPt;
    U16 usIndex;
    U8 ucCheckSum = 0;/*У���*/

	APP_MEM_PRINTF("MEM_Write\r\n");
	
    /*����У���*/
    memcpy(WriteBuff, &gst_AppMemDB, RECORD_DATA_LEN);

    for(ucPt = 0; ucPt < RECORD_DATA_LEN - 1; ucPt++)
    {
        ucCheckSum += WriteBuff[ucPt];
    }
    /*�����������ֹȫ0������ֵͨ��У��*/
    ucCheckSum ^= 0xFF;
    ucCheckSum += 0x5A;
        
    gst_AppMemDB.ucCheckSum = ucCheckSum;
    WriteBuff[RECORD_DATA_LEN - 1] = ucCheckSum;
    
    /*�ж��Ƿ������һ����¼λ��*/
    if(gst_AppMemTB.usCurIndex >= RECORD_DATA_NUM)
    {
        /*�ڱ���������д�뵱ǰ���ݣ�Ȼ���ٲ���ǰ��Ĵ洢��¼����*/
		flash_erase(NVM_UESR_DATA_ADDR_BK, NVM_UESR_DATA_SIZE);

        /*д��RECORD_DATA_LEN�ֽ�*/
		flash_write(NVM_UESR_DATA_ADDR_BK, RECORD_DATA_LEN, WriteBuff);

        MEM_Erase();/*����ǰ��Ĵ洢��¼����*/
        /*�ڵ�0����λ�ã��洢����*/
        gst_AppMemTB.usCurIndex = 0;
    }

    usIndex = gst_AppMemTB.usCurIndex;
    
    /*д��RECORD_DATA_LEN�ֽ�*/
	flash_write(NVM_UESR_DATA_ADDR + (usIndex) * RECORD_DATA_LEN, RECORD_DATA_LEN, WriteBuff);

	/*��ȡ���ݣ��ȶ��Ƿ�д��ɹ�*/
	flash_read(NVM_UESR_DATA_ADDR + (usIndex) * RECORD_DATA_LEN, RECORD_DATA_LEN, ReadBuff);

	if(0x00 == memcmp(WriteBuff, ReadBuff, RECORD_DATA_LEN))
	{
	    gst_AppMemTB.usCurIndex++;/*ָ����һ���洢��¼����*/
	    return TRUE;
	}
	else
	{
	    return FALSE;
	}   
}



/*****************************************************************************
Function name :  Save_Timer_Handler
Description:	 ��ʱд��ʱ���ص�����
Input:	         arg->�ص�����    
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
static void Save_Timer_Handler(void *arg)
{
	YK_APP_Set_Evt(AppTaskTCB[APP_TASK_MEM].Handler, EVT_MEM_SAVE);
}

/*****************************************************************************
Function name :  APP_MEM_TimeProc
Description:	 APP MEM ģ���ʱ����������Ҫ�ڶ�ʱ���ж��ڵ���
Input:	         ulPeriod->���������õ����ڣ�ms��		
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void APP_MEM_TimeProc(U32 ulPeriod)
{

}

/*****************************************************************************
Function name :  APP_MEM_Init
Description:	 APP MEM ģ��ĳ�ʼ����������Ҫ���ϵ��ʼ���е���
Input:	         void
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void APP_MEM_Init(void)
{
    U16 usPt = 0;
    
    memset((U8*)&gst_AppMemDB, 0, sizeof(gst_AppMemDB));	
    memset((U8*)&gst_AppMemTB, 0, sizeof(gst_AppMemTB));	

	/*������ʱ���������ʱ�� ����3s*/
	os_timer_init(&s_Save_Timer, Save_Timer_Handler, PNULL);	
	
    /**************** ���ҵ�ǰ�Ĵ洢���� *************/
    /*���ζ�ȡÿ����¼��CRC*/
    for(usPt = 0; usPt < RECORD_DATA_NUM; usPt++)
    {
        gst_AppMemTB.usCurIndex = usPt + 1;/*Read�����ڲ���-1*/
        if(FALSE == MEM_Read())
        {
            /*ι��*/
        #if BSP_WDT_USE
            BSP_WDT_Refresh();
        #endif
                
            /*��� ��0�������ͱ���������CRCУ�������˵���豸�ǵ�һ���ϵ�*/
            /*��� ������������������ȷ�ģ���0���������ݲ��ԣ�˵���ھ��Ƶ�ʱ�����˵�����߸�λ*/
            if(0x00 == usPt)
            {
                gst_AppMemTB.usCurIndex = RECORD_DATA_NUM + 1;/*����ָ�򱸷�����*/
                if(FALSE == MEM_Read())/*��ȡ�������������*/
                {
                    APP_MEM_PRINTF("Device is First Run!\r\n");
                    memcpy((U8*)&gst_AppMemDB, (U8*)&st_MemDef, sizeof(gst_AppMemDB));
                }
                else
                {
                    /*�Ѿ��ӱ�������������ݣ����ﲻ����������*/
                    APP_MEM_PRINTF("Device Power Down During the Winding Process!\r\n"); 
                }

                /*ˢ�µ�0������λ�� ����*/
                gst_AppMemTB.usCurIndex = 0;
                if(TRUE == MEM_Erase())
                {
                    MEM_Write();
                }
                /*��ӡ����*/
                APP_MEM_VALUE_PRINTF(); 
                
                return;
            }
            else
            {
                gst_AppMemTB.usCurIndex--;/*��ǰCRCУ�����˵����һ�����������յ�����*/
                MEM_Read();
                
                /*��ӡ����*/
                APP_MEM_VALUE_PRINTF(); 
                
                return;
            }
        }
    }
    /*����û���¼��������������CRC����ȷ���Ƕ�ȡ��������������,*/
    /*�����������������ȷ,������ھ��Ƶ�ʱ������ˣ��ϴ����е�ʱ�򱸷���������������ˢ�µ���0����*/
    /*��������������ݴ���������ϴ�ֻд�����һ��������λ�ã�û�з������ƣ�������д�����������ݵ�ʱ������ˣ�����������������Ҳ�������������ϣ����ɲ����û����������һ����Ч����*/
    gst_AppMemTB.usCurIndex = RECORD_DATA_NUM + 1;/*����ָ�򱸷����� + 1*/
    if(TRUE == MEM_Read())/*��ȡ�������������*/
    {
        APP_MEM_PRINTF("Device Power Down During the Winding Process!\r\n"); 
        
        /*ˢ�µ�0������λ�� ����*/
        gst_AppMemTB.usCurIndex = 0;
        if(TRUE == MEM_Erase())
        {
            MEM_Write();
        }
    }
    else
    {
		gst_AppMemTB.usCurIndex = RECORD_DATA_NUM;/*����ָ�򱸷�����*/
        APP_MEM_PRINTF("Device In Winding Process or Wirte to ADDR_BK Power Down!\r\n"); 
        /*�ϴ���������������¼�ڱ��������ʱ�����ڲ��������������ϣ����õ����ڶ������ݣ��洢�����һ��������λ�ã�*/
    }

	APP_MEM_VALUE_PRINTF();	
}

/*****************************************************************************
Function name :  APP_MEM_Task
Description:	 APP MEM ģ���������
Input:	         param->������Ϣ
Return:          ��Ϣ������
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
int APP_MEM_Task(os_event_t *param)
{
	U8 Ret;	
	
	SHIELD_WARNING(Ret);
	
	switch(param->event_id)
	{
		case EVT_MEM_SAVE:
		{
		    /*ι��*/
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
	
	return EVT_CONSUMED;/*�ͷ���Ϣ*/
}

/*****************************************************************************
Function name :  APP_MEM_VALUE_PRINTF
Description:	 ��ӡ��NVM�ж�ȡ������
Input:           void
Return: 	     void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   ########  build this module
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
Description:	 APP MEM ģ��Ĳ��Ժ���������ʽ������ע�͵�
Input:	         void
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void APP_MEM_Test(void)
{
	U16 usPt;
	
	gst_AppMemDB.Provision = PROV_TYPE_NONE;
	
	for(usPt = 0; usPt < RECORD_DATA_NUM + 10; usPt++)
	{
        /*ι��*/
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
Description:	 �ָ���������
Input:	         void
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void APP_MEM_Factory_Reset(void)
{
    memcpy((U8*)&gst_AppMemDB, (U8*)&st_MemDef, sizeof(gst_AppMemDB));

    /*ι��*/
#if BSP_WDT_USE
    BSP_WDT_Refresh();
#endif
    
    /*ˢ�µ�0������λ�� ����*/
    gst_AppMemTB.usCurIndex = 0;
    if(TRUE == MEM_Erase())
    {
        MEM_Write();
    }
    /*��ӡ����*/
    APP_MEM_VALUE_PRINTF(); 
}

/*****************************************************************************
Function name :  APP_MEM_Slow_Save
Description:	 ���ٱ��棬��Ҫ�ȴ�3S
Input:	         void
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void APP_MEM_Slow_Save(void)
{
    os_timer_stop(&s_Save_Timer);
	os_timer_start(&s_Save_Timer, APP_MEM_WRITE_DELAY, false);
}

/*****************************************************************************
Function name :  APP_MEM_Fast_Save
Description:	 ���ٱ��棬���õȴ�3S
Input:	         void
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void APP_MEM_Fast_Save(void)
{
	/*ι��*/
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



