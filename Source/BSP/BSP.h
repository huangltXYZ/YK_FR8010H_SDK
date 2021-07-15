#ifndef __BSP_H__
#define __BSP_H__

#ifdef __cplusplus
 extern "C" {
#endif

/************通用头文件**************/
#include "Common.h"

/************工程配置头文件**************/
#include "YK_Light_Config.h"

/************SDK相关头文件**************/
#include "driver_plf.h"
#include "driver_system.h"
#include "driver_i2s.h"
#include "driver_pmu.h"
#include "driver_uart.h"
#include "driver_gpio.h"
#include "driver_flash.h"
#include "driver_efuse.h"
//#include "flash_usage_config.h"
#include "driver_timer.h"
#include "driver_pwm.h"

#include "driver_gpio.h"


#include "FR801x_it.h"
  
#include "jump_table.h"

typedef void(*IrqFun)(void); /* 定义中断处理函数*/



/*MCU外设配置*/
#define BSP_GPIO_USE        0
#define BSP_KEY_USE         0
#define BSP_UART_USE        1
#define BSP_TIMER_USE       1
#define BSP_SPI_USE         0
#define BSP_WDT_USE         0
#define BSP_IAP_USE         1

/*外围驱动配置*/
#define BSP_ZB25D_USE       0


/*调光方式选择*/
#define BSP_LED_USE         1

#if (BSP_LED_USE > 0u)

	#if (DIMMER == DIMMER_WAY_PWM)
		#define BSP_PWM_USE         1
		
	#elif (DIMMER == DIMMER_WAY_IIC_WITH_SM2135EJ)
		#define BSP_IIC_USE         1
        #define BSP_SM2135EJ_USE    1
		
	#elif (DIMMER == DIMMER_WAY_IIC_WITH_BP1658CJ)
		#define BSP_IIC_USE         1
        #define BSP_BP1658CJ_USE    1	
	#else
		#warning "Wrong Dimmer Way!"
	#endif

#endif



#if BSP_GPIO_USE
    #include "BSP_GPIO.h"
#endif

#if BSP_KEY_USE
    #include "BSP_KEY.h"
#endif  

#if BSP_UART_USE
    #include "BSP_UART.h"
#endif
     
#if BSP_TIMER_USE
#include "BSP_TIMER.h"
#endif

#if BSP_SPI_USE
    #include "BSP_SPI.h"
#endif 

#if BSP_PWM_USE
    #include "BSP_PWM.h"
#endif 

#if BSP_IIC_USE
    #include "BSP_IIC.h"
#endif 

#if BSP_WDT_USE
    #include "BSP_WDT.h"
#endif 

#if BSP_IAP_USE
    #include "BSP_IAP.h"
#endif 

#if BSP_ZB25D_USE
    #include "BSP_ZB25D.h"
#endif 

#if BSP_SM2135EJ_USE
    #include "BSP_SM2135EJ.h"
#endif 

#if BSP_BP1658CJ_USE
    #include "BSP_BP1658CJ.h"
#endif 

#if BSP_LED_USE
    #include "BSP_LED.h"
#endif

/***********************定义各个外设的中断优先级*************************************/  
#define TIM0_INT_PRIORITY_DEF         (3U)/*任务时基*/	
#define TIM1_INT_PRIORITY_DEF         (2U)/*Light控制*/
 
void BSP_Init(void);

#ifdef __cplusplus
}
#endif 

#endif


