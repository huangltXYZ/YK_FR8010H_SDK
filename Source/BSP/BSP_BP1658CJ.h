#ifndef  __BSP_BP1658CJ_H__
#define  __BSP_BP1658CJ_H__

#ifdef __cplusplus
extern "C" {
#endif

#if (HW_RGB_ENABLE == 1) 
#define HW_BP1658CJ_MAX_CURRENT_RGB    (6)/*硬件支持的最大RGB电流*/
#endif

#define HW_BP1658CJ_MAX_CURRENT_WY     (9) /*硬件支持的最大WY电流*/

#if (HW_RGB_ENABLE == 1) 
#define HW_BP1658CJ_RGB_LEVEL_MAX      (1023U)/*硬件支持的RGB最大灰度等级*/
#endif

#define HW_BP1658CJ_WY_LEVEL_MAX       (1023U)/*硬件支持的WY最大灰度等级*/
	
	

#define BP1658CJ_MAX_CURRENT_RGB_STEP    (10)/*10mA步进*/
#define BP1658CJ_MAX_CURRENT_WY_STEP     (5)/*5mA步进*/

/*BYTE0的bit0~bit3 地址选择*/
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

/*BYTE0的bit4~bit5 模式选择选择*/
typedef enum
{
	BP1658CJ_MODE_SLEEP = 0x00,/*睡眠模式*/
	BP1658CJ_MODE_RGB,         /*RGB工作*/
	BP1658CJ_MODE_WY,          /*WY工作*/
	BP1658CJ_MODE_RGB_WY,      /*5路都工作*/

	BP1658CJ_MODE_MAX,
	
}BP1658CJ_MODE;


/*BP1658CJ 最大电流控制*/
typedef struct
{
	U8 WY : 4;
	U8 RGB : 4;

}BP1658CJ_MAX_CURRENT;


/*BP1658CJ 控制位*/
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

