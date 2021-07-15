#ifndef LIGHT_HSL_COMMON__H
#define LIGHT_HSL_COMMON__H

#include <stdint.h>
#include "model_common.h"


/** Model Company ID */
#define LIGHT_HSL_COMPANY_ID 0xFFFF

typedef enum
{
    /** The provided range set values are valid */
    LIGHT_HSL_RANGE_STATUS_GOOD,
    /** The provided MINIMUM range set value is invalid */
    LIGHT_HSL_RANGE_STATUS_CANNOT_SET_MIN_RANGE,
    /** The provided MAXIMUM range set value is invalid */
    LIGHT_HSL_RANGE_STATUS_CANNOT_SET_MAX_RANGE
} light_hsl_range_status_t;


typedef struct
{
	uint16_t hsl_lightness;
	uint16_t hsl_hue;
	uint16_t hsl_saturation;
	uint8_t tid;
}msg_light_hsl_set_params_t;


typedef struct
{
	uint16_t hsl_hue;
	uint8_t tid;
}msg_light_hsl_hue_set_params_t;

typedef struct
{
	uint16_t hsl_saturation;
	uint8_t tid;
}msg_light_hsl_saturation_set_params_t;

typedef struct
{
	uint16_t hsl_lightness;
	uint16_t hsl_hue;
	uint16_t hsl_saturation;

}msg_light_hsl_default_set_params_t;

typedef struct
{
	uint16_t hue_range_min;
	uint16_t hue_range_max;
	uint16_t saturation_range_min;
	uint16_t saturation_range_max;
}msg_light_hsl_range_set_params_t;


typedef struct
{
	uint16_t present_hsl_lightness;
	uint16_t present_hsl_hue;
	uint16_t present_hsl_saturation;
	uint16_t target_hsl_lightness;
	uint16_t target_hsl_hue;
	uint16_t target_hsl_saturation;
	uint8_t remaining_times;
}msg_light_hsl_status_params_t;


typedef struct
{
	uint16_t hsl_lightness_target;
	uint16_t hsl_hue_target;
	uint16_t hsl_saturation_target;
	uint8_t remaining_times;
}msg_light_hsl_target_status_params_t;

typedef struct
{
	uint16_t hsl_lightness;
	uint16_t hsl_hue;
	uint16_t hsl_saturation;
}msg_light_hsl_default_status_params_t;

typedef struct
{
	int8_t status_code;
	uint16_t hue_range_min;
	uint16_t hue_range_max;
	uint16_t saturation_range_min;
	uint16_t saturation_range_max;
}msg_light_hsl_range_status_params_t;

typedef struct
{
	uint16_t present_saturation;
	uint16_t target_saturation;
	uint8_t remaining_times;
}msg_light_hsl_saturation_status_params_t;

typedef struct
{
	uint16_t present_hue;
	uint16_t target_hue;
	uint8_t remaining_times;
}msg_light_hsl_hue_status_params_t;


#endif /* LIGHT_HSL_COMMON__H */


