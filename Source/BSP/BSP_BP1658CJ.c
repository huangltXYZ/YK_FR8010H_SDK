#include "BSP.h"

#if (BSP_BP1658CJ_USE > 0u)

#if (DIMMER == DIMMER_WAY_IIC_WITH_BP1658CJ)

#define BP1658CJ_ID    (2)/*CTRL位的ID 10*/ 

/*默认的工作模式*/
#if (HW_RGB_ENABLE == 1) 
    #define BP1658CJ_WORK_MODE_DEF    (BP1658CJ_MODE_RGB_WY)
#else
	#define BP1658CJ_WORK_MODE_DEF    (BP1658CJ_MODE_WY)
#endif

//static void BSP_BP1658CJ_TEST(void);
/*****************************************************************************
Function name :  BSP_BP1658CJ_Init
Description:	 BP1658CJ初始化
Input:	         void
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_BP1658CJ_Init(void)
{
	BP1658CJ_MAX_CURRENT MaxCurrent;

    memset(&MaxCurrent, 0x00, sizeof(BP1658CJ_MAX_CURRENT));

#if (HW_RGB_ENABLE == 1) 
    MaxCurrent.RGB = HW_BP1658CJ_MAX_CURRENT_RGB;
#endif

    MaxCurrent.WY = HW_BP1658CJ_MAX_CURRENT_WY;

	/*选择5路同时输出*/
    BSP_BP1658CJ_SetMaxCurrent(MaxCurrent);
}

/*****************************************************************************
Function name :  BSP_BP1658CJ_SetMaxCurrent
Description:	 设置最大电流
Input:	         MaxCurrent->RGB和WY的最大电流
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_BP1658CJ_SetMaxCurrent(BP1658CJ_MAX_CURRENT MaxCurrent)
{
	BP1658CJ_CTRL Ctrl;
    /*参数合法性判断*/

    BSP_IIC_Start();

	Ctrl.ID       = BP1658CJ_ID;
	Ctrl.WorkMode = BP1658CJ_WORK_MODE_DEF;
	Ctrl.Addr     = BP1658CJ_ADDR_SET_MAX_CURRENT;
	
    BSP_IIC_SendData(*((U8*)&Ctrl));
    BSP_IIC_SendData(*((U8*)&MaxCurrent));

    BSP_IIC_Stop();
}

/*****************************************************************************
Function name :  BSP_BP1658CJ_SetWorkMode
Description:	 设置工作模式 待机 or 正常工作 RGB or WY or RGB WY
Input:	         WorkMode->工作模式
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_BP1658CJ_SetWorkMode(BP1658CJ_MODE WorkMode)
{
	BP1658CJ_CTRL Ctrl;
    /*参数合法性判断*/

    BSP_IIC_Start();

	Ctrl.ID       = BP1658CJ_ID;
	Ctrl.WorkMode = WorkMode;
	Ctrl.Addr     = BP1658CJ_ADDR_SET_MAX_CURRENT;
	
    BSP_IIC_Start();

    BSP_IIC_SendData(*((U8*)&Ctrl));

    BSP_IIC_Stop();
}

/*****************************************************************************
Function name :  BSP_BP1658CJ_SetWYLevel
Description:	 设置WY通道的灰度等级
Input:	         WLevel->W通道的灰度等级
                 YLevel->Y通道的灰度等级
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_BP1658CJ_SetWYLevel(U16 WLevel, U16 YLevel)
{
	U16 Temp;
	BP1658CJ_CTRL Ctrl;
	U8 HighByte = 0;
	U8 LowByte = 0;
	
	/*硬件上交换了W和Y*/
	Temp = YLevel;
	YLevel = WLevel;
	WLevel = Temp;
	
    /*参数合法性判断*/
	WLevel = (WLevel > HW_BP1658CJ_WY_LEVEL_MAX) ? (WLevel = HW_BP1658CJ_WY_LEVEL_MAX) : WLevel;
	YLevel = (YLevel > HW_BP1658CJ_WY_LEVEL_MAX) ? (YLevel = HW_BP1658CJ_WY_LEVEL_MAX) : YLevel;
	
	/*WY两路的和不能超过硬件支持的最大门限*/
	if(WLevel + YLevel > HW_BP1658CJ_WY_LEVEL_MAX)
	{
	    return;
	}

	Ctrl.ID       = BP1658CJ_ID;
	Ctrl.WorkMode = BP1658CJ_WORK_MODE_DEF;
	Ctrl.Addr     = BP1658CJ_ADDR_SET_Y_LEVEL;
	
    BSP_IIC_Start();
	
	BSP_IIC_SendData(*((U8*)&Ctrl));
	
    /*YLevel 设置*/
	LowByte  = (U8)(YLevel & 0x1F);
	HighByte = (U8)((YLevel >> 5) & 0x1F);
    BSP_IIC_SendData(LowByte);
    BSP_IIC_SendData(HighByte);
	
	/*WLevel 设置*/
	LowByte  = (U8)(WLevel & 0x1F);
	HighByte = (U8)((WLevel >> 5) & 0x1F);
    BSP_IIC_SendData(LowByte);
    BSP_IIC_SendData(HighByte);
	
    BSP_IIC_Stop();
}

#if (HW_RGB_ENABLE == 1)
/*****************************************************************************
Function name :  BSP_BP1658CJ_SetRGBLevel
Description:	 设置RGB通道的灰度等级
Input:	         RLevel->R通道的灰度等级
                 GLevel->G通道的灰度等级
                 BLevel->B通道的灰度等级
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_BP1658CJ_SetRGBLevel(U16 RLevel, U16 GLevel, U16 BLevel)
{
	BP1658CJ_CTRL Ctrl;
	U8 HighByte = 0;
	U8 LowByte = 0;
	
    /*参数合法性判断*/
    RLevel = (RLevel > HW_BP1658CJ_RGB_LEVEL_MAX) ? (RLevel = HW_BP1658CJ_RGB_LEVEL_MAX) : RLevel;
	GLevel = (GLevel > HW_BP1658CJ_RGB_LEVEL_MAX) ? (GLevel = HW_BP1658CJ_RGB_LEVEL_MAX) : GLevel;
	BLevel = (BLevel > HW_BP1658CJ_RGB_LEVEL_MAX) ? (BLevel = HW_BP1658CJ_RGB_LEVEL_MAX) : BLevel;
	
	Ctrl.ID       = BP1658CJ_ID;
	Ctrl.WorkMode = BP1658CJ_WORK_MODE_DEF;
	Ctrl.Addr     = BP1658CJ_ADDR_SET_B_LEVEL;
	
    BSP_IIC_Start();

    BSP_IIC_SendData(*((U8*)&Ctrl));
	
    /*BLevel 设置*/
	LowByte  = (U8)(BLevel & 0x1F);
	HighByte = (U8)((BLevel >> 5) & 0x1F);
    BSP_IIC_SendData(LowByte);
    BSP_IIC_SendData(HighByte);
	
    /*GLevel 设置*/
	LowByte  = (U8)(GLevel & 0x1F);
	HighByte = (U8)((GLevel >> 5) & 0x1F);
    BSP_IIC_SendData(LowByte);
    BSP_IIC_SendData(HighByte);
	
    /*RLevel 设置*/
	LowByte  = (U8)(RLevel & 0x1F);
	HighByte = (U8)((RLevel >> 5) & 0x1F);
    BSP_IIC_SendData(LowByte);
    BSP_IIC_SendData(HighByte);
	
    BSP_IIC_Stop();
}
#endif


///*****************************************************************************
//Function name :  BSP_BP1658CJ_SetMaxCurrent
//Description:	 设置最大电流
//Input:	         MaxCurrent->RGB和WY的最大电流
//Return:          void
//Notes:
//-----------------------------------------------------------------------------
//Modification History:
//<No.> <version>	  <time>	   <author>  <contents>
//2、
//1、   1.00 		  ####-##-##   hlt     build this module
//******************************************************************************/
//static void BSP_BP1658CJ_TEST(void)
//{
//	BP1658CJ_CTRL Ctrl;
//    /*参数合法性判断*/

//    BSP_IIC_Start();

//	BSP_IIC_SendData(0xB0);
//	
//    BSP_IIC_SendData(0x6C);
//	
//	BSP_IIC_SendData(0xA2);
//    BSP_IIC_SendData(0xA0);
//	
//	BSP_IIC_SendData(0xA0);
//	BSP_IIC_SendData(0xB0);
//	
//	BSP_IIC_SendData(0xBE);
//	BSP_IIC_SendData(0xBF);
//	
//	BSP_IIC_SendData(0xA0);
//	BSP_IIC_SendData(0xB0);
//	
//	BSP_IIC_SendData(0xBE);
//	BSP_IIC_SendData(0xBF);
//	
//    BSP_IIC_Stop();
//}

#endif

#endif
