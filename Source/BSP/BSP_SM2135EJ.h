#ifndef  __BSP_SM2135EJ_H__
#define  __BSP_SM2135EJ_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "TC620Series.h"
#include "tcip_config.h"

#include "Common.h"

#include "BSP_IIC.h"

#if (HW_RGB_ENABLE == 1) 
#define HW_MAX_CURRENT_RGB    (SM2135EJ_RGB_MAX_CURRENT_14P3)/*硬件支持的最大RGB电流*/
#endif

#define HW_MAX_CURRENT_WY     (SM2135EJ_WY_MAX_CURRENT_50P0) /*硬件支持的最大WY电流*/

#if (HW_RGB_ENABLE == 1) 
#define HW_SM2135EJ_RGB_LEVEL_MAX      (255U)/*硬件支持的RGB最大灰度等级*/
#endif

#define HW_SM2135EJ_WY_LEVEL_MAX       (233U)/*硬件支持的WY最大灰度等级*/
	

/* Byte0 为地址指令。 0xC0~0xC6 选择往 Byte1~Byte7 地址位写数据，地址初始化，
   其中 Byte1 为最大电流控制指令， Byte2 为待机功能选择， Byte3~Byte7 为 R/G/B/W/Y 灯的灰度数据。
*/
typedef enum
{
	SM2135EJ_ADDR_CMD_SET_MAX_CURRENT = 0xC0,
	SM2135EJ_ADDR_CMD_SET_WORK_MODE,
	SM2135EJ_ADDR_CMD_SET_R_LEVEL,
	SM2135EJ_ADDR_CMD_SET_G_LEVEL,
	SM2135EJ_ADDR_CMD_SET_B_LEVEL,
	SM2135EJ_ADDR_CMD_SET_W_LEVEL,
	SM2135EJ_ADDR_CMD_SET_Y_LEVEL,
	
	SM2135EJ_ADDR_CMD_MAX,
	
}SM2135EJ_ADDR_CMD;

/*RGB 最大电流控制*/
typedef enum
{
	SM2135EJ_RGB_MAX_CURRENT_14P3 = 0x00,
	SM2135EJ_RGB_MAX_CURRENT_21P4,
	SM2135EJ_RGB_MAX_CURRENT_28P5,
	SM2135EJ_RGB_MAX_CURRENT_35P6,
	SM2135EJ_RGB_MAX_CURRENT_42P7,
	SM2135EJ_RGB_MAX_CURRENT_49P8,
	SM2135EJ_RGB_MAX_CURRENT_56P9,
	SM2135EJ_RGB_MAX_CURRENT_64P0,
	
	SM2135EJ_RGB_MAX_CURRENT_MAX,
	
}SM2135EJ_RGB_MAX_CURRENT;


/*W Y 最大电流控制*/
typedef enum
{
	SM2135EJ_WY_MAX_CURRENT_0P0 = 0x00,
	SM2135EJ_WY_MAX_CURRENT_7P3,
	SM2135EJ_WY_MAX_CURRENT_14P4,
	SM2135EJ_WY_MAX_CURRENT_21P5,
	SM2135EJ_WY_MAX_CURRENT_28P6,
	SM2135EJ_WY_MAX_CURRENT_35P7,
	SM2135EJ_WY_MAX_CURRENT_42P8,
	SM2135EJ_WY_MAX_CURRENT_50P0,
	SM2135EJ_WY_MAX_CURRENT_56P7,
	SM2135EJ_WY_MAX_CURRENT_63P5,
	SM2135EJ_WY_MAX_CURRENT_70P2,
	SM2135EJ_WY_MAX_CURRENT_77P0,
	SM2135EJ_WY_MAX_CURRENT_83P8,
	SM2135EJ_WY_MAX_CURRENT_90P0,

	SM2135EJ_WY_MAX_CURRENT_MAX,
	
}SM2135EJ_WY_MAX_CURRENT;


/*待机功能控制*/
typedef enum
{
	SM2135EJ_WORK_MODE_STANDBY  = 0x20,/*待机*/
	SM2135EJ_WORK_MODE_NORMAL = 0x00,/*正常工作状态*/
	
}SM2135EJ_WORK_MODE;

/*SM2135EJ 最大电流控制*/
typedef struct
{
	SM2135EJ_RGB_MAX_CURRENT RGB;
	SM2135EJ_WY_MAX_CURRENT WY;
	
}SM2135EJ_MAX_CURRENT;

void BSP_SM2135EJ_Init(void);
void BSP_SM2135EJ_SetMaxCurrent(SM2135EJ_MAX_CURRENT MaxCurrent);
void BSP_SM2135EJ_SetWorkMode(SM2135EJ_WORK_MODE WorkMode);
void BSP_SM2135EJ_SetWYLevel(U8 WLevel, U8 YLevel);

#if (HW_RGB_ENABLE == 1)
void BSP_SM2135EJ_SetRGBLevel(U8 RLevel, U8 GLevel, U8 BLevel);
#endif






#ifdef __cplusplus
}
#endif

#endif

