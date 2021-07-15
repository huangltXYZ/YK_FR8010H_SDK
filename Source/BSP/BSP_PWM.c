#include "BSP.h"

#if (BSP_PWM_USE > 0u)

/*****************************************************************************
Function name :  BSP_PWM_Init
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
void BSP_PWM_Init(void)
{
#if (HW_RGB_ENABLE == 1)
	system_set_port_mux(GPIO_PORT_A,GPIO_BIT_2,PORTA2_FUNC_PWM2);/*R*/
	system_set_port_mux(GPIO_PORT_A,GPIO_BIT_4,PORTA4_FUNC_PWM4);/*G*/
	system_set_port_mux(GPIO_PORT_A,GPIO_BIT_5,PORTA5_FUNC_PWM5);/*B*/
#endif
	system_set_port_mux(GPIO_PORT_A,GPIO_BIT_6,PORTA6_FUNC_PWM0);/*W*/
	system_set_port_mux(GPIO_PORT_A,GPIO_BIT_7,PORTA7_FUNC_PWM1);/*Y*/

#if (HW_RGB_ENABLE == 1)	
    pwm_init(R_CHAANNEL, PWM_FREQ, 0);
    pwm_init(G_CHAANNEL, PWM_FREQ, 0);
	pwm_init(B_CHAANNEL, PWM_FREQ, 0);
#endif
	
	pwm_init(W_CHAANNEL, PWM_FREQ, 0);
    pwm_init(Y_CHAANNEL, PWM_FREQ, 0);
	
#if (HW_RGB_ENABLE == 1)	
    pwm_start(R_CHAANNEL);
    pwm_start(G_CHAANNEL);
	pwm_start(B_CHAANNEL);
#endif

    pwm_start(W_CHAANNEL);
    pwm_start(Y_CHAANNEL);

}



/*****************************************************************************
Function name :  BSP_PWM_Set_DutyCycle
Description:	 修改指定通道的占空比
Input:	         ucChannel->PWM通道号
                 ucDutyCycle->占空比
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_PWM_Set_DutyCycle(U8 ucChannel, U8 ucDutyCycle)
{ 
   
}

/*****************************************************************************
Function name :  BSP_PWM_Set_CmpData
Description:	 修改指定通道的比较值
Input:	         ucChannel->PWM通道号
                 ulCmpData->比较值
Return:          void
Notes:           通过直接修改比较值的方式比修改占空比的方式调节更加精细化
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_PWM_Set_CmpData(U8 ucChannel, U32 ulCmpData)
{
	if(ucChannel >= PWM_CHANNEL_MAX)
	{
		return;
	}
	
	if(ulCmpData >= HW_PWM_LEVEL_MAX)
	{
		ulCmpData = HW_PWM_LEVEL_MAX - 1;/*FR801x占空比不能达到100%*/
	}

	pwm_set_high_cnt((enum pwm_channel_t)ucChannel, HW_PWM_LEVEL_MAX - ulCmpData);

}

#endif
