#ifndef __APP_LIGHT_H
#define __APP_LIGHT_H
#ifdef __cplusplus
 extern "C" {
#endif

#define APP_Light_TRANSITION_TIME    (400U)/*400ms的渐变时间*/
#define APP_LIGHT_TIMER_INTVERAL  	 (10U) /*渐变定时器间隔10ms*/
	 

#define YK_CW_DIM_LEVEL_MIN_PERCENT (2U) /*白光模式下的最低占空比(CW路的总和)*/
#define YK_CW_DIM_LEVEL_MAX_PERCENT (100U) /*白光模式下的最低占空比(CW路的总和)*/

#define YK_CW_DIM_LEVEL_MIN  (HW_PWM_WY_LEVEL_MAX * YK_CW_DIM_LEVEL_MIN_PERCENT / 100)
#define YK_CW_DIM_LEVEL_MAX  (HW_PWM_WY_LEVEL_MAX * YK_CW_DIM_LEVEL_MAX_PERCENT / 100)

/*Light控制结构体*/
typedef struct
{	
	U16 start_r;
	U16 start_g;
	U16 start_b;
	U16 start_c;
	U16 start_w;
	
	U16 current_r;
	U16 current_g;
	U16 current_b;
	U16 current_c;
	U16 current_w;
	
	U16 target_r;
	U16 target_g;
	U16 target_b;
	U16 target_c;
	U16 target_w;
	
	U16 transtime;//转换时间 unit:1ms
	U16 transtime_elapsed;
	
	U8 transitioning;//转换状态
	
	U8 onoff;
	U8 r;
	U8 g;
	U8 b;
	U16 temperature;
	U8 lightness;
	U8 mode;

}LIGHT;

typedef enum
{
	OFF = 0x00,
	ON = 0x01,
}ONOFF_TypeDef;

typedef enum
{
	RGB_MODE = 0x00,
	CW_MODE,
	
}LIGHT_MODE_TypeDef;

void APP_Light_Init(void);
void APP_Light_SetLevel(U16 TargetLevel, U16 usTransitionTime);
U8 APP_Light_GetLevel(void);
void APP_Light_SetTemperature(U16 TargetTemperature, S16 DeltaUV, U16 usTransitionTime);
U16 APP_Light_GetTemperature(void);
void APP_Light_SetOnOff(U8 onoff, U16 usTransitionTime);
U8 APP_Light_GetOnoff(void);
#if (HW_RGB_ENABLE == 1)
void APP_Light_SetRGB(U16 Target_R, U16 Target_G, U16 Target_B, U16 Level, U16 usTransitionTime);
void APP_Light_SetHSL(U16 Target_H, U16 Target_S, U16 Target_L, U16 usTransitionTime);
#endif 	
void light_start_transition(U16 transtime);
void app_light_timer_cb(void *arg);

#ifdef __cplusplus
}
#endif
  
   
#endif

