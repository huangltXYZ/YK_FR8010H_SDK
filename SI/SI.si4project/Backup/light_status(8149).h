#ifndef __LIGHT_STATUS_H
#define __LIGHT_STATUS_H

#ifdef __cplusplus
 extern "C" {
#endif


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

void Light_Default_Status_Init(void);

	

#ifdef __cplusplus
}
#endif
  
   
#endif


