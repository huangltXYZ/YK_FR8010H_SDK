#ifndef __APP_TC_OTA_H
#define __APP_TC_OTA_H

#ifdef __cplusplus
 extern "C" {
#endif


/*�̼��л�����*/
#define APP_TC_OTA_FW_SWITCH_EN              (0)
#define APP_TC_OTA_CMD_OFFSET                (0)
#define APP_TC_OTA_PROT_LEN_MIN              (1U)
#define APP_TC_OTA_PROT_RSP_PARA_OFFSET      (2U)
#define APP_TC_OTA_RX_BUFF_LEN               (255U)
#define APP_TC_OTA_TX_BUFF_LEN               (60U)/*���巢��buff�ĳ���*/
/*TC OTA ������*/
typedef enum
{
    TC_OTA_PROT_CMD_SYNC_FW_VERSION = 0x01,   /*ͬ���̼��汾*/   
    TC_OTA_PROT_CMD_START,                    /*����OTA*/
    TC_OTA_PROT_CMD_CONFIRM,                  /*�̼�У��*/
    TC_OTA_PROT_CMD_RESET,                    /*��λ�豸*/
    TC_OTA_PROT_CMD_FW_SWITCH,                /*�л��̼�*/
    TC_OTA_PROT_CMD_GET_FW_INFO,              /*��ȡ�̼��汾��Ϣ*/
    TC_OTA_PROT_CMD_IMAGE_UPDATE,             /*����̼� ע������ʵ��Э����û�и������룬������Ϊ��д���뷽��*/
    TC_OTA_PROT_CMD_MAX,
    
    TC_OTA_PROT_CMD_RSP = 0x80,/*Ӧ��*/

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

/*�л��Ĺ̼�����*/
typedef enum
{
	FW_TYPE_BLE_STACK = 0x00,
	FW_TYPE_BLE_APP,
	FW_TYPE_MESH_STACK,
	FW_TYPE_MESH_APP,
	
	FW_TYPE_MAX,
	
}FW_TYPE;

/*0->��ǰ���е���BLE����; 1->��ǰ���е���MESH����*/
typedef enum
{
    CUR_FW_TYPE_BLE    = 0,
    CUR_FW_TYPE_MESH   = 1,
    
}CUR_FW_TYPE_ENUM;
    
/*OTA �·��Ĺ̼����� MESH or BLE*/
typedef enum
{
    OTA_FILE_TYPE_BLE    = 0,
    OTA_FILE_TYPE_MESH = 1,
    
} OTA_FILE_TYPE;

/*OTA �·��Ĺ̼�FLAG APP or STACK*/
typedef enum 
{
    OTA_FILE_FLAG_APP     = 1,
    OTA_FILE_FLAG_STACK   = 2,
    
} OTA_FILE_FLAG;

/*�����̼���������Ĵ�����*/
typedef enum
{
    START_OTA_ERR_NONE          = 0x00,
    START_OTA_ERR_IMAGE_SIZE    = 0x01,
    START_OTA_ERR_FW_VER        = 0x02,
    START_OTA_ERR_PARA          = 0x03,
    
}START_OTA_ERR;
    
/*�̼�У��Ĵ�����*/
typedef enum
{
    CONFIRM_ERR_NONE   = 0x00,
    CONFIRM_ERR_CRC    = 0x01,
    
}CONFIRM_ERR;

/*��ȡ�汾��Ϣʱ��ָ���Ĺ̼�����*/
typedef enum
{
    GET_FW_INFO_TYPE_BLE    = 0,
    GET_FW_INFO_TYPE_MESH = 1,
    
}GET_FW_INFO_TYPE;
    
/*�̼��汾��Ϣ*/
typedef struct
{
    U16 softdevice;/*Э��ջ�汾*/
    U16 appliction;/*Ӧ�ó���汾*/
	U16	project;    /*SDK�汾*/
    
}FW_VERSION;

typedef struct
{
    U32    size;
    U32    crc;
    FW_VERSION   version;
    U8    flag;/*�����APP ����STACK*/
    U8    type;/*�����BLE or MESH*/
	
} TC_OTA_FW_INFO;



/*APP TC OTA ������ƿ�*/    
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
	U8 FirstPageData[256];//�洢BIN�ļ�ǰ256�ֽ�����
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

