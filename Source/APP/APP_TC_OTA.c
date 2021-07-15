#include "YK_APP.h"

/*���� ι�� ����*/
#if (BSP_WDT_USE > 0u)
#define WDT_CLEAR()     wdt_feed()
#else
#define WDT_CLEAR()
#endif


#define APP_TC_OTA_PRINTF(...)    printf(__VA_ARGS__)

/*�ⲿ��������*/
void enable_cache(uint8_t invalid_ram);
void disable_cache(void);


/*APP TC OTA ������ƿ�*/
APP_TC_OTA_TB gst_AppTCOTATB;

os_timer_t ota_reboot_timer;

/************** TC OTA Э�鴦���������� *******************************/
typedef void (*APP_TCOTA_Process)(U8*, U8, U8*, U8*);

static void Proc_SyncFwVersion(U8 *RcvBuff, U8 ucRcvLen,  U8 *SendBuff, U8 *pucSendLen);
static void Proc_StartOTA(U8 *RcvBuff, U8 ucRcvLen,  U8 *SendBuff, U8 *pucSendLen);
static void Proc_Confirm(U8 *RcvBuff, U8 ucRcvLen,  U8 *SendBuff, U8 *pucSendLen);
static void Proc_Reset(U8 *RcvBuff, U8 ucRcvLen,  U8 *SendBuff, U8 *pucSendLen);
static void Proc_GetFwInfo(U8 *RcvBuff, U8 ucRcvLen,  U8 *SendBuff, U8 *pucSendLen);
static void Proc_ImageUpdate(U8 *RcvBuff, U8 ucRcvLen,  U8 *SendBuff, U8 *pucSendLen);
static void ProtocolProcess(U8 *RcvBuff, U8 ucRcvLen,  U8 *SendBuff, U8 *pucSendLen);

static uint8_t crc_cal(uint32_t firmware_length,uint32_t new_bin_addr,uint32_t crc_data_t);

static void OTA_Param_Init(void);
static BOOL OTA_Image_Update(U8 *pData, U16 usLen);
static void GATT_SendData(U8 *SendBuff, U8 ucSendLen);
static void Reboot_timer_handler(void * p_context);

static const APP_TCOTA_Process s_AppTCOTAProcTbl[TC_OTA_PROT_CMD_MAX] =
{
    PNULL,
    Proc_SyncFwVersion,     /*ͬ���̼��汾*/
    Proc_StartOTA,          /*����OTA*/
    Proc_Confirm,           /*�̼�У��*/
    Proc_Reset,             /*��λ�豸*/

#if (APP_TC_OTA_FW_SWITCH_EN)
    Proc_FwSwitch,          /*�л��̼�*/
#else
    PNULL,
#endif
    Proc_GetFwInfo,         /*��ȡ�̼��汾��Ϣ*/
    Proc_ImageUpdate,       /*����̼� ע������ʵ��Э����û�и������룬������Ϊ��д���뷽��*/
};

__attribute__((section("ram_code")))static uint8_t crc_cal(uint32_t firmware_length,uint32_t new_bin_addr,uint32_t crc_data_t)
{
    uint32_t crc_data = 0xffffffff,i = 0;
    uint16_t crc_packet_num = (firmware_length-256)/256;
    uint32_t crc32_check_addr = new_bin_addr+256;
    uint8_t * crc_check_data = os_malloc(256);
    uint8_t ret = 0;
    uint32_t current_remap_address, remap_size;

    APP_TC_OTA_PRINTF("crc32_check_addr: 0x%x\r\n",crc32_check_addr);

    if(PNULL == crc_check_data)
    {
        return 0;
    }

    /*�ȼ������RAM�е�ǰ256�ֽ�CRC*/
    crc_data =  Crc32CalByte(crc_data, gst_AppTCOTATB.FirstPageData, 256);

    /*�ټ������flash�е�ʣ���ļ���CRC*/
    for( i = 0; i < crc_packet_num; i++)
    {
        current_remap_address = system_regs->remap_virtual_addr;
        remap_size = system_regs->remap_length;
        GLOBAL_INT_DISABLE();

        system_regs->remap_virtual_addr = 0;
        system_regs->remap_length = 0;
		
        disable_cache();
        flash_read((crc32_check_addr+256*i), 256, crc_check_data);
        enable_cache(true);

        system_regs->remap_virtual_addr = current_remap_address;
        system_regs->remap_length = remap_size;
        GLOBAL_INT_RESTORE();

        crc_data = Crc32CalByte(crc_data, crc_check_data, 256);
    }
	
    firmware_length -= (256 * (crc_packet_num + 1));
    if(firmware_length > 0)
    {
        //uint32_t current_remap_address, remap_size;
        current_remap_address = system_regs->remap_virtual_addr;
        remap_size = system_regs->remap_length;
        GLOBAL_INT_DISABLE();
		
        system_regs->remap_virtual_addr = 0;
        system_regs->remap_length = 0;
		
        disable_cache();
        flash_read((crc32_check_addr+256*i),firmware_length,crc_check_data);
        enable_cache(true);
		
        system_regs->remap_virtual_addr = current_remap_address;
        system_regs->remap_length = remap_size;
        GLOBAL_INT_RESTORE();
		
        crc_data =  Crc32CalByte(crc_data, crc_check_data, firmware_length);
    }
    APP_TC_OTA_PRINTF("crc_data= %x\r\n",crc_data);

    crc_data ^= 0xFFFFFFFF;
	
    os_free(crc_check_data);
	
    if(crc_data_t == crc_data)
	{
	    ret = 1;
	}
        
    return ret;
}


/*******************************************************************
��������: Reboot_timer_handler
����˵��: Reboot��ʱ���Ļص�����
���˵��: RcvBuff->���յ�������buff
        ucRcvLen->���յ������ݳ���
        SendBuff->�����͵����ݻ���
        pucParaLen->��������
����ֵ˵��: void

--------------------------------------------------------------------
���ļ�¼    |     ������     |    ��������    |     ����˵��       |
--------------------------------------------------------------------
 V1.00      |      hlt        |   2021-04-08    |     ��������       |
--------------------------------------------------------------------
*******************************************************************/
static void Reboot_timer_handler(void * p_context)
{
    SYSTEM_RESET();
}

/*****************************************************************************
Function name :  OTA_Param_Init
Description:	 OTA ������ʼ��
Input:	         OTAType->׼���·���OTA�̼�����
Return:          void
Notes: Ƭ��FLASH�Ĳ�����START OTA�������Ѿ�ִ��
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
static void OTA_Param_Init()
{
    gst_AppTCOTATB.ulAddrOffset = 0;
    gst_AppTCOTATB.ulAddrOffsetMax = 0;
    gst_AppTCOTATB.FirstPageReceiveLen = 0;
    memset(&gst_AppTCOTATB.FwInfoCur, 0, sizeof(TC_OTA_FW_INFO));
    memset(&gst_AppTCOTATB.FwInfoNew, 0, sizeof(TC_OTA_FW_INFO));
}

/*****************************************************************************
Function name :  OTA_Image_Update
Description:	 ���¹̼�����
Input:	         pData->�̼�����
Input:	         usLen->��Ӧ���Ĺ̼����ݳ���
Return:          TRUE->һ�����ݸ��³ɹ���FALSE->����ʧ��
Notes: Ƭ��FLASH�Ĳ�����START OTA�������Ѿ�ִ��
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
static BOOL OTA_Image_Update(U8 *pData, U16 usLen)
{
//	U8 rd_buf[16],i;

    /*�̼������ж�*/
    if( (0x00 == gst_AppTCOTATB.FwInfoNew.size) || ( gst_AppTCOTATB.FwInfoNew.size > 0x30000 ))
    {
        return FALSE;
    }

    if(gst_AppTCOTATB.ulAddrOffset + usLen > gst_AppTCOTATB.ulAddrOffsetMax)
    {
        APP_TC_OTA_PRINTF("Out of Range Cur Offset = %d, OffsetMax = %d\r\n", gst_AppTCOTATB.ulAddrOffset + usLen, gst_AppTCOTATB.ulAddrOffsetMax);
        return FALSE;
    }

    APP_TC_OTA_PRINTF("ota_write to %d,len %d",gst_AppTCOTATB.OTAFwAddr+gst_AppTCOTATB.ulAddrOffset,usLen);
//	co_printf("write:\r\n");
//  show_reg(pData,16,1);
    app_otas_save_data(gst_AppTCOTATB.OTAFwAddr + gst_AppTCOTATB.ulAddrOffset, (U8*)pData, usLen);

//	flash_read(storage_address+gst_AppTCOTATB.ulAddrOffset,16,(U8*)rd_buf);
//	co_printf("read:\r\n");
//  show_reg(rd_buf,16,1);
    gst_AppTCOTATB.ulAddrOffset += usLen;


    return TRUE; // Success
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
    APP_TC_OTA_PRINTF("%s %s\r\n",__FILE__,__func__);
    if(g_ntf_TC_OTA_Ctrl_Enable)
    {
        ntf_att.att_idx  = TC_OTA_IDX_CTRL_VALUE;
        ntf_att.conidx   = gst_AppTCOTATB.ConIdx;
        ntf_att.svc_id   = g_TC_OTA_Svc_ID;
        ntf_att.data_len = ucSendLen;
        ntf_att.p_data   = SendBuff;

        gatt_notification(ntf_att);
    }
    else
    {
        APP_TC_OTA_PRINTF("g_ntf_TC_OTA_Ctrl_Enable==0\r\n");
    }
}

/*******************************************************************
��������: Proc_SyncFwVersion
����˵��: ͬ���̼��汾
���˵��: RcvBuff->���յ�������buff
        ucRcvLen->���յ������ݳ���
        SendBuff->�����͵����ݻ���
        pucParaLen->��������
����ֵ˵��: void

--------------------------------------------------------------------
���ļ�¼    |     ������     |    ��������    |     ����˵��       |
--------------------------------------------------------------------
 V1.00      |      hlt        |   2021-04-08    |     ��������       |
--------------------------------------------------------------------
*******************************************************************/
static void Proc_SyncFwVersion(U8 *RcvBuff, U8 ucRcvLen,  U8 *SendBuff, U8 *pucParaLen)
{
    U8 ucIndex = APP_TC_OTA_PROT_RSP_PARA_OFFSET;

    APP_TC_OTA_PRINTF("Proc_SyncFwVersion\r\n");

//    OTA_Param_Init();

    gst_AppTCOTATB.FwInfoNew.version.appliction = co_read16p(&RcvBuff[1]);
    gst_AppTCOTATB.FwInfoNew.version.softdevice = co_read16p(&RcvBuff[3]);
    gst_AppTCOTATB.FwInfoNew.version.project    = co_read16p(&RcvBuff[5]);
    gst_AppTCOTATB.FwInfoNew.flag               = 0;
    gst_AppTCOTATB.FwInfoNew.type               = RcvBuff[8];

    //test only
    gst_AppTCOTATB.FwInfoCur.version.appliction = __jump_table.firmware_version;
    gst_AppTCOTATB.FwInfoCur.version.softdevice = 0;
    gst_AppTCOTATB.FwInfoCur.version.project    = PID_TYPE;


    SendBuff[ucIndex++] = gst_AppTCOTATB.FwInfoCur.version.appliction;
    SendBuff[ucIndex++] = gst_AppTCOTATB.FwInfoCur.version.appliction >> 8;
    SendBuff[ucIndex++] = gst_AppTCOTATB.FwInfoCur.version.softdevice;
    SendBuff[ucIndex++] = gst_AppTCOTATB.FwInfoCur.version.softdevice >> 8;
    SendBuff[ucIndex++] = gst_AppTCOTATB.FwInfoCur.version.project;
    SendBuff[ucIndex++] = gst_AppTCOTATB.FwInfoCur.version.project >> 8;
    SendBuff[ucIndex++] = 0;
    SendBuff[ucIndex++] = CUR_FW_TYPE_BLE;//gst_AppTCOTATB.FwInfoCur.type;

    *pucParaLen = ucIndex - APP_TC_OTA_PROT_RSP_PARA_OFFSET;

    gap_conn_param_update(gst_AppTCOTATB.ConIdx, 12, 12, 0, 500);
}

/*******************************************************************
��������: Proc_StartOTA
����˵��: ����OTA
���˵��: RcvBuff->���յ�������buff
        ucRcvLen->���յ������ݳ���
        SendBuff->�����͵����ݻ���
        pucParaLen->��������
����ֵ˵��: void

--------------------------------------------------------------------
���ļ�¼    |     ������     |    ��������    |     ����˵��       |
--------------------------------------------------------------------
 V1.00      |      hlt        |   2021-04-08    |     ��������       |
--------------------------------------------------------------------
*******************************************************************/
static void Proc_StartOTA(U8 *RcvBuff, U8 ucRcvLen,  U8 *SendBuff, U8 *pucParaLen)
{
    U32 offset;
    U32 ulLen_4K;

    U8 ucIndex = APP_TC_OTA_PROT_RSP_PARA_OFFSET;

    APP_TC_OTA_PRINTF("Proc_StartOTA\r\n");

    OTA_Param_Init();

    gst_AppTCOTATB.OTAFwAddr = app_otas_get_storage_address();
    gst_AppTCOTATB.FwInfoNew.size = co_read32p(RcvBuff+2);
    gst_AppTCOTATB.ulAddrOffsetMax = gst_AppTCOTATB.FwInfoNew.size;

    /*ע��FR801x SDK STACK��APP�Ǻ���һ��ģ��������ﲻ������*/
    if(*(RcvBuff+1) == OTA_FILE_FLAG_APP)
    {
        APP_TC_OTA_PRINTF("OTA file type:APP\r\n");
    }
    else if(*(RcvBuff+1) == OTA_FILE_FLAG_STACK)
    {
        APP_TC_OTA_PRINTF("OTA file type:Profile\r\n");
    }
    else
    {
        APP_TC_OTA_PRINTF("OTA file type:Unknow\r\n");
        SendBuff[ucIndex++] = START_OTA_ERR_PARA;
        *pucParaLen = ucIndex - APP_TC_OTA_PROT_RSP_PARA_OFFSET;
        app_set_ota_state(0);

        return;
    }

    APP_TC_OTA_PRINTF("OTAFwAddr��%d\r\n",gst_AppTCOTATB.OTAFwAddr);
    APP_TC_OTA_PRINTF("FwInfoNew.size��%d\r\n",gst_AppTCOTATB.FwInfoNew.size);
    if ( gst_AppTCOTATB.FwInfoNew.size > IMAGE_SIZE)
    {
        SendBuff[ucIndex++] = START_OTA_ERR_IMAGE_SIZE;
        app_set_ota_state(0);

        return;
    }
    else
    {
        SendBuff[ucIndex++] = START_OTA_ERR_NONE;
        app_set_ota_state(1);
    }

    ulLen_4K = ALIGN_4K(gst_AppTCOTATB.FwInfoNew.size);/*���϶��뵽4K*/

    //Flash��������ο�ota.c
    //1.�����256byteΪ��λȥ�������ڶ���OTA�жϵ��豸��ש
    //2.�����4096byteΪ��λȥ�������ڶ���OTA�жϵ��豸��ש
    /*ע��Ϊ�˱�֤�̼�������;���磬�̼���ǰ256�ֽ���Ҫ���д��*/
    for(offset = 256; offset < 4096; offset += 256)
    {
#ifdef FLASH_PROTECT
        flash_protect_disable(0);
#endif
        flash_page_erase(offset + gst_AppTCOTATB.OTAFwAddr);
#ifdef FLASH_PROTECT
        flash_protect_enable(0);
#endif
    }

    for(offset = 4096; offset < ulLen_4K; offset += 4096)
    {
        flash_erase(offset + gst_AppTCOTATB.OTAFwAddr, 0x1000);
    }

    *pucParaLen = ucIndex - APP_TC_OTA_PROT_RSP_PARA_OFFSET;
}

/*******************************************************************
��������: Proc_Confirm
����˵��: �̼�У��
���˵��: RcvBuff->���յ�������buff
        ucRcvLen->���յ������ݳ���
        SendBuff->�����͵����ݻ���
        pucParaLen->��������
����ֵ˵��: void

--------------------------------------------------------------------
���ļ�¼    |     ������     |    ��������    |     ����˵��       |
--------------------------------------------------------------------
 V1.00      |      hlt        |   2021-04-08    |     ��������       |
--------------------------------------------------------------------
*******************************************************************/
static void Proc_Confirm(U8 *RcvBuff, U8 ucRcvLen,  U8 *SendBuff, U8 *pucParaLen)
{
    U8 ucIndex = APP_TC_OTA_PROT_RSP_PARA_OFFSET;

    APP_TC_OTA_PRINTF("Proc_Confirm\r\n");

    gst_AppTCOTATB.FwInfoNew.crc = co_read32p (&RcvBuff[1]);


    WDT_CLEAR();
    if(crc_cal(gst_AppTCOTATB.FwInfoNew.size, gst_AppTCOTATB.OTAFwAddr, gst_AppTCOTATB.FwInfoNew.crc))
    {
        APP_TC_OTA_PRINTF("CRC OK \r\n");
        SendBuff[ucIndex++] = CONFIRM_ERR_NONE;
    }
    else
    {
        APP_TC_OTA_PRINTF("CRC ERR \r\n");
        SendBuff[ucIndex++] = CONFIRM_ERR_CRC;
    }

#ifdef FLASH_PROTECT
    flash_protect_disable(0);
#endif
    flash_page_erase(gst_AppTCOTATB.OTAFwAddr);
#ifdef FLASH_PROTECT
    flash_protect_enable(0);
#endif
    app_otas_save_data(gst_AppTCOTATB.OTAFwAddr, gst_AppTCOTATB.FirstPageData, 256);
    app_set_ota_state(0);
    *pucParaLen = ucIndex - APP_TC_OTA_PROT_RSP_PARA_OFFSET;
}

/*******************************************************************
��������: Proc_Reset
����˵��: ��λ�豸
���˵��: RcvBuff->���յ�������buff
        ucRcvLen->���յ������ݳ���
        SendBuff->�����͵����ݻ���
        pucParaLen->��������
����ֵ˵��: void

--------------------------------------------------------------------
���ļ�¼    |     ������     |    ��������    |     ����˵��       |
--------------------------------------------------------------------
 V1.00      |      hlt        |   2021-04-08    |     ��������       |
--------------------------------------------------------------------
*******************************************************************/
static void Proc_Reset(U8 *RcvBuff, U8 ucRcvLen,  U8 *SendBuff, U8 *pucParaLen)
{
//    U8 ucIndex = APP_TC_OTA_PROT_RSP_PARA_OFFSET;

    APP_TC_OTA_PRINTF("Proc_Reset\r\n");
    *pucParaLen = 0;/*ע�����ﲻ�ظ�����Ȼ��λָ���0x04�͹̼�����Ӧ��ָ���0x04��ͬ��*/
    os_timer_init(&ota_reboot_timer,Reboot_timer_handler, NULL);
    os_timer_start(&ota_reboot_timer,1000, true); //����һ�� 1s �Ķ�ʱ����

}


/*******************************************************************
��������: Proc_GetFwInfo
����˵��: ��ȡ�̼��汾��Ϣ
���˵��: RcvBuff->���յ�������buff
        ucRcvLen->���յ������ݳ���
        SendBuff->�����͵����ݻ���
        pucParaLen->��������
����ֵ˵��: void

--------------------------------------------------------------------
���ļ�¼    |     ������     |    ��������    |     ����˵��       |
--------------------------------------------------------------------
 V1.00      |      hlt        |   2021-04-08    |     ��������       |
--------------------------------------------------------------------
*******************************************************************/
static void Proc_GetFwInfo(U8 *RcvBuff, U8 ucRcvLen,  U8 *SendBuff, U8 *pucParaLen)
{
    U8 Type;
    TC_OTA_FW_INFO FwInfo;
    U8 ucIndex = APP_TC_OTA_PROT_RSP_PARA_OFFSET;

    APP_TC_OTA_PRINTF("Proc_GetFwInfo\r\n");

    memset(&FwInfo, 0, sizeof(TC_OTA_FW_INFO));

    Type = RcvBuff[2];

    if((Type != GET_FW_INFO_TYPE_BLE) && (Type != GET_FW_INFO_TYPE_MESH))
    {
        //�쳣����
    }

    //test only
    FwInfo.version.appliction = __jump_table.firmware_version;
    FwInfo.version.softdevice = 0;
    FwInfo.version.project    = PID_TYPE;

    /*��֡*/
    SendBuff[ucIndex++] = FwInfo.version.appliction;
    SendBuff[ucIndex++] = FwInfo.version.appliction >> 8;
    SendBuff[ucIndex++] = FwInfo.version.softdevice;
    SendBuff[ucIndex++] = FwInfo.version.softdevice >> 8;
    SendBuff[ucIndex++] = FwInfo.version.project;
    SendBuff[ucIndex++] = FwInfo.version.project >> 8;
    SendBuff[ucIndex++] = 0x00;/*Ԥ��λ*/
    SendBuff[ucIndex++] = Type;

    *pucParaLen = ucIndex - APP_TC_OTA_PROT_RSP_PARA_OFFSET;
}

/*******************************************************************
��������: Proc_ImageUpdate
����˵��: ����̼�
���˵��: RcvBuff->���յ�������buff
        ucRcvLen->���յ������ݳ���
        SendBuff->�����͵����ݻ���
        pucParaLen->��������
����ֵ˵��: void

--------------------------------------------------------------------
���ļ�¼    |     ������     |    ��������    |     ����˵��       |
--------------------------------------------------------------------
 V1.00      |      hlt        |   2021-04-08    |     ��������       |
--------------------------------------------------------------------
*******************************************************************/
static void Proc_ImageUpdate(U8 *RcvBuff, U8 ucRcvLen,  U8 *SendBuff, U8 *pucParaLen)
{
    U16 PacketLen = 0;
    APP_TC_OTA_PRINTF("Proc_ImageUpdate...\r\n");

    WDT_CLEAR();
    PacketLen =  ucRcvLen - 2;
    //�ж��Ƿ������ǰ256�ֽڣ�ǰ256�ֽ���Ҫ�ȱ�����������OTA���д��
    if(gst_AppTCOTATB.FirstPageReceiveLen < 256)
    {
        if(gst_AppTCOTATB.FirstPageReceiveLen + PacketLen < 256)
        {
            APP_TC_OTA_PRINTF("gst_AppTCOTATB.FirstPageReceiveLen + PacketLen < 256\r\n");
            memcpy(&gst_AppTCOTATB.FirstPageData[gst_AppTCOTATB.FirstPageReceiveLen], RcvBuff + 2, PacketLen);
            gst_AppTCOTATB.FirstPageReceiveLen += PacketLen;
        }
        else
        {
            APP_TC_OTA_PRINTF("else of gst_AppTCOTATB.FirstPageReceiveLen + PacketLen < 256\r\n");
            memcpy(&gst_AppTCOTATB.FirstPageData[gst_AppTCOTATB.FirstPageReceiveLen],RcvBuff + 2,256-gst_AppTCOTATB.FirstPageReceiveLen);
            gst_AppTCOTATB.ulAddrOffset = 256;
            OTA_Image_Update(RcvBuff + 2 + (256-gst_AppTCOTATB.FirstPageReceiveLen), PacketLen-(256-gst_AppTCOTATB.FirstPageReceiveLen));
            gst_AppTCOTATB.FirstPageReceiveLen = 256;
        }
    }
    else
    {
        OTA_Image_Update(RcvBuff + 2, PacketLen);
    }
    *pucParaLen = 0;
}

/*******************************************************************
��������: ProtocolProcess
����˵��: TC OTA Э�鴦����
���˵��: RcvBuff->���յ�������buff
        ucRcvLen->���յ������ݳ���
        SendBuff->�����͵����ݻ���
        pucSendLen->�����͵����ݳ���
����ֵ˵��: void
--------------------------------------------------------------------
���ļ�¼    |     ������     |    ��������    |     ����˵��       |
--------------------------------------------------------------------
 V1.00      |      hlt        |   2021-04-08    |     ��������       |
--------------------------------------------------------------------
*******************************************************************/
static void ProtocolProcess(U8 *RcvBuff, U8 ucRcvLen,  U8 *SendBuff, U8 *pucSendLen)
{
    U8 Cmd;
    U8 ucSendLen = 0;
    U8 ucParaLen = 0;//��������


//  APP_TC_OTA_PRINTF("ProtocolProcess\r\n");

//	APP_TC_OTA_PRINTF("\r\nRxLen = %d,RxData = ", ucRcvLen);
//	for(ucPt = 0; ucPt < ucRcvLen; ucPt++)
//	{
//	    APP_TC_OTA_PRINTF("%2X ", RcvBuff[ucPt]);
//	}
//	APP_TC_OTA_PRINTF("\r\n");

    if(TRUE == gst_AppTCOTATB.ImageUpdate)
    {
        Cmd = TC_OTA_PROT_CMD_IMAGE_UPDATE;/*����һ�����������*/
    }
    else
    {
        Cmd = RcvBuff[APP_TC_OTA_CMD_OFFSET];
    }
    APP_TC_OTA_PRINTF("ProtocolProcess Cmd:%d\r\n",Cmd);
    if((Cmd > TC_OTA_PROT_CMD_MAX) || (ucRcvLen < APP_TC_OTA_PROT_LEN_MIN))
    {
        return ;
    }
    /* �������������ö�Ӧ���������*/
    if(PNULL != s_AppTCOTAProcTbl[Cmd])
    {
        s_AppTCOTAProcTbl[Cmd](RcvBuff, ucRcvLen, SendBuff, &ucParaLen);

        /*�̼���������Ƚ�����*/
        if(TRUE == gst_AppTCOTATB.ImageUpdate)
        {
            ucSendLen = 2;
            SendBuff[0] = TC_OTA_PROT_CMD_RSP;
            SendBuff[1] = 0x04;
        }
        else
        {
            if(ucParaLen != 0)
            {
                ucSendLen = 2 + ucParaLen;//Response Code+OP Code+����
                SendBuff[0] = TC_OTA_PROT_CMD_RSP;
                SendBuff[1] = Cmd;
            }
        }
    }
    *pucSendLen = ucSendLen;

//	APP_TC_OTA_PRINTF("\r\nTxLen = %d,TxData = ", ucSendLen);
//	for(ucPt = 0; ucPt < ucSendLen; ucPt++)
//	{
//	    APP_TC_OTA_PRINTF("%2X ", SendBuff[ucPt]);
//	}
//	APP_TC_OTA_PRINTF("\r\n");
}

/*****************************************************************************
Function name :  APP_TC_OTA_Init
Description:	 top chip ota���� ��ʼ��
Input:	         void
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void APP_TC_OTA_Init(void)
{
    memset(&gst_AppTCOTATB, 0x00, sizeof(APP_TC_OTA_TB));
    gst_AppTCOTATB.FwType = FW_TYPE_MAX;
    gst_AppTCOTATB.ImageUpdate = FALSE;
    gst_AppTCOTATB.FirstPageReceiveLen = 0;
}
/*****************************************************************************
Function name :  APP_TC_OTA_Task
Description:	 top chip ota����
Input:	         void
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
int APP_TC_OTA_Task(os_event_t *param)
{
    U8 SendBuff[APP_TC_OTA_TX_BUFF_LEN];
    U8 ucSendLen = 0;

    /**************��������******************/
    switch(param->event_id)
    {
        case EVT_TC_OTA_RECV_FRAME:
        {
            /**************OTA Э�鴦��******************/
            APP_TC_OTA_PRINTF("EVT_TC_OTA_RECV_FRAME\r\n");
            ProtocolProcess(gst_AppTCOTATB.Buff, gst_AppTCOTATB.ucLen, SendBuff, &ucSendLen);

            if(ucSendLen > 0)
            {
                GATT_SendData(SendBuff, ucSendLen);
            }

        }
        break;

        default:
            break;
    }

    return EVT_CONSUMED;/*�ͷ���Ϣ*/
}
