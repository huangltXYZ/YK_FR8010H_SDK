#ifndef  __BSP_PWM_H__
#define  __BSP_PWM_H__

#ifdef __cplusplus
extern "C" {
#endif

#if (HW_RGB_ENABLE == 1)    

/* RGB ͨ��*/
#define R_CHAANNEL      PWM_CHANNEL_2
#define G_CHAANNEL      PWM_CHANNEL_4
#define B_CHAANNEL      PWM_CHANNEL_5  

#endif

/* WY ͨ��*/
#define W_CHAANNEL      PWM_CHANNEL_0
#define Y_CHAANNEL      PWM_CHANNEL_1

	
/*PWMƵ��*/
#define PWM_FREQ                (1000U)


#define HW_PWM_LEVEL_MAX        (48000U)

/*PWM������*/
#if (HW_RGB_ENABLE == 1) 
#define HW_PWM_RGB_LEVEL_MAX    HW_PWM_LEVEL_MAX /*RGB PWM �ȽϼĴ��������ֵ, 100%ռ�ձ�*/
#endif

#define HW_PWM_WY_LEVEL_MAX     HW_PWM_LEVEL_MAX /*WY PWM �ȽϼĴ��������ֵ, 100%ռ�ձ�*/

	
void BSP_PWM_Init(void);
void BSP_PWM_Set_DutyCycle(U8 ucChannel, U8 ucDutyCycle);
void BSP_PWM_Set_CmpData(U8 ucChannel, U32 ulCmpData);
    


#ifdef __cplusplus
}
#endif

#endif

