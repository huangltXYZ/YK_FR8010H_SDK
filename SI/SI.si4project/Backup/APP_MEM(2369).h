#ifndef __APP_MEM_H
#define __APP_MEM_H

#ifdef __cplusplus
 extern "C" {
#endif


#define NVM_UESR_DATA_ADDR       (0x65000UL)/*用户存储数据的起始地址*/
#define NVM_UESR_DATA_SIZE       (0x1000UL)/*用户存储数据的大小*/
#define RECORD_DATA_LEN          (sizeof(APP_MEM_DB))/*一条记录数据的的大小*/
#define RECORD_DATA_NUM          ((NVM_UESR_DATA_SIZE) / sizeof(APP_MEM_DB))/*总共可以存储几条记录数据*/

#define NVM_UESR_DATA_ADDR_BK    (NVM_UESR_DATA_ADDR + NVM_UESR_DATA_SIZE)/*注：备份扇区的空间用于保证数据在卷绕的时候不丢失*/

#define APP_MEM_WRITE_DELAY      (TmrSeconds(3))/*延时3S写入数据*/

#define OTA_WITH_OFF_FLAG        (0xAA)


#define MEM_DB_BLE    (gst_AppMemDB.Light.ble)
#define MEM_DB_MESH   (gst_AppMemDB.Light.mesh)

/*设备配网类型*/
typedef enum
{
    PROV_TYPE_NONE = 0x00,
    PROV_TYPE_WITH_GOOGLE,
    PROV_TYPE_WITH_AMANZON,

}PROV_TYPE;


/*设备运行的模式 RGB or CW*/
typedef enum
{
    LIGHT_MODE_RGB = 0x00,
    LIGHT_MODE_CW,
    LIGHT_MODE_MAX,

}LIGHT_MODE;

/*MESH MEM 数据块 36Byte*/     
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

/*BLE MEM 数据块 7Byte*/     
__packed typedef struct
{
    U8  Level;       /*亮度等级*/
	U16 Temperature; /*色温*/
	U8  R;           /*RGB 数值*/
	U8  G;
	U8  B;
	U8  LightMode;   /*灯运行模式 RGB or CW*/

}BLE_MEM_DB;

/*Light 数据块*/ 
__packed typedef union
{
    MESH_MEM_DB mesh;
    BLE_MEM_DB  ble;

}LIGHT_DB;


/*MEM 数据块*/     
__packed typedef struct
{
	PROV_TYPE Provision;    /*配网标志*/ 
	U8        ResetCnt;     /*复位次数*/
	U8        OTAWithOff;   /*记录是否处于关灯下的OTA*/
 	LIGHT_DB  Light;        /*Light 数据块*/ 
    
    U8        ucCheckSum;   /*校验和*/
   
}APP_MEM_DB;


/*存储模块 任务控制块*/
typedef struct
{
    U32 ulWriteDelayCnt;/*存储数据的延迟*/
    U16 usCurIndex;/*当前存储位置的索引*/
 
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

