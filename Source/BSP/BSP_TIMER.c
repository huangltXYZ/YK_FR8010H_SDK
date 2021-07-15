#include "BSP.h"

#if (BSP_TIMER_USE > 0u)


/********��ʱ�����ñ�*****************/
const TIM_CFG_STRUCT s_stTimCfg[TIM_CHNL_MAX] = 
{
    /******TIM_CHNL_TIME_TIME_BASE************************/
    {
        .Mode     = TIMER_PERIODIC,/*������*/
        .ulPeriod = 1000,/*1000us������*/
    },
	
    /******TIM_CHNL_TIME_LIGHT************************/
    {
        .Mode     = TIMER_PERIODIC,/*������*/
        .ulPeriod = 10000,/*1000 0us������*/
    },	
	
};


/*****************************************************************************
Function name :  BSP_TIMER_Init
Description:	 ��ʱ����ʼ��
Input:	         ucChannel->��ʱ��ͨ�����
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_TIMER_Init(U8 ucChannel)
{  
    if(ucChannel >= TIM_CHNL_MAX)
    {
        return;
    }
    
    if(TIM_CHNL_TIME_TIME_BASE == ucChannel)
    {
		timer_init(TIMER0, s_stTimCfg[ucChannel].ulPeriod, s_stTimCfg[ucChannel].Mode);
        //timer_run(TIMER0);
		NVIC_SetPriority(TIMER0_IRQn, TIM0_INT_PRIORITY_DEF);
        NVIC_EnableIRQ(TIMER0_IRQn);
		
    }
	else if(TIM_CHNL_TIME_LIGHT == ucChannel)
    {
		timer_init(TIMER0, s_stTimCfg[ucChannel].ulPeriod, s_stTimCfg[ucChannel].Mode);
        //timer_run(TIMER1);
		NVIC_SetPriority(TIMER1_IRQn, TIM1_INT_PRIORITY_DEF);
        NVIC_EnableIRQ(TIMER1_IRQn);
    }  
}


#endif
