#ifndef   __BSP_TIMER_H__
#define   __BSP_TIMER_H__

#ifdef __cplusplus
 extern "C" {
#endif

/*定时器编号*/
typedef enum 
{
	TIM_CHNL_TIME_TIME_BASE = 0,/*任务时基*/	
	TIM_CHNL_TIME_LIGHT,        /*Light控制*/
	TIM_CHNL_MAX,		
}ENUM_TIM_CHNL_BSP;

typedef struct
{
    U32 Mode;     /*定时器模式 单次 or 周期*/
    U32 ulPeriod; /*定时器周期*/
	
}TIM_CFG_STRUCT;



void BSP_TIMER_Init(U8 ucChannel);


#ifdef __cplusplus
}
#endif 

#endif


