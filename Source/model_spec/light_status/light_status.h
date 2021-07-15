#ifndef __LIGHT_STATUS_H
#define __LIGHT_STATUS_H

#ifdef __cplusplus
 extern "C" {
#endif


#define LIGHTNESS_MIN	    (1U)
#define LIGHTNESS_MAX	    (65535U)
						    
#define TEMP_MIN	        (2200U) 
#define TEMP_MAX	        (6500U)
#define DELTA_UV_DEF	    (0x0000U)

#define HSL_HUE_MIN	        (0x0000U)
#define HSL_HUE_MAX	        (0xFFFFU) 
#define HSL_SATURATION_MIN	(0x0000U)
#define HSL_SATURATION_MAX	(0xFFFFU) 


#define LIGHTNESS_TO_LEVEL(Lightness)    (Lightness / 655)

typedef enum 
{
	LIGHT_STATUS_TYPE_ONOFF = 0,
	LIGHT_STATUS_TYPE_LIGHT_LEVEL,
	DELTA_LEVEL_LIGHT,
	MOVE_LIGHT,
	LIGHT_STATUS_TYPE_ACTUAL,
	LINEAR,
	LIGHT_STATUS_TYPE_CTL_LIGHT,

	LIGHT_STATUS_TYPE_TEMP_LEVEL,
	MOVE_TEMP,
	LIGHT_STATUS_TYPE_CTL_TEMP,
	LIGHT_STATUS_TYPE_CTL_DELTA_UV,	
	LIGHT_STATUS_TYPE_HSL_LIGHT,
	
	LIGHT_STATUS_TYPE_HUE_LEVEL,
	MOVE_HUE,
	LIGHT_STATUS_TYPE_HSL_HUE,
	
	LIGHT_STATUS_TYPE_SATURATION_LEVEL,
	MOVE_SATURATION,
	LIGHT_STATUS_TYPE_HSL_SATURATION,

    LIGHT_STATUS_TYPE_MAX,

}LIGHT_STATUS_TYPE;


typedef struct  
{
	uint16_t current;
	uint16_t target;

	uint16_t def;
	uint16_t last;

	uint16_t range_min;
	uint16_t range_max;

    int16_t delta;

}LIGHTNESS_STATUS;

typedef struct  
{
	uint16_t current;
	uint16_t target;

	uint16_t def;

	uint16_t range_min;
	uint16_t range_max;
	uint32_t range;

	int16_t delta_current;
	int16_t delta_target;
	int16_t delta_def;
	
}LIGHT_TEMP_STATUS;

typedef struct  
{
	uint16_t current;
	uint16_t target;

	uint16_t def;

	uint16_t range_min;
	uint16_t range_max;

	int16_t delta;
	
}LHSL_HUE_STATUS;

typedef struct  
{
	uint16_t current;
	uint16_t target;

	uint16_t def;

	uint16_t range_min;
	uint16_t range_max;

	int16_t delta;
	
}LHSL_SATURATION_STATUS;


typedef struct  
{
	uint8_t  onpowerup;
	uint8_t  dtt;
	U8 LightMode;   /*������ģʽ RGB or CW*/
    
	LIGHTNESS_STATUS       lightness;
	LIGHT_TEMP_STATUS       temp;
	LHSL_HUE_STATUS        hue;
	LHSL_SATURATION_STATUS saturation;

}LIGHT_STATUS;

extern LIGHT_STATUS LightStatus;

U16 Light_Status_Constrain_Value(U16 Value, U16 Min, U16 Max);
void Light_Status_Default_Init(void);
void Light_Status_Set_Target(LIGHT_STATUS_TYPE Type, void *pTarget);

S32 Light_Status_Get_Current(LIGHT_STATUS_TYPE Type);
S32 Light_Status_Get_Target(LIGHT_STATUS_TYPE Type);	
void Light_Status_Save(void);

#ifdef __cplusplus
}
#endif
  
   
#endif


