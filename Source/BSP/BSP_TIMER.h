#ifndef   __BSP_TIMER_H__
#define   __BSP_TIMER_H__

#ifdef __cplusplus
 extern "C" {
#endif

/*��ʱ�����*/
typedef enum 
{
	TIM_CHNL_TIME_TIME_BASE = 0,/*����ʱ��*/	
	TIM_CHNL_TIME_LIGHT,        /*Light����*/
	TIM_CHNL_MAX,		
}ENUM_TIM_CHNL_BSP;

typedef struct
{
    U32 Mode;     /*��ʱ��ģʽ ���� or ����*/
    U32 ulPeriod; /*��ʱ������*/
	
}TIM_CFG_STRUCT;



void BSP_TIMER_Init(U8 ucChannel);


#ifdef __cplusplus
}
#endif 

#endif


