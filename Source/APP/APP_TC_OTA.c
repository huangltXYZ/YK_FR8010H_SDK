#include "YK_APP.h"

/*抽象 喂狗 函数*/
#if (BSP_WDT_USE > 0u)
#define WDT_CLEAR()     wdt_feed()
#else
#define WDT_CLEAR()
#endif


#define APP_TC_OTA_PRINTF(...)    printf(__VA_ARGS__)

/*外部函数声明*/
void enable_cache(uint8_t invalid_ram);
void disable_cache(void);


/*APP TC OTA 任务控制块*/
APP_TC_OTA_TB gst_AppTCOTATB;

os_timer_t ota_reboot_timer;

/************** TC OTA 协议处理函数向量表 *******************************/
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
    Proc_SyncFwVersion,     /*同步固件版本*/
    Proc_StartOTA,          /*启动OTA*/
    Proc_Confirm,           /*固件校验*/
    Proc_Reset,             /*复位设备*/

#if (APP_TC_OTA_FW_SWITCH_EN)
    Proc_FwSwitch,          /*切换固件*/
#else
    PNULL,
#endif
    Proc_GetFwInfo,         /*获取固件版本信息*/
    Proc_ImageUpdate,       /*传输固件 注！！：实际协议中没有改命令码，这里是为了写代码方便*/
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

    /*先计算存在RAM中的前256字节CRC*/
    crc_data =  Crc32CalByte(crc_data, gst_AppTCOTATB.FirstPageData, 256);

    /*再计算存在flash中的剩余文件的CRC*/
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
函数名称: Reboot_timer_handler
功能说明: Reboot定时器的回调函数
入参说明: RcvBuff->接收到的数据buff
        ucRcvLen->接收到的数据长度
        SendBuff->待发送的数据缓存
        pucParaLen->参数长度
返回值说明: void

--------------------------------------------------------------------
更改记录    |     更改者     |    更改日期    |     更改说明       |
--------------------------------------------------------------------
 V1.00      |      hlt        |   2021-04-08    |     创建函数       |
--------------------------------------------------------------------
*******************************************************************/
static void Reboot_timer_handler(void * p_context)
{
    SYSTEM_RESET();
}

/*****************************************************************************
Function name :  OTA_Param_Init
Description:	 OTA 参数初始化
Input:	         OTAType->准备下发的OTA固件类型
Return:          void
Notes: 片外FLASH的擦除在START OTA步骤中已经执行
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
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
Description:	 更新固件内容
Input:	         pData->固件内容
Input:	         usLen->对应包的固件内容长度
Return:          TRUE->一包数据更新成功；FALSE->更新失败
Notes: 片外FLASH的擦除在START OTA步骤中已经执行
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
static BOOL OTA_Image_Update(U8 *pData, U16 usLen)
{
//	U8 rd_buf[16],i;

    /*固件长度判断*/
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
Description:	 通过GATT发送数据
Input:	         SendBuff->待发送数据的buff
Input:	         ucSendLen->待发送数据的长度
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
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
函数名称: Proc_SyncFwVersion
功能说明: 同步固件版本
入参说明: RcvBuff->接收到的数据buff
        ucRcvLen->接收到的数据长度
        SendBuff->待发送的数据缓存
        pucParaLen->参数长度
返回值说明: void

--------------------------------------------------------------------
更改记录    |     更改者     |    更改日期    |     更改说明       |
--------------------------------------------------------------------
 V1.00      |      hlt        |   2021-04-08    |     创建函数       |
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
函数名称: Proc_StartOTA
功能说明: 启动OTA
入参说明: RcvBuff->接收到的数据buff
        ucRcvLen->接收到的数据长度
        SendBuff->待发送的数据缓存
        pucParaLen->参数长度
返回值说明: void

--------------------------------------------------------------------
更改记录    |     更改者     |    更改日期    |     更改说明       |
--------------------------------------------------------------------
 V1.00      |      hlt        |   2021-04-08    |     创建函数       |
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

    /*注：FR801x SDK STACK和APP是合在一起的，所以这里不做区分*/
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

    APP_TC_OTA_PRINTF("OTAFwAddr：%d\r\n",gst_AppTCOTATB.OTAFwAddr);
    APP_TC_OTA_PRINTF("FwInfoNew.size：%d\r\n",gst_AppTCOTATB.FwInfoNew.size);
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

    ulLen_4K = ALIGN_4K(gst_AppTCOTATB.FwInfoNew.size);/*向上对齐到4K*/

    //Flash擦除代码参考ota.c
    //1.如果以256byte为单位去擦除，第二次OTA中断电设备变砖
    //2.如果以4096byte为单位去擦除，第二次OTA中断电设备变砖
    /*注：为了保证固件升级中途掉电，固件的前256字节需要最后写入*/
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
函数名称: Proc_Confirm
功能说明: 固件校验
入参说明: RcvBuff->接收到的数据buff
        ucRcvLen->接收到的数据长度
        SendBuff->待发送的数据缓存
        pucParaLen->参数长度
返回值说明: void

--------------------------------------------------------------------
更改记录    |     更改者     |    更改日期    |     更改说明       |
--------------------------------------------------------------------
 V1.00      |      hlt        |   2021-04-08    |     创建函数       |
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
函数名称: Proc_Reset
功能说明: 复位设备
入参说明: RcvBuff->接收到的数据buff
        ucRcvLen->接收到的数据长度
        SendBuff->待发送的数据缓存
        pucParaLen->参数长度
返回值说明: void

--------------------------------------------------------------------
更改记录    |     更改者     |    更改日期    |     更改说明       |
--------------------------------------------------------------------
 V1.00      |      hlt        |   2021-04-08    |     创建函数       |
--------------------------------------------------------------------
*******************************************************************/
static void Proc_Reset(U8 *RcvBuff, U8 ucRcvLen,  U8 *SendBuff, U8 *pucParaLen)
{
//    U8 ucIndex = APP_TC_OTA_PROT_RSP_PARA_OFFSET;

    APP_TC_OTA_PRINTF("Proc_Reset\r\n");
    *pucParaLen = 0;/*注：这里不回复，不然复位指令的0x04和固件更新应答指令的0x04相同了*/
    os_timer_init(&ota_reboot_timer,Reboot_timer_handler, NULL);
    os_timer_start(&ota_reboot_timer,1000, true); //启动一个 1s 的定时器。

}


/*******************************************************************
函数名称: Proc_GetFwInfo
功能说明: 获取固件版本信息
入参说明: RcvBuff->接收到的数据buff
        ucRcvLen->接收到的数据长度
        SendBuff->待发送的数据缓存
        pucParaLen->参数长度
返回值说明: void

--------------------------------------------------------------------
更改记录    |     更改者     |    更改日期    |     更改说明       |
--------------------------------------------------------------------
 V1.00      |      hlt        |   2021-04-08    |     创建函数       |
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
        //异常处理
    }

    //test only
    FwInfo.version.appliction = __jump_table.firmware_version;
    FwInfo.version.softdevice = 0;
    FwInfo.version.project    = PID_TYPE;

    /*组帧*/
    SendBuff[ucIndex++] = FwInfo.version.appliction;
    SendBuff[ucIndex++] = FwInfo.version.appliction >> 8;
    SendBuff[ucIndex++] = FwInfo.version.softdevice;
    SendBuff[ucIndex++] = FwInfo.version.softdevice >> 8;
    SendBuff[ucIndex++] = FwInfo.version.project;
    SendBuff[ucIndex++] = FwInfo.version.project >> 8;
    SendBuff[ucIndex++] = 0x00;/*预留位*/
    SendBuff[ucIndex++] = Type;

    *pucParaLen = ucIndex - APP_TC_OTA_PROT_RSP_PARA_OFFSET;
}

/*******************************************************************
函数名称: Proc_ImageUpdate
功能说明: 传输固件
入参说明: RcvBuff->接收到的数据buff
        ucRcvLen->接收到的数据长度
        SendBuff->待发送的数据缓存
        pucParaLen->参数长度
返回值说明: void

--------------------------------------------------------------------
更改记录    |     更改者     |    更改日期    |     更改说明       |
--------------------------------------------------------------------
 V1.00      |      hlt        |   2021-04-08    |     创建函数       |
--------------------------------------------------------------------
*******************************************************************/
static void Proc_ImageUpdate(U8 *RcvBuff, U8 ucRcvLen,  U8 *SendBuff, U8 *pucParaLen)
{
    U16 PacketLen = 0;
    APP_TC_OTA_PRINTF("Proc_ImageUpdate...\r\n");

    WDT_CLEAR();
    PacketLen =  ucRcvLen - 2;
    //判断是否接受完前256字节，前256字节需要先保存下来，在OTA最后写入
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
函数名称: ProtocolProcess
功能说明: TC OTA 协议处理函数
入参说明: RcvBuff->接收到的数据buff
        ucRcvLen->接收到的数据长度
        SendBuff->待发送的数据缓存
        pucSendLen->待发送的数据长度
返回值说明: void
--------------------------------------------------------------------
更改记录    |     更改者     |    更改日期    |     更改说明       |
--------------------------------------------------------------------
 V1.00      |      hlt        |   2021-04-08    |     创建函数       |
--------------------------------------------------------------------
*******************************************************************/
static void ProtocolProcess(U8 *RcvBuff, U8 ucRcvLen,  U8 *SendBuff, U8 *pucSendLen)
{
    U8 Cmd;
    U8 ucSendLen = 0;
    U8 ucParaLen = 0;//参数长度


//  APP_TC_OTA_PRINTF("ProtocolProcess\r\n");

//	APP_TC_OTA_PRINTF("\r\nRxLen = %d,RxData = ", ucRcvLen);
//	for(ucPt = 0; ucPt < ucRcvLen; ucPt++)
//	{
//	    APP_TC_OTA_PRINTF("%2X ", RcvBuff[ucPt]);
//	}
//	APP_TC_OTA_PRINTF("\r\n");

    if(TRUE == gst_AppTCOTATB.ImageUpdate)
    {
        Cmd = TC_OTA_PROT_CMD_IMAGE_UPDATE;/*虚拟一个命令码出来*/
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
    /* 根据命令码查表获得对应处理函数入口*/
    if(PNULL != s_AppTCOTAProcTbl[Cmd])
    {
        s_AppTCOTAProcTbl[Cmd](RcvBuff, ucRcvLen, SendBuff, &ucParaLen);

        /*固件更新命令比较特殊*/
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
                ucSendLen = 2 + ucParaLen;//Response Code+OP Code+长度
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
Description:	 top chip ota任务 初始化
Input:	         void
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
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
Description:	 top chip ota任务
Input:	         void
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
int APP_TC_OTA_Task(os_event_t *param)
{
    U8 SendBuff[APP_TC_OTA_TX_BUFF_LEN];
    U8 ucSendLen = 0;

    /**************重启处理******************/
    switch(param->event_id)
    {
        case EVT_TC_OTA_RECV_FRAME:
        {
            /**************OTA 协议处理******************/
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

    return EVT_CONSUMED;/*释放消息*/
}
