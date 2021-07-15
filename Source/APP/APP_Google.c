#include "YK_APP.h"

  


#define APP_Google_PRINTF(...)    printf(__VA_ARGS__)

#define PROVISIONED_STATUS    (PROV_TYPE_WITH_GOOGLE == APP_Platfrom_Get_Provision_Status())  

#define APP_Google_REBOOT_DELAY_TIME        (300U)/*��ʱ300ms������*/
#define APP_Google_BLINK_TIME               (500U)/*500MS ��������˸*/


APP_Google_TB gst_AppGoogleTB;

/*������ʱ��*/
os_timer_t s_Reboot_Timer;
static void Reboot_Timer_Handler(void *arg);
/*�豸����*/
static const DEVICE_TRAITS s_DeviceTraits = 
{
    .OnOff          = 1, /*֧�ֿ��ص�*/
    .Brightness     = 1, /*֧�����ȵ���*/  
    .Temperature    = 1, /*֧��ɫ�µ���*/
    
#if (HW_RGB_ENABLE == 1)
    .RGB            = 1, /*֧��RGB����*/
#else
    .RGB            = 0, /*��֧��RGB����*/
#endif
    
    .TemperatureMin = YK_TEMPERATURE_MIN,/*2000K ɫ����Сֵ ���*/
    .TemperatureMax = YK_TEMPERATURE_MAX,/*7500 ɫ�����ֵ ���*/
};

/*�豸����*/
static const U8 s_DeviceName[] = "A19 Full Color";

/*�豸�汾*/
static const U8 s_DeviceVersion[2] = {HW, SW};


static void GATT_SendData(U8 *SendBuff, U8 ucSendLen);



/************** �ȸ���Ƶ�Э�麯�������� *******************************/
typedef void (*APP_Google_Process)(U8*, U8, U8*, U8*);

static void Proc_OnOff(U8 *RcvBuff, U8 ucRcvLen,  U8 *SendBuff, U8 *pucSendLen);
static void Proc_AdjustBrightness(U8 *RcvBuff, U8 ucRcvLen,  U8 *SendBuff, U8 *pucSendLen);
static void Proc_AdjustTemperature(U8 *RcvBuff, U8 ucRcvLen,  U8 *SendBuff, U8 *pucSendLen);
static void Proc_Blink(U8 *RcvBuff, U8 ucRcvLen,  U8 *SendBuff, U8 *pucSendLen);

static void Proc_Provision(U8 *RcvBuff, U8 ucRcvLen,  U8 *SendBuff, U8 *pucSendLen);
static void Proc_UnProvision(U8 *RcvBuff, U8 ucRcvLen,  U8 *SendBuff, U8 *pucSendLen);

#if (HW_RGB_ENABLE == 1)
static void Proc_AdjustRGB(U8 *RcvBuff, U8 ucRcvLen,  U8 *SendBuff, U8 *pucSendLen);
#endif


static void ProtocolProcess(U8 *RcvBuff, U8 ucRcvLen, U8 *SendBuff, U8 *pucSendLen);

static const APP_Google_Process s_AppGoogleProcTbl[GOOGLE_PROT_CMD_MAX] =
{
	Proc_OnOff,              /*���ص�*/   
    Proc_AdjustBrightness,   /*��������*/
    Proc_AdjustTemperature,/*����ɫ��*/
    Proc_Blink,              /*��˸��*/
    Proc_Provision,          /*�󶨵�*/  
    Proc_UnProvision,        /*����*/
    
#if (HW_RGB_ENABLE == 1)
    Proc_AdjustRGB,          /*����RGBֵ*/
#else
    PNULL,                   /*�պ���*/
#endif

};

/*****************************************************************************
Function name :  Reboot_Timer_Handler
Description:	 ������ʱ���ص�����
Input:	         arg->�ص�����    
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
static void Reboot_Timer_Handler(void *arg)
{
	YK_APP_Set_Evt(AppTaskTCB[APP_TASK_GOOGLE].Handler, EVT_Google_REBOOT);
}

/*****************************************************************************
Function name :  Proc_OnOff
Description:	 ���صƿ���
Input:	         RcvBuff->���յ�������buff
	             ucRcvLen->���յ������ݳ���
	             SendBuff->�����͵����ݻ���
	             pucSendLen->�����͵����ݳ���
	             
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
static void Proc_OnOff(U8 *RcvBuff, U8 ucRcvLen,  U8 *SendBuff, U8 *pucSendLen)
{
    U8 OnOff;
    APP_Google_PRINTF("%s\r\n", __func__);

    OnOff = RcvBuff[1];

    if(0x00 == OnOff)/*�ص�*/
    {
        gst_AppGoogleTB.DeviceStatus.OnOff = 0x00;
        gst_AppGoogleTB.DeviceStatus.Lightness = 0x00;

        APP_Light_SetOnOff(0,0);
    
    }
    else if(OnOff)
    {
		gst_AppGoogleTB.DeviceStatus.OnOff = 1;
        gst_AppGoogleTB.DeviceStatus.Lightness = APP_Light_GetLevel();
        APP_Light_SetOnOff(1,0);
    }
    
    APP_Google_PRINTF("OnOff = %d\r\n", OnOff);
}

/*****************************************************************************
Function name :  Proc_AdjustBrightness
Description:	 ���ڵƵ�����
Input:	         RcvBuff->���յ�������buff
	             ucRcvLen->���յ������ݳ���
	             SendBuff->�����͵����ݻ���
	             pucSendLen->�����͵����ݳ���
	             
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
static void Proc_AdjustBrightness(U8 *RcvBuff, U8 ucRcvLen,  U8 *SendBuff, U8 *pucSendLen)
{
    U8 Brightness;
    APP_Google_PRINTF("%s\r\n", __func__);

    Brightness = RcvBuff[1];

    gst_AppGoogleTB.DeviceStatus.OnOff = 0x01;/*����״̬Ϊ1*/
    gst_AppGoogleTB.DeviceStatus.Lightness = Brightness;/*�ı�����*/

    APP_Light_SetLevel(Brightness, APP_Light_TRANSITION_TIME);
}

/*****************************************************************************
Function name :  Proc_AdjustTemperature
Description:	 ���ڵƵ�ɫ��
Input:	         RcvBuff->���յ�������buff
	             ucRcvLen->���յ������ݳ���
	             SendBuff->�����͵����ݻ���
	             pucSendLen->�����͵����ݳ���
	             
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
static void Proc_AdjustTemperature(U8 *RcvBuff, U8 ucRcvLen,  U8 *SendBuff, U8 *pucSendLen)
{
    U16 Temperature;
    U16 usMappingValue;
    
    APP_Google_PRINTF("%s\r\n", __func__);

    Temperature = RcvBuff[1] << 8 | (RcvBuff[2]);

    gst_AppGoogleTB.DeviceStatus.OnOff = 0x01;//����״̬Ϊ1
    gst_AppGoogleTB.DeviceStatus.Lightness = APP_Light_GetLevel();

    gst_AppGoogleTB.DeviceStatus.R = 0x00;//RGB����״̬
    gst_AppGoogleTB.DeviceStatus.G = 0x00;
    gst_AppGoogleTB.DeviceStatus.B = 0x00;
      
    gst_AppGoogleTB.DeviceStatus.Temperature_H = (Temperature >> 8) & 0xFF;
    gst_AppGoogleTB.DeviceStatus.Temperature_L = (Temperature >> 0) & 0xFF;
    
    APP_Light_SetTemperature(Temperature, 0, APP_Light_TRANSITION_TIME);

    APP_Google_PRINTF("Temperature = %d, MappingValue = %d\r\n", Temperature, usMappingValue);
}

/*****************************************************************************
Function name :  Proc_Blink
Description:	 ��˸��
Input:	         RcvBuff->���յ�������buff
	             ucRcvLen->���յ������ݳ���
	             SendBuff->�����͵����ݻ���
	             pucSendLen->�����͵����ݳ���	             
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
static void Proc_Blink(U8 *RcvBuff, U8 ucRcvLen,  U8 *SendBuff, U8 *pucSendLen)
{
    U8 Blink;

    APP_Google_PRINTF("%s\r\n", __func__);
    
    Blink = RcvBuff[1];
    
    if(Blink == 0x00)
	{
        gst_AppGoogleTB.StartBlink = FALSE;
		gst_AppGoogleTB.BlinkoOnOffFlag = 0;
        gst_AppGoogleTB.usBlinkTimeCnt = 0;
		APP_Light_SetOnOff(1,0);
	}
	else if(Blink == 0x01)
	{ 
        gst_AppGoogleTB.StartBlink = TRUE;
        gst_AppGoogleTB.BlinkoOnOffFlag = 0;
        gst_AppGoogleTB.usBlinkTimeCnt = 0;
    }
    
    APP_Google_PRINTF("Blink = %d\r\n", Blink);
}

/*****************************************************************************
Function name :  Proc_Provision
Description:	 �󶨵�
Input:	         RcvBuff->���յ�������buff
	             ucRcvLen->���յ������ݳ���
	             SendBuff->�����͵����ݻ���
	             pucSendLen->�����͵����ݳ���
	             
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
static void Proc_Provision(U8 *RcvBuff, U8 ucRcvLen,  U8 *SendBuff, U8 *pucSendLen)
{
    U8 Result;/*�󶨽��*/

    APP_Google_PRINTF("%s\r\n", __func__);
    
    Result = RcvBuff[1];
    
    if(Result == 0x00)
	{
		APP_MEM_Factory_Reset();
		
		os_timer_start(&s_Reboot_Timer, APP_Google_REBOOT_DELAY_TIME, false);
	}
	else if(Result == 0x01)
	{
		APP_Platfrom_Set_Provision_Status(PROV_TYPE_WITH_GOOGLE);
	}
    
    APP_Google_PRINTF("Result = %d\r\n", Result);
}

/*****************************************************************************
Function name :  Proc_UnProvision
Description:	 ����
Input:	         RcvBuff->���յ�������buff
	             ucRcvLen->���յ������ݳ���
	             SendBuff->�����͵����ݻ���
	             pucSendLen->�����͵����ݳ���
	             
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
static void Proc_UnProvision(U8 *RcvBuff, U8 ucRcvLen,  U8 *SendBuff, U8 *pucSendLen)
{
    U8 Result;/*�󶨽��*/

    APP_Google_PRINTF("%s\r\n", __func__);
    
    Result = RcvBuff[1];
    
    if(0x01 == Result)
	{
		APP_MEM_Factory_Reset();
        
		os_timer_start(&s_Reboot_Timer,APP_Google_REBOOT_DELAY_TIME, false);/*��ʱ300MS*/	
	}

    APP_Google_PRINTF("Result = %d\r\n", Result);
}

#if (HW_RGB_ENABLE == 1)
/*****************************************************************************
Function name :  Proc_AdjustRGB
Description:	 ���õƵ�RGBֵ
Input:	         RcvBuff->���յ�������buff
	             ucRcvLen->���յ������ݳ���
	             SendBuff->�����͵����ݻ���
	             pucSendLen->�����͵����ݳ���
	             
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
static void Proc_AdjustRGB(U8 *RcvBuff, U8 ucRcvLen,  U8 *SendBuff, U8 *pucSendLen)
{
    U8 R, G, B;
    
    APP_Google_PRINTF("%s\r\n", __func__);

    R = RcvBuff[1];
    G = RcvBuff[2];
    B = RcvBuff[3];

    gst_AppGoogleTB.DeviceStatus.OnOff = 0x01;//����״̬Ϊ1
    gst_AppGoogleTB.DeviceStatus.Lightness = APP_Light_GetLevel();

    gst_AppGoogleTB.DeviceStatus.R = R;//RGB����״̬
    gst_AppGoogleTB.DeviceStatus.G = G;
    gst_AppGoogleTB.DeviceStatus.B = B;

    APP_Light_SetRGB(R, G, B, APP_Light_GetLevel(), APP_Light_TRANSITION_TIME);
    
    APP_Google_PRINTF("R = %d, G = %d, B = %d\r\n", R, G, B);
}
#endif

/*****************************************************************************
Function name :  ProtocolProcess
Description:	 �ȸ���Ƶ�Э�鴦��
Input:	         RcvBuff->���յ�������buff
	             ucRcvLen->���յ������ݳ���
	             SendBuff->�����͵����ݻ���
	             pucSendLen->�����͵����ݳ���
	             
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/

static void ProtocolProcess(U8 *RcvBuff, U8 ucRcvLen, U8 *SendBuff, U8 *pucSendLen)
{
    U8 Cmd;
    U8 ucSendLen = 0;
    U8 ucParaLen = 0;//��������
    U8 ucPt;
	
	APP_Google_PRINTF("ProtocolProcess\r\n");
	
	APP_Google_PRINTF("RxLen = %d,RxData = \r\n", ucRcvLen);
	for(ucPt = 0; ucPt < ucRcvLen; ucPt++)
	{
	    APP_Google_PRINTF("%2X ", RcvBuff[ucPt]);
	}
	APP_Google_PRINTF("\r\n");
	
	Cmd = RcvBuff[0];
	
    if((Cmd > GOOGLE_PROT_CMD_MAX) || (ucRcvLen < APP_GOOGLE_PROT_LEN_MIN))
    {
        return;
    }

    /*�����δ��״̬���򲻴���ƵĿ������������˸/��/���ָ��*/
    if(((Cmd != GOOGLE_PROT_CMD_BLINK) && (Cmd != GOOGLE_PROT_CMD_PROVISION)  && (Cmd != GOOGLE_PROT_CMD_UNPROVISION))&& (0x00 == PROVISIONED_STATUS))
    {
        return;
    }
    
    /* �������������ö�Ӧ���������*/
    if(PNULL != s_AppGoogleProcTbl[Cmd])
    {
        s_AppGoogleProcTbl[Cmd](RcvBuff, ucRcvLen, SendBuff, &ucParaLen);
	
        /*���������˸/��/���ָ�����ָ���Ӧ����ס���ϱ��Ƶ�״ֵ̬*/
        if((Cmd != GOOGLE_PROT_CMD_BLINK) && (Cmd != GOOGLE_PROT_CMD_PROVISION)  && (Cmd != GOOGLE_PROT_CMD_UNPROVISION)) 
        {
            ucSendLen = sizeof(gst_AppGoogleTB.DeviceStatus);
            memcpy(SendBuff, &gst_AppGoogleTB.DeviceStatus, ucSendLen);
        }
        else
        {
            ucSendLen = 0x00;
        } 
    }
    *pucSendLen = ucSendLen;
	
	APP_Google_PRINTF("TxLen = %d,TxData = ", ucSendLen);
	for(ucPt = 0; ucPt < ucSendLen; ucPt++)
	{
	    APP_Google_PRINTF("%2X ", SendBuff[ucPt]);
	}
	APP_Google_PRINTF("\r\n");
}


/*****************************************************************************
Function name :  GATT_SendData
Description:	 ͨ��GATT��������
Input:	         SendBuff->���������ݵ�buff
Input:	         ucSendLen->���������ݵĳ���
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
static void GATT_SendData(U8 *SendBuff, U8 ucSendLen)
{ 
	gatt_ntf_t ntf_att;
	
	if(0x00 == g_ntf_Google_char6_Enable)
	{
		return;
	}
	ntf_att.att_idx  = GOOGLE_IDX_CHAR6_VALUE;
	ntf_att.conidx   = gst_AppGoogleTB.ConIdx;
	ntf_att.svc_id   = g_Google_Svc_ID;
	ntf_att.data_len = ucSendLen;
	ntf_att.p_data   = SendBuff;
	
	gatt_notification(ntf_att);
}


/*****************************************************************************
Function name :  APP_Google_TimeProc
Description:	 �ȸ�������� ģ���ʱ����������Ҫ�ڶ�ʱ���ж��ڵ���
Input:	         ulPeriod->���������õ����ڣ�ms��		
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void APP_Google_TimeProc(U32 ulPeriod)
{
    /***********��˸�ƴ���************/
    if(TRUE == gst_AppGoogleTB.StartBlink)
    {
        gst_AppGoogleTB.usBlinkTimeCnt += ulPeriod;
        if(gst_AppGoogleTB.usBlinkTimeCnt >=   APP_Google_BLINK_TIME)
        {
            gst_AppGoogleTB.usBlinkTimeCnt -=   APP_Google_BLINK_TIME;
            
            if(gst_AppGoogleTB.BlinkoOnOffFlag)
            {
				APP_Google_PRINTF("turn on\r\n");
				APP_Light_SetOnOff(1,0);
                gst_AppGoogleTB.BlinkoOnOffFlag = 0;
            }
            else
            {
				APP_Google_PRINTF("turn off\r\n");
				APP_Light_SetOnOff(0,0);
                gst_AppGoogleTB.BlinkoOnOffFlag = 1;
            }
        }
    }
}

/*****************************************************************************
Function name :  APP_Google_Init
Description:	 �ȸ������������ĳ�ʼ��
Input:	         void
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void APP_Google_Init(void)
{
    memset(&gst_AppGoogleTB, 0x00, sizeof(APP_Google_TB)); 

    gst_AppGoogleTB.pDeviceTraits = (DEVICE_TRAITS*)&s_DeviceTraits;
    gst_AppGoogleTB.pDeviceName = (U8*)&s_DeviceName;
    gst_AppGoogleTB.DeviceNameLen = sizeof(s_DeviceName) - 1;
    memcpy(gst_AppGoogleTB.DeviceVersion, s_DeviceVersion, sizeof(gst_AppGoogleTB.DeviceVersion));
	
	
	gst_AppGoogleTB.DeviceStatus.OnOff = APP_Light_GetOnoff();
	gst_AppGoogleTB.DeviceStatus.Lightness = APP_Light_GetLevel();
	gst_AppGoogleTB.DeviceStatus.Temperature_H = APP_Light_GetTemperature() >> 8;
	gst_AppGoogleTB.DeviceStatus.Temperature_L = APP_Light_GetTemperature();
	
	/*�������������ʱ�� ����300ms*/
	os_timer_init(&s_Reboot_Timer, Reboot_Timer_Handler, PNULL);	
}


/*****************************************************************************
Function name :  APP_TC_OTA_Task
Description:	 �ȸ������������ĺ���
Input:	         param->������Ϣ
Return:          ��Ϣ������
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
int APP_Google_Task(os_event_t *param)
{  
    U8 Ret;	
	
	SHIELD_WARNING(Ret);
	
	switch(param->event_id)
	{
		/**************Google������� Э�鴦��******************/
		case EVT_Google_RECV_FRAME:
		{
			ProtocolProcess(gst_AppGoogleTB.RxBuff, gst_AppGoogleTB.ucRxLen, gst_AppGoogleTB.TxBuff, &gst_AppGoogleTB.ucTxLen);

            if(gst_AppGoogleTB.ucTxLen > 0)
            {
                GATT_SendData(gst_AppGoogleTB.TxBuff, gst_AppGoogleTB.ucTxLen);
            }
		
		}break;
		
		/**************��������******************/
		case EVT_Google_REBOOT:
		{
			SYSTEM_RESET();
		
		}break;	
		
		default:
			break;
	}
	
	return EVT_CONSUMED;/*�ͷ���Ϣ*/
}

 /*****************************************************************************
Function name :  APP_Google_Start_Advertising
Description:	 �����㲥
Input:	         void
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void APP_Google_Start_Advertising(void)
{
  	// Set advertising parameters
    gap_adv_param_t adv_param;
    
	adv_param.adv_mode        = GAP_ADV_MODE_UNDIRECT;
    adv_param.adv_addr_type   = GAP_ADDR_TYPE_PUBLIC;
    adv_param.adv_intv_min    = 32;
    adv_param.adv_intv_max    = 50;
	adv_param.adv_chnl_map    = GAP_ADV_CHAN_ALL;
	adv_param.disc_mode       =  GAP_ADV_DISC_MODE_GEN_DISC;
    adv_param.adv_filt_policy = GAP_ADV_ALLOW_SCAN_ANY_CON_ANY;
        
    gap_set_advertising_param(&adv_param);
    
    
	
	/*������*/
    //uint8_t ad_data_unpro[] = {0x5,0x09,'B','G','1','2'};//unpro
    //uint8_t ad_data_pro[] = {0x8,0x09,'B','G','1','2','P','R','O'};//pro
	
    /*Set advertising data & scan response data*/ 
	U8 ad_data_unpro[] = {0x12,0x09,'E', 'n', 'e', 'r', 'g', 'e', 't', 'i', 'c', 'L', 'i', 'g', 'h', 't', 'i', 'n', 'g',
	                      0x04, 0xFF, PID_TYPE, SUPPORT_FW_TYPE, RUN_FW_TYPE};
	
    U8 ad_data_pro[] = {0x15, 0x09, 'E', 'n', 'e', 'r', 'g', 'e', 't', 'i', 'c', 'L', 'i', 'g', 'h', 't', 'i', 'n', 'g', 'P', 'R', 'O', 
	                    0x04, 0xFF, PID_TYPE, SUPPORT_FW_TYPE, RUN_FW_TYPE}; 
		
    U8 scan_data[] = {0x07, 0xFF, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
	                  0x07, 0xFE, 'Y', 'a', 'n', 'k', 'o', 'n',
	                  0x02, 0xFD, HW,
	                  0x02, 0xFC, SW,
	                  0x03, 0x03, 0x00, 0x18};

	//���Դ���
	APP_Platfrom_Set_Provision_Status(PROV_TYPE_NONE);	
					  
	if(PROVISIONED_STATUS)
	{
		gap_set_advertising_data(ad_data_pro, sizeof(ad_data_pro));
        APP_Google_PRINTF("pro\r\n");	
	}
	else
    {
		gap_set_advertising_data(ad_data_unpro, sizeof(ad_data_unpro));
		APP_Google_PRINTF("unpro\r\n");	
    }		

	mac_addr_t addr;
	gap_address_get(&addr);
	APP_Google_PRINTF("Local BDADDR: 0x%2X%2X%2X%2X%2X%2X\r\n", addr.addr[0], addr.addr[1], addr.addr[2], addr.addr[3], addr.addr[4], addr.addr[5]);
	scan_data[2] = addr.addr[5];
    scan_data[3] = addr.addr[4];
    scan_data[4] = addr.addr[3];
    scan_data[5] = addr.addr[2];
    scan_data[6] = addr.addr[1];
    scan_data[7] = addr.addr[0];				  
	gap_set_advertising_rsp_data(scan_data, sizeof(scan_data));
	
    // Start advertising
	APP_Google_PRINTF("Start advertising...\r\n");
	gap_start_advertising(0);
}




