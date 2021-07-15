#ifndef __APP_MEM_H
#define __APP_MEM_H

#ifdef __cplusplus
 extern "C" {
#endif


#define NVM_UESR_DATA_ADDR       (0x65000UL)/*�û��洢���ݵ���ʼ��ַ*/
#define NVM_UESR_DATA_SIZE       (0x1000UL)/*�û��洢���ݵĴ�С*/
#define RECORD_DATA_LEN          (sizeof(APP_MEM_DB))/*һ����¼���ݵĵĴ�С*/
#define RECORD_DATA_NUM          ((NVM_UESR_DATA_SIZE) / sizeof(APP_MEM_DB))/*�ܹ����Դ洢������¼����*/

#define NVM_UESR_DATA_ADDR_BK    (NVM_UESR_DATA_ADDR + NVM_UESR_DATA_SIZE)/*ע�����������Ŀռ����ڱ�֤�����ھ��Ƶ�ʱ�򲻶�ʧ*/

#define APP_MEM_WRITE_DELAY      (TmrSeconds(3))/*��ʱ3Sд������*/

#define OTA_WITH_OFF_FLAG        (0xAA)


#define MEM_DB_BLE    (gst_AppMemDB.Light.ble)
#define MEM_DB_MESH   (gst_AppMemDB.Light.mesh)

/*�豸��������*/
typedef enum
{
    PROV_TYPE_NONE = 0x00,
    PROV_TYPE_WITH_GOOGLE,
    PROV_TYPE_WITH_AMANZON,

}PROV_TYPE;


/*�豸���е�ģʽ RGB or CW*/
typedef enum
{
    LIGHT_MODE_RGB = 0x00,
    LIGHT_MODE_CW,
    LIGHT_MODE_MAX,

}LIGHT_MODE;

/*MESH MEM ���ݿ� 36Byte*/     
__packed typedef struct
{
	U8 dtt;       /*Generic Default Transition Time*/
	U8 onpowerup; /*Generic OnPowerUp*/
	
	U16 lightness_default;   
	U16 lightness_target;  
	U16 lightness_last; 
	U16 lightness_range_min;
	U16 lightness_range_max;
  	S16 lightness_delta;
 
	
	U16 temp_default;  
	U16 temp_target; 
	U16 temp_range_min;
	U16 temp_range_max;	
	
    U16 duv_default;   
	U16 duv_target;  
	
	U16 hue_target;
	U16 hue_range_min;
	U16 hue_range_max;

    
	U16 saturation_target;
	U16 saturation_range_min;
	U16 saturation_range_max;

	
}MESH_MEM_DB;

/*BLE MEM ���ݿ� 7Byte*/     
__packed typedef struct
{
    U8  Level;       /*���ȵȼ�*/
	U16 Temperature; /*ɫ��*/
	U8  R;           /*RGB ��ֵ*/
	U8  G;
	U8  B;
	U8  LightMode;   /*������ģʽ RGB or CW*/

}BLE_MEM_DB;

/*Light ���ݿ�*/ 
__packed typedef union
{
    MESH_MEM_DB mesh;
    BLE_MEM_DB  ble;

}LIGHT_DB;


/*MEM ���ݿ�*/     
__packed typedef struct
{
	PROV_TYPE Provision;    /*������־*/ 
	U8        ResetCnt;     /*��λ����*/
	U8        OTAWithOff;   /*��¼�Ƿ��ڹص��µ�OTA*/
 	LIGHT_DB  Light;        /*Light ���ݿ�*/ 
    
    U8        ucCheckSum;   /*У���*/
   
}APP_MEM_DB;


/*�洢ģ�� ������ƿ�*/
typedef struct
{
    U32 ulWriteDelayCnt;/*�洢���ݵ��ӳ�*/
    U16 usCurIndex;/*��ǰ�洢λ�õ�����*/
 
}APP_MEM_TB;


extern APP_MEM_DB gst_AppMemDB;
extern APP_MEM_TB gst_AppMemTB;

void APP_MEM_TimeProc(U32 ulPeriod);
void APP_MEM_Init(void); 
int APP_MEM_Task(os_event_t *param);
void APP_MEM_Test(void);
void APP_MEM_Factory_Reset(void);
void APP_MEM_Slow_Save(void);
void APP_MEM_Fast_Save(void);  

#ifdef __cplusplus
}
#endif
  
   
#endif

