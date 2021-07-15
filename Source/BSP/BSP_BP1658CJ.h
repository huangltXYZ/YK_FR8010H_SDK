#ifndef  __BSP_BP1658CJ_H__
#define  __BSP_BP1658CJ_H__

#ifdef __cplusplus
extern "C" {
#endif

#if (HW_RGB_ENABLE == 1) 
#define HW_BP1658CJ_MAX_CURRENT_RGB    (6)/*Ӳ��֧�ֵ����RGB����*/
#endif

#define HW_BP1658CJ_MAX_CURRENT_WY     (9) /*Ӳ��֧�ֵ����WY����*/

#if (HW_RGB_ENABLE == 1) 
#define HW_BP1658CJ_RGB_LEVEL_MAX      (1023U)/*Ӳ��֧�ֵ�RGB���Ҷȵȼ�*/
#endif

#define HW_BP1658CJ_WY_LEVEL_MAX       (1023U)/*Ӳ��֧�ֵ�WY���Ҷȵȼ�*/
	
	

#define BP1658CJ_MAX_CURRENT_RGB_STEP    (10)/*10mA����*/
#define BP1658CJ_MAX_CURRENT_WY_STEP     (5)/*5mA����*/

/*BYTE0��bit0~bit3 ��ַѡ��*/
typedef enum
{
	BP1658CJ_ADDR_SET_MAX_CURRENT = 0x00,
	BP1658CJ_ADDR_SET_B_LEVEL,
	BP1658CJ_ADDR_SET_G_LEVEL,
	BP1658CJ_ADDR_SET_R_LEVEL,
	BP1658CJ_ADDR_SET_Y_LEVEL = 0x07,
	BP1658CJ_ADDR_SET_W_LEVEL,
	
	BP1658CJ_ADDR_MAX,
	
}BP1658CJ_ADDR;

/*BYTE0��bit4~bit5 ģʽѡ��ѡ��*/
typedef enum
{
	BP1658CJ_MODE_SLEEP = 0x00,/*˯��ģʽ*/
	BP1658CJ_MODE_RGB,         /*RGB����*/
	BP1658CJ_MODE_WY,          /*WY����*/
	BP1658CJ_MODE_RGB_WY,      /*5·������*/

	BP1658CJ_MODE_MAX,
	
}BP1658CJ_MODE;


/*BP1658CJ ����������*/
typedef struct
{
	U8 WY : 4;
	U8 RGB : 4;

}BP1658CJ_MAX_CURRENT;


/*BP1658CJ ����λ*/
typedef struct
{
	U8 Addr     : 4;
	U8 WorkMode : 2;
	U8 ID       : 2;
	
}BP1658CJ_CTRL;


void BSP_BP1658CJ_Init(void);
void BSP_BP1658CJ_SetMaxCurrent(BP1658CJ_MAX_CURRENT MaxCurrent);
void BSP_BP1658CJ_SetWorkMode(BP1658CJ_MODE WorkMode);
void BSP_BP1658CJ_SetWYLevel(U16 WLevel, U16 YLevel);

#if (HW_RGB_ENABLE == 1)
void BSP_BP1658CJ_SetRGBLevel(U16 RLevel, U16 GLevel, U16 BLevel);
#endif






#ifdef __cplusplus
}
#endif

#endif

