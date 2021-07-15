#ifndef  __BSP_GPIO_H__
#define  __BSP_GPIO_H__

#ifdef __cplusplus
 extern "C" {
#endif
     



/* GPIO Item*/
typedef enum 
{
	GPIO_ITEM_DTC = 0, /*过零检测口*/
    GPIO_ITEM_DIM,     /*调光口*/
    GPIO_ITEM_PLATFORM,/*运行平台选择*/
	GPIO_ITEM_MAX,	
    
}ENUM_GPIO_ITEM_BSP;

/* GPIO 配置结构体*/
typedef struct
{
	
	U32           GpioPort;
	U32           GpioPin;
	U8            Mode;
    U8            Pull;   /*!< GPIO pull-up */
    U32           OutputLogic;
    U32           ActiveLogic;//按键触发电平
    U32           ExtiMode;
    IRQn_Type     IRQn;
	tIRQ_TYPE     IRQType;
	IrqFun        Fun;
    //U8            Preemption;
    U8            IntEn;

}BSP_GPIO_CONFIG;   


/****************** GPIO 引脚映射**************************/
/*过零检测口*/
#define DTC_PORT                            3
#define DTC_PIN                             0

/*调光口*/
#define DIM_PORT                            3
#define DIM_PIN                             1

/*运行平台选择*/
#define PLATFORM_PORT                       5
#define PLATFORM_PIN                        2



/* Definition for GPIO EXTI NVIC */
#define  DTC_GPIO_IRQn                     GPIO234_IRQn

                                       

void BSP_GPIO_Init(void);
void BSP_GPIO_EXTI_Clear_IT(U32 GPIO_Port, U32 GPIO_Pin);
ITStatus BSP_GPIO_EXTI_Get_IT(U32 GPIO_Port, U32 GPIO_Pin);

U32 BSP_GPIO_ReadPin(U32 GPIO_Port, U32 GPIO_Pin);
void BSP_GPIO_EXTI_Init(U8 GpioItem, BOOL Enable);

extern const BSP_GPIO_CONFIG g_stGpioCfg[GPIO_ITEM_MAX];

#ifdef __cplusplus
}
#endif 

#endif

