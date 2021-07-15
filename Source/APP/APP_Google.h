#ifndef __APP_Google_Protocol_H
#define __APP_Google_Protocol_H

#ifdef __cplusplus
 extern "C" {
#endif
	
#define APP_GOOGLE_PROT_LEN_MIN  (2U)

#define APP_GOOGLE_BUFF_LEN      (50U)


#define HW 3
#define SW 1

/*�ȸ�������Ƶ�������*/
typedef enum
{
    GOOGLE_PROT_CMD_ONOFF = 0x00,  /*���ص�*/   
    GOOGLE_PROT_CMD_ADJ_BRI,/*��������*/
    GOOGLE_PROT_CMD_ADJ_TMP,/*����ɫ��*/
    GOOGLE_PROT_CMD_BLINK,/*��˸��*/
    GOOGLE_PROT_CMD_PROVISION, /*�󶨵�*/   
    GOOGLE_PROT_CMD_UNPROVISION,/*����*/
    GOOGLE_PROT_CMD_ADJ_RGB,/*����RGBֵ*/

    GOOGLE_PROT_CMD_MAX,

}GOOGLE_PROT_CMD;


/*�ϱ��� Google������豸���� ����*/    
typedef struct
{
    U8   OnOff;          /*�Ƿ�֧�ֿ��ص�*/
	U8   Brightness;     /*�Ƿ�֧�����ȵ���*/
	U8   Temperature;    /*�Ƿ�֧��ɫ�µ���*/
	U8   RGB;            /*�Ƿ�֧��RGB����*/
    U16  TemperatureMin;/*ɫ����Сֵ ���*/
    U16  TemperatureMax;/*ɫ�����ֵ ���*/

}DEVICE_TRAITS;


/*�ϱ��� Google������豸״̬*/    
__packed typedef struct
{
    U8 OnOff;/*����״̬*/
	U8 Lightness; /*����״̬*/
	U8 Temperature_H;  /*ɫ�¸��ֽ�*/
	U8 Temperature_L;  /*ɫ�µ��ֽ�*/
    U8 R;/*R Value*/
    U8 G;/*G Value*/
    U8 B;/*B Value*/

}DEVICE_STATUS;



/*APP Google ������ƿ�*/    
typedef struct
{
    U8   TxBuff[APP_GOOGLE_BUFF_LEN];
	U8   ucTxLen;
	U8   RxBuff[APP_GOOGLE_BUFF_LEN];
	U8   ucRxLen;
    DEVICE_TRAITS *pDeviceTraits;/*�豸����*/
    DEVICE_STATUS DeviceStatus;/*�豸״̬*/
    U8 *pDeviceName;/*�豸����*/
    U8 DeviceNameLen;
    U8 DeviceVersion[2];/*�豸�汾*/

    U8 ConIdx;
    U8 AttIdx;

    U8 StartBlink;/*��˸��־*/
    U8 BlinkoOnOffFlag;/*��˸ʱ�ƵĿ���״̬*/
    U16 usBlinkTimeCnt;
    
    
}APP_Google_TB;

extern APP_Google_TB gst_AppGoogleTB;

void APP_Google_TimeProc(U32 ulPeriod);
void APP_Google_Init(void);
int APP_Google_Task(os_event_t *param);
void APP_Google_Start_Advertising(void);

#ifdef __cplusplus
}
#endif
  
   
#endif

