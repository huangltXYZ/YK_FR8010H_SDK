#ifndef YK_Light_Config_H
#define YK_Light_Config_H

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

//<o> ��Ʒ�ͺ� 
//<1=> PID_TYPE_G_RGB_A19_8W  ��оƬ-�ȸ�-RGB-A19-8W 
//<2=> PID_TYPE_A_RGB_A19_8W  ��оƬ-����ѷ-RGB-A19-8W 
//<3=> PID_TYPE_G_CW_A19_8W   ��оƬ-�ȸ�-CW-A19-8W  
//<4=> PID_TYPE_A_CW_A19_8W   ��оƬ-����ѷ-CW-A19-8W 
//<5=> PID_TYPE_GA_RGB_A19_8W ˫оƬ-�ȸ�&����ѷ-RGB-A19-8W 
//<6=> PID_TYPE_GA_CW_A19_8W  ˫оƬ-�ȸ�&����ѷ-CW-A19-8W

#define PID_TYPE 5

//<o> ֧�ֵĹ̼����� 
//<0=> TC_OTA_SUPPORT_FW_TYPE_G   �ȸ�
//<1=> TC_OTA_SUPPORT_FW_TYPE_A   ����ѷ
//<2=> TC_OTA_SUPPORT_FW_TYPE_AG  ˫ƽ̨
#define SUPPORT_FW_TYPE 2

//<o> ��ǰ���еĹ̼����� 
//<0=> TC_OTA_RUN_FW_TYPE_G �ȸ�
//<1=> TC_OTA_RUN_FW_TYPE_A ����ѷ
#define RUN_FW_TYPE 0

/***************���ⷽʽѡ�� PWM ****************/
#define DIMMER_WAY_PWM                (0x01)/*ͨ��PWM��ʽ����*/
#define DIMMER_WAY_IIC_WITH_SM2135EJ  (0x02)/*ͨ��IIC��ʽ���� SM2135EJ*/
#define DIMMER_WAY_IIC_WITH_BP1658CJ  (0x03)/*ͨ��IIC��ʽ���� BP1658CJ*/
//<o> ���ⷽʽѡ�� 
//<1=> PWM���ⷽʽ
//<2=> IIC���ⷽʽ ͨ��SM2135EJ
//<3=> IIC���ⷽʽ ͨ��BP1658CJ
#define DIMMER 1

//<e> �Ƿ�֧��RGB���ⷽʽ 
#define HW_RGB_ENABLE 1
//</e> 


/*ɫ�·�Χ,���ݵ��鶨��*/
// <o> ɫ�·�Χ��Сֵ
// <800-20000>
#define YK_TEMPERATURE_MIN   (2200U)

// <o> ɫ�·�Χ���ֵ
// <800-20000>
#define YK_TEMPERATURE_MAX   (7500U)

/*WY ��2%-100%�����ڵ���*/
// <o> ��ůģʽ�µ���С���Ե���ϵ��
// <0-100>
#define YK_WY_LEVEL_MIN_COEFFICIENT   (2U)

// <o> ��ůģʽ�µ�������Ե���ϵ��
// <0-100>
#define YK_WY_LEVEL_MAX_COEFFICIENT   (100U)


// <s>  �豸����
#define DEVICE_NAME  "ABCD"

#define DEVICE_NAME_LEN    sizeof(DEVICE_NAME)


// <o> Image Size
// <0-0x40000>
#define IMAGE_SIZE   (0x35000U)

// <o> Firmware Version
// <0-0xFFFFFFFF>
#define Firmware_Version   (0x1)

// <<< end of configuration section >>>

#endif
