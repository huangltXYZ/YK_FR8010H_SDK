#include "BSP.h"

#if (BSP_GPIO_USE > 0u)


/**********************GPIO配置表***************************************/                         

const BSP_GPIO_CONFIG g_stGpioCfg[GPIO_ITEM_MAX] = 
{
    /*过零检测口*/
    {
        .GpioPort    = DTC_PORT,
        .GpioPin     = DTC_PIN,
        .Mode        = GPIO_MODE_INPUT,
        .Pull        = 0,/*不带上拉*/		
        .ExtiMode    = GPIO_INT_RISING,/*上升沿触发中断*/
        .IRQn        = DTC_GPIO_IRQn,
		.IRQType     = GPIO234_IRQ,
		.Fun         = GPIO234_IRQHandler,
        .IntEn       = DISABLE,  
    }, 

    /*调光口*/
    {
        .GpioPort    = DIM_PORT,
        .GpioPin     = DIM_PIN,
        .Mode        = GPIO_MODE_OUTPUT,
		.Pull        = 0,/*不带上拉*/
		.OutputLogic = 0,/*默认低电平*/
	    .IntEn       = DISABLE,
    }, 	
	
	/*运行平台选择*/
    {
        .GpioPort    = PLATFORM_PORT,
        .GpioPin     = PLATFORM_PIN,
        .Mode        = GPIO_MODE_INPUT,
		.Pull        = 0,/*不带上拉*/
		.ActiveLogic = 0,/*低电平触发*/
		.IntEn       = DISABLE,
    }, 	
};

/*****************************************************************************
Function name :  BSP_GPIO_Init
Description:	 初始化IO口
Input:	         void
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_GPIO_Init(void)
{
    U8 ucIndex;

    /* GPIO Ports Clock Enable */

    for(ucIndex = 0; ucIndex < GPIO_ITEM_MAX; ucIndex++)
    {
        /* Configures MCU GPIO */ 
        if(GPIO_MODE_OUTPUT == g_stGpioCfg[ucIndex].Mode) 
        {
            GPIO_PIN_ADDR(g_stGpioCfg[ucIndex].GpioPort, g_stGpioCfg[ucIndex].GpioPin) = g_stGpioCfg[ucIndex].OutputLogic;
        }
		
		GPIO_SetMode((GPIO_T*)(P0_BASE + g_stGpioCfg[ucIndex].GpioPort * 0x40), 1 << g_stGpioCfg[ucIndex].GpioPin, g_stGpioCfg[ucIndex].Mode);
	    GPIO_PullUp ((GPIO_T*)(P0_BASE + g_stGpioCfg[ucIndex].GpioPort * 0x40), 1 << g_stGpioCfg[ucIndex].GpioPin, g_stGpioCfg[ucIndex].Pull);
	
        if(ENABLE == g_stGpioCfg[ucIndex].IntEn) 
        {
			((interrupt_register_handler)SVC_interrupt_register)(g_stGpioCfg[ucIndex].IRQType, g_stGpioCfg[ucIndex].Fun);/*注册中断函数*/
			BSP_GPIO_EXTI_Clear_IT(g_stGpioCfg[ucIndex].GpioPort, g_stGpioCfg[ucIndex].GpioPin);/*清除中断标志*/
			GPIO_EnableInt((GPIO_T*)(P0_BASE + g_stGpioCfg[ucIndex].GpioPort * 0x40), g_stGpioCfg[ucIndex].GpioPin, g_stGpioCfg[ucIndex].ExtiMode);
            NVIC_EnableIRQ(g_stGpioCfg[ucIndex].IRQn);     
        }
    }   
}

/*****************************************************************************
Function name :  BSP_GPIO_EXTI_Init
Description:	 初始化GPIO外部中断
Input:	         GpioItem->GPIO编号;Enable->是否使能中断
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_GPIO_EXTI_Init(U8 GpioItem, BOOL Enable)
{
	if(TRUE == Enable) 
    {
		((interrupt_register_handler)SVC_interrupt_register)(g_stGpioCfg[GpioItem].IRQType, g_stGpioCfg[GpioItem].Fun);/*注册中断函数*/
		BSP_GPIO_EXTI_Clear_IT(g_stGpioCfg[GpioItem].GpioPort, g_stGpioCfg[GpioItem].GpioPin);/*清除中断标志*/
		GPIO_EnableInt((GPIO_T*)(P0_BASE + g_stGpioCfg[GpioItem].GpioPort * 0x40), g_stGpioCfg[GpioItem].GpioPin, g_stGpioCfg[GpioItem].ExtiMode);
//		NVIC_SetPriority(g_stGpioCfg[GpioItem].IRQn, 0);
		NVIC_EnableIRQ(g_stGpioCfg[GpioItem].IRQn);     
    }
	else
	{
		BSP_GPIO_EXTI_Clear_IT(g_stGpioCfg[GpioItem].GpioPort, g_stGpioCfg[GpioItem].GpioPin);/*清除中断标志*/
		GPIO_DisableInt((GPIO_T*)(P0_BASE + g_stGpioCfg[GpioItem].GpioPort * 0x40), g_stGpioCfg[GpioItem].GpioPin);
		NVIC_DisableIRQ(g_stGpioCfg[GpioItem].IRQn);
	}
}
/*****************************************************************************
Function name :  BSP_GPIO_EXTI_Clear_IT
Description:	 外部清中断标志
Input:	         GPIOx->GPIO端口号
                 GPIO_Pin->GPIO Pin
Return:          
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
ITStatus BSP_GPIO_EXTI_Get_IT(U32 GPIO_Port, U32 GPIO_Pin)
{
	/*输入参数合法性判断*/
	if(((GPIO_T*)(P0_BASE + GPIO_Port * 0x40))->INTSRC & (1 << GPIO_Pin))
	{
		return SET;
	}
	else
	{
		return RESET;
	
	}
	
}

		
/*****************************************************************************
Function name :  BSP_GPIO_EXTI_Clear_IT
Description:	 外部清中断标志
Input:	         GPIOx->GPIO端口号
                 GPIO_Pin->GPIO Pin
Return:          
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_GPIO_EXTI_Clear_IT(U32 GPIO_Port, U32 GPIO_Pin)
{
	/*输入参数合法性判断*/
	
	((GPIO_T*)(P0_BASE + GPIO_Port * 0x40))->INTSRC = (1 << GPIO_Pin);/*清除中断标志*/
}


/*****************************************************************************
Function name :  BSP_GPIO_ReadPin
Description:	 读取对应IO的电平
Input:	         GPIOx->GPIO端口号
                 GPIO_Pin->GPIO Pin
Return:          IO电平状态 0->低电平   1->高电平
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
U32 BSP_GPIO_ReadPin(U32 GPIO_Port, U32 GPIO_Pin)
{
	return GPIO_PIN_ADDR(GPIO_Port, GPIO_Pin);
}


#endif




