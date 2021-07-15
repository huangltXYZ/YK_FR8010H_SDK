#ifndef __LIGHT_STATUS_H
#define __LIGHT_STATUS_H

#ifdef __cplusplus
 extern "C" {
#endif


#define LIGHTNESS_MIN	    (655U)
#define LIGHTNESS_MAX	    (65535U)
						    
#define TEMP_MIN	        (YK_TEMPERATURE_MIN) 
#define TEMP_MAX	        (YK_TEMPERATURE_MAX)
#define DELTA_UV_DEF	    (0x0000U)

#define HSL_HUE_MIN	        (0x0000U)
#define HSL_HUE_MAX	        (0xFFFFU) 
#define HSL_SATURATION_MIN	(0x0000U)
#define HSL_SATURATION_MAX	(0xFFFFU) 



typedef enum 
{
	LIGHT_STATUS_TYPE_ONOFF = 0,
	LEVEL_LIGHT,
	DELTA_LEVEL_LIGHT,
	MOVE_LIGHT,
	ACTUAL,
	LINEAR,
	CTL_LIGHT,

	LEVEL_TEMP,
	MOVE_TEMP,
	CTL_TEMP,

	CTL_DELTA_UV,	
	HSL_LIGHT,
	
	LEVEL_HUE,
	MOVE_HUE,
	HSL_HUE,
	
	LEVEL_SATURATION,
	MOVE_SATURATION,
	HSL_SATURATION,

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
	
	LIGHTNESS_STATUS       lightness;
	LIGHT_TEMP_STATUS       temp;
	LHSL_HUE_STATUS        hue;
	LHSL_SATURATION_STATUS saturation;

}LIGHT_STATUS;

extern LIGHT_STATUS LightStatus;

void Light_Status_Default_Init(void);
void Light_Status_Set_Target(LIGHT_STATUS_TYPE Type, void *pTarget);

S32 Light_Status_Get_Current(LIGHT_STATUS_TYPE Type);
S32 Light_Status_Get_Target(LIGHT_STATUS_TYPE Type);	

#ifdef __cplusplus
}
#endif
  
   
#endif


