#include "BSP.h"

#if (BSP_SM2135EJ_USE > 0u)

/*****************************************************************************
Function name :  BSP_SM2135EJ_Init
Description:	 SM2135EJ初始化
Input:	         void
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_SM2135EJ_Init(void)
{
	SM2135EJ_MAX_CURRENT MaxCurrent;

    memset(&MaxCurrent, 0x00, sizeof(SM2135EJ_MAX_CURRENT));
        
#if (HW_RGB_ENABLE == 1) 
    MaxCurrent.RGB = HW_MAX_CURRENT_RGB;
#endif

    MaxCurrent.WY = HW_MAX_CURRENT_WY;

    BSP_SM2135EJ_SetMaxCurrent(MaxCurrent);
    BSP_SM2135EJ_SetWorkMode(SM2135EJ_WORK_MODE_NORMAL);

//    BSP_SM2135EJ_SetRGBLevel(255,0,0);
//    while(1);
}

/*****************************************************************************
Function name :  BSP_SM2135EJ_SetMaxCurrent
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
void BSP_SM2135EJ_SetMaxCurrent(SM2135EJ_MAX_CURRENT MaxCurrent)
{
    /*参数合法性判断*/
    if((MaxCurrent.RGB >= SM2135EJ_RGB_MAX_CURRENT_MAX)  || (MaxCurrent.WY >= SM2135EJ_WY_MAX_CURRENT_MAX))
    {
        return;
    }
    
    BSP_IIC_Start();

    BSP_IIC_SendData(SM2135EJ_ADDR_CMD_SET_MAX_CURRENT);
    BSP_IIC_SendData(MaxCurrent.WY | (MaxCurrent.RGB << 4));

    BSP_IIC_Stop();
}

/*****************************************************************************
Function name :  BSP_SM2135EJ_SetWorkMode
Description:	 设置工作模式 待机 or 正常工作
Input:	         WorkMode->工作模式
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_SM2135EJ_SetWorkMode(SM2135EJ_WORK_MODE WorkMode)
{
    /*参数合法性判断*/
    if((WorkMode != SM2135EJ_WORK_MODE_STANDBY)  && (WorkMode != SM2135EJ_WORK_MODE_NORMAL))
    {
        return;
    }
    
    BSP_IIC_Start();

    BSP_IIC_SendData(SM2135EJ_ADDR_CMD_SET_WORK_MODE);
    BSP_IIC_SendData(WorkMode);

    BSP_IIC_Stop();
}

/*****************************************************************************
Function name :  BSP_SM2135EJ_SetWYLevel
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
void BSP_SM2135EJ_SetWYLevel(U8 WLevel, U8 YLevel)
{
    /*参数合法性判断*/
	WLevel = (WLevel > HW_SM2135EJ_WY_LEVEL_MAX) ? (WLevel = HW_SM2135EJ_WY_LEVEL_MAX) : WLevel;
	YLevel = (YLevel > HW_SM2135EJ_WY_LEVEL_MAX) ? (YLevel = HW_SM2135EJ_WY_LEVEL_MAX) : YLevel;
	
	/*WY两路的和不能超过硬件支持的最大门限*/
	if(WLevel + YLevel > HW_SM2135EJ_WY_LEVEL_MAX)
	{
	    return;
	}

    BSP_IIC_Start();

    BSP_IIC_SendData(SM2135EJ_ADDR_CMD_SET_W_LEVEL);
    BSP_IIC_SendData(WLevel);
    BSP_IIC_SendData(YLevel);
	
    BSP_IIC_Stop();
}

#if (HW_RGB_ENABLE == 1)
/*****************************************************************************
Function name :  BSP_SM2135EJ_SetRGBLevel
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
void BSP_SM2135EJ_SetRGBLevel(U8 RLevel, U8 GLevel, U8 BLevel)
{
    /*参数合法性判断*/
    RLevel = (RLevel > HW_SM2135EJ_RGB_LEVEL_MAX) ? (RLevel = HW_SM2135EJ_RGB_LEVEL_MAX) : RLevel;
	GLevel = (GLevel > HW_SM2135EJ_RGB_LEVEL_MAX) ? (GLevel = HW_SM2135EJ_RGB_LEVEL_MAX) : GLevel;
	BLevel = (BLevel > HW_SM2135EJ_RGB_LEVEL_MAX) ? (BLevel = HW_SM2135EJ_RGB_LEVEL_MAX) : BLevel;
	
    BSP_IIC_Start();

    BSP_IIC_SendData(SM2135EJ_ADDR_CMD_SET_R_LEVEL);
	
//    BSP_IIC_SendData(RLevel);
//    BSP_IIC_SendData(GLevel);
//    BSP_IIC_SendData(BLevel);
	/*注：这版硬件R和B接反了，所以软件上做调整*/
	BSP_IIC_SendData(BLevel);
    BSP_IIC_SendData(GLevel);
    BSP_IIC_SendData(RLevel);
	

    BSP_IIC_Stop();
}
#endif

#endif
