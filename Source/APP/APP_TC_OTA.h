#ifndef __APP_TC_OTA_H
#define __APP_TC_OTA_H

#ifdef __cplusplus
 extern "C" {
#endif


/*固件切换功能*/
#define APP_TC_OTA_FW_SWITCH_EN              (0)
#define APP_TC_OTA_CMD_OFFSET                (0)
#define APP_TC_OTA_PROT_LEN_MIN              (1U)
#define APP_TC_OTA_PROT_RSP_PARA_OFFSET      (2U)
#define APP_TC_OTA_RX_BUFF_LEN               (255U)
#define APP_TC_OTA_TX_BUFF_LEN               (60U)/*定义发送buff的长度*/
/*TC OTA 命令码*/
typedef enum
{
    TC_OTA_PROT_CMD_SYNC_FW_VERSION = 0x01,   /*同步固件版本*/   
    TC_OTA_PROT_CMD_START,                    /*启动OTA*/
    TC_OTA_PROT_CMD_CONFIRM,                  /*固件校验*/
    TC_OTA_PROT_CMD_RESET,                    /*复位设备*/
    TC_OTA_PROT_CMD_FW_SWITCH,                /*切换固件*/
    TC_OTA_PROT_CMD_GET_FW_INFO,              /*获取固件版本信息*/
    TC_OTA_PROT_CMD_IMAGE_UPDATE,             /*传输固件 注！！：实际协议中没有改命令码，这里是为了写代码方便*/
    TC_OTA_PROT_CMD_MAX,
    
    TC_OTA_PROT_CMD_RSP = 0x80,/*应答*/

}TC_OTA_PROT_CMD;


typedef enum
{
    APP_TC_OTA_STATE_SYNC_FW_VERSION = 0,
    APP_TC_OTA_STATE_START,
    APP_TC_OTA_STATE_RESET,
    APP_TC_OTA_STATE_CONFIRM,
    APP_TC_OTA_STATE_IMAGE_UPDATE,
#if (APP_TC_OTA_FW_SWITCH_EN)    
    APP_TC_OTA_STATE_FW_SWITCH,
#endif
    APP_TC_OTA_STATE_IDLE,
    APP_TC_OTA_STATE_MAX,
    
} APP_TC_OTA_STATE;

/*切换的固件类型*/
typedef enum
{
	FW_TYPE_BLE_STACK = 0x00,
	FW_TYPE_BLE_APP,
	FW_TYPE_MESH_STACK,
	FW_TYPE_MESH_APP,
	
	FW_TYPE_MAX,
	
}FW_TYPE;

/*0->当前运行的是BLE程序; 1->当前运行的是MESH程序*/
typedef enum
{
    CUR_FW_TYPE_BLE    = 0,
    CUR_FW_TYPE_MESH   = 1,
    
}CUR_FW_TYPE_ENUM;
    
/*OTA 下发的固件类型 MESH or BLE*/
typedef enum
{
    OTA_FILE_TYPE_BLE    = 0,
    OTA_FILE_TYPE_MESH = 1,
    
} OTA_FILE_TYPE;

/*OTA 下发的固件FLAG APP or STACK*/
typedef enum 
{
    OTA_FILE_FLAG_APP     = 1,
    OTA_FILE_FLAG_STACK   = 2,
    
} OTA_FILE_FLAG;

/*启动固件传输命令的错误码*/
typedef enum
{
    START_OTA_ERR_NONE          = 0x00,
    START_OTA_ERR_IMAGE_SIZE    = 0x01,
    START_OTA_ERR_FW_VER        = 0x02,
    START_OTA_ERR_PARA          = 0x03,
    
}START_OTA_ERR;
    
/*固件校验的错误码*/
typedef enum
{
    CONFIRM_ERR_NONE   = 0x00,
    CONFIRM_ERR_CRC    = 0x01,
    
}CONFIRM_ERR;

/*获取版本信息时，指定的固件类型*/
typedef enum
{
    GET_FW_INFO_TYPE_BLE    = 0,
    GET_FW_INFO_TYPE_MESH = 1,
    
}GET_FW_INFO_TYPE;
    
/*固件版本信息*/
typedef struct
{
    U16 softdevice;/*协议栈版本*/
    U16 appliction;/*应用程序版本*/
	U16	project;    /*SDK版本*/
    
}FW_VERSION;

typedef struct
{
    U32    size;
    U32    crc;
    FW_VERSION   version;
    U8    flag;/*标记是APP 还是STACK*/
    U8    type;/*标记是BLE or MESH*/
	
} TC_OTA_FW_INFO;



/*APP TC OTA 任务控制块*/    
typedef struct
{
    FW_TYPE        FwType;
    U32            OTAFwAddr;
    TC_OTA_FW_INFO FwInfoCur;
    TC_OTA_FW_INFO FwInfoNew;
    U32            ulAddrOffset;
    U32            ulAddrOffsetMax;
    BOOL           ImageUpdate;
    
	U8 Buff[APP_TC_OTA_RX_BUFF_LEN];
    U8 ucLen;
    U8 ConIdx;
    U8 AttIdx;
	U8 FirstPageData[256];//存储BIN文件前256字节数据
	U16 FirstPageReceiveLen;
}APP_TC_OTA_TB;


void APP_TC_OTA_Init(void);
int APP_TC_OTA_Task(os_event_t *param);

#if (CUR_PROJECT_TYPE == PROJECT_TYPE_BLE)/*BLE*/
    void APP_TC_OTA_TimeProc(U32 ulPeriod);
#endif    


extern APP_TC_OTA_TB gst_AppTCOTATB;

#ifdef __cplusplus
}
#endif
  
#endif

