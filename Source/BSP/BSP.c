#include "BSP.h"

static void SystemClock_Config(void);


/*****************************************************************************
Function name :  BSP_Init
Description:	 驱动层初始化
Input:	         void
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_Init(void)
{
	/*系统时钟初始化*/
	SystemClock_Config();
	
#if	BSP_GPIO_USE
	BSP_GPIO_Init();
#endif 

#if BSP_KEY_USE
    BSP_KEY_Init();
#endif 

#if BSP_UART_USE
    BSP_UART_Init(UART_CHNL_HCI);
#endif 

#if BSP_TIMER_USE
    /*定时器初始化 时基定时器/Light控制定时器*/
    BSP_TIMER_Init(TIM_CHNL_TIME_TIME_BASE);
	BSP_TIMER_Init(TIM_CHNL_TIME_LIGHT);
#endif 


#if BSP_SPI_USE
    BSP_SPI_Init(SPI_CHNL_FLASH);
#endif 

#if BSP_LED_USE
    BSP_LED_Init();
#endif 

#if BSP_WDT_USE
    BSP_WDT_Init();
#endif 
}

/*****************************************************************************
Function name :  SystemClock_Config
Description:	 系统时钟初始化
Input:	         void
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
static void SystemClock_Config(void)
{
	
}


