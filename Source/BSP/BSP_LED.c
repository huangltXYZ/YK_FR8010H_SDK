#include "BSP.h"

#if (BSP_LED_USE > 0u)

/*****************************************************************************
Function name :  BSP_LED_Init
Description:	 PWM��ʼ��
Input:	         void
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_LED_Init(void)
{
	/*���ݵ��ⷽʽѡ���ʼ����Ӧ��MCU����*/
	/*���ⷽʽѡ��*/
#if (DIMMER == DIMMER_WAY_PWM)
	/*PWM��ʼ��*/
	BSP_PWM_Init();
	
#elif (DIMMER == DIMMER_WAY_IIC_WITH_SM2135EJ)
	/*��ʼ��I2C�Ͷ�Ӧ����*/
	BSP_IIC_Init();
    BSP_SM2135EJ_Init();
	
#elif (DIMMER == DIMMER_WAY_IIC_WITH_BP1658CJ)

    /*��ʼ��I2C�Ͷ�Ӧ����*/
	BSP_IIC_Init();
    BSP_BP1658CJ_Init();
#endif
}

/*****************************************************************************
Function name :  BSP_LED_SetWYLevel
Description:	 ����WYͨ���ĻҶȵȼ�
Input:	         ulWLevel->Wͨ���ĻҶȵȼ�
                 ulYLevel->Yͨ���ĻҶȵȼ�
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
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
Description:	 ����RGBͨ���ĻҶȵȼ�
Input:	         ulRLevel->Rͨ���ĻҶȵȼ�
                 ulGLevel->Gͨ���ĻҶȵȼ�
                 ulBLevel->Bͨ���ĻҶȵȼ�
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
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

