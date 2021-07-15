#include "BSP.h"

#if (BSP_LED_USE > 0u)

/*****************************************************************************
Function name :  BSP_LED_Init
Description:	 PWM初始化
Input:	         void
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_LED_Init(void)
{
	/*根据调光方式选择初始化对应的MCU外设*/
	/*调光方式选择*/
#if (DIMMER == DIMMER_WAY_PWM)
	/*PWM初始化*/
	BSP_PWM_Init();
	
#elif (DIMMER == DIMMER_WAY_IIC_WITH_SM2135EJ)
	/*初始化I2C和对应器件*/
	BSP_IIC_Init();
    BSP_SM2135EJ_Init();
	
#elif (DIMMER == DIMMER_WAY_IIC_WITH_BP1658CJ)

    /*初始化I2C和对应器件*/
	BSP_IIC_Init();
    BSP_BP1658CJ_Init();
#endif
}

/*****************************************************************************
Function name :  BSP_LED_SetWYLevel
Description:	 设置WY通道的灰度等级
Input:	         ulWLevel->W通道的灰度等级
                 ulYLevel->Y通道的灰度等级
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_LED_SetWYLevel(U32 ulWLevel, U32 ulYLevel)
{
#if (DIMMER == DIMMER_WAY_PWM)

    BSP_PWM_Set_CmpData(W_CHAANNEL, (U32)ulWLevel);
    BSP_PWM_Set_CmpData(Y_CHAANNEL, (U32)ulYLevel);  
                                            
#elif (DIMMER == DIMMER_WAY_IIC_WITH_SM2135EJ)
    BSP_SM2135EJ_SetWYLevel((U8)ulWLevel, (U8)ulYLevel);    
                                			
#elif (DIMMER == DIMMER_WAY_IIC_WITH_BP1658CJ)
    BSP_BP1658CJ_SetWYLevel((U16)ulWLevel, (U16)ulYLevel);                    

#endif   

}


#if (HW_RGB_ENABLE == 1)
/*****************************************************************************
Function name :  BSP_LED_SetRGBLevel
Description:	 设置RGB通道的灰度等级
Input:	         ulRLevel->R通道的灰度等级
                 ulGLevel->G通道的灰度等级
                 ulBLevel->B通道的灰度等级
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_LED_SetRGBLevel(U32 ulRLevel, U32 ulGLevel, U32 ulBLevel)
{  
#if (DIMMER == DIMMER_WAY_PWM)
    BSP_PWM_Set_CmpData(R_CHAANNEL, ulRLevel);
    BSP_PWM_Set_CmpData(G_CHAANNEL, ulGLevel);
    BSP_PWM_Set_CmpData(B_CHAANNEL, ulBLevel);                        
                                                      
#elif (DIMMER == DIMMER_WAY_IIC_WITH_SM2135EJ)
    BSP_SM2135EJ_SetRGBLevel((U8)ulRLevel, (U8)ulGLevel, (U8)ulBLevel);                       
                                                               
#elif (DIMMER == DIMMER_WAY_IIC_WITH_BP1658CJ)
    BSP_BP1658CJ_SetRGBLevel((U16)ulRLevel, (U16)ulGLevel, (U16)ulBLevel);                                   
                   
#endif   
}
#endif




#endif

