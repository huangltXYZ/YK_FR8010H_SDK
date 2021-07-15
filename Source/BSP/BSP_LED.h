#ifndef  __BSP_LED_H__
#define  __BSP_LED_H__

#ifdef __cplusplus
extern "C" {
#endif

/**************Ӳ��֧�ֵ�RGB WY ���Ҷȵȼ�*****************/
#if (DIMMER == DIMMER_WAY_PWM)

    #if (HW_RGB_ENABLE == 1)
        #define HW_RGB_LEVEL_MAX      (HW_PWM_RGB_LEVEL_MAX)/*Ӳ��֧�ֵ�RGB���Ҷȵȼ�*/
    #endif

    #define HW_WY_LEVEL_MAX           (HW_PWM_WY_LEVEL_MAX)/*Ӳ��֧�ֵ�WY���Ҷȵȼ�*/

#elif (DIMMER == DIMMER_WAY_IIC_WITH_SM2135EJ)

    #if (HW_RGB_ENABLE == 1)
        #define HW_RGB_LEVEL_MAX      (HW_SM2135EJ_RGB_LEVEL_MAX)/*Ӳ��֧�ֵ�RGB���Ҷȵȼ�*/
    #endif
    
    #define HW_WY_LEVEL_MAX           (HW_SM2135EJ_WY_LEVEL_MAX)/*Ӳ��֧�ֵ�WY���Ҷȵȼ�*/

#elif (DIMMER == DIMMER_WAY_IIC_WITH_BP1658CJ)

    #if (HW_RGB_ENABLE == 1)
        #define HW_RGB_LEVEL_MAX      (HW_BP1658CJ_RGB_LEVEL_MAX)/*Ӳ��֧�ֵ�RGB���Ҷȵȼ�*/
    #endif
    
    #define HW_WY_LEVEL_MAX           (HW_BP1658CJ_WY_LEVEL_MAX)/*Ӳ��֧�ֵ�WY���Ҷȵȼ�*/

#endif


void BSP_LED_Init(void);
void BSP_LED_SetWYLevel(U32 ulWLevel, U32 ulYLevel);

#if (HW_RGB_ENABLE == 1)
void BSP_LED_SetRGBLevel(U32 ulRLevel, U32 ulGLevel, U32 ulBLevel);
#endif

#ifdef __cplusplus
}
#endif

#endif

