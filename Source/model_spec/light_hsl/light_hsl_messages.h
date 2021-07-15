#ifndef LIGHT_HSL_MESSAGES_H__
#define LIGHT_HSL_MESSAGES_H__

#include <stdint.h>
#include "light_hsl_common.h"



/** Shortest allowed length for the Set message. */
#define LIGHT_HSL_SET_MINLEN 7      //original value 6
/** Longest allowed length for the Set message. */
#define LIGHT_HSL_SET_MAXLEN 9

/** Shortest allowed length for the Set message. */
#define LIGHT_HSL_HUE_SET_MINLEN 3      //original value 6
/** Longest allowed length for the Set message. */
#define LIGHT_HSL_HUE_SET_MAXLEN 5

/** Shortest allowed length for the Set message. */
#define LIGHT_HSL_SATURATION_SET_MINLEN 3      //original value 6
/** Longest allowed length for the Set message. */
#define LIGHT_HSL_SATURATION_SET_MAXLEN 5

/** Shortest allowed length for the Status message. */
#define LIGHT_HSL_TARGET_STATUS_MINLEN 6
/** Longest allowed length for the Status message. */
#define LIGHT_HSL_TARGET_STATUS_MAXLEN 7


/** Shortest allowed length for the Status message. */
#define LIGHT_HSL_STATUS_MINLEN 6
/** Longest allowed length for the Status message. */
#define LIGHT_HSL_STATUS_MAXLEN 7

#define LIGHT_HSL_DEFAULT_STATUS_LEN 6

#define LIGHT_HSL_RANGE_STATUS_LEN 9


/** Generic On Off model message opcodes. */
typedef enum
{
	LIGHT_HSL_OPCODE_GET = 0x826D,
	LIGHT_HSL_OPCODE_HUE_GET = 0x826E,
	LIGHT_HSL_OPCODE_HUE_SET = 0x826F,
	LIGHT_HSL_OPCODE_HUE_SET_UNACKNOWLEDGED = 0x8270,
	LIGHT_HSL_OPCODE_HUE_STATUS = 0x8271,

	LIGHT_HSL_OPCODE_SATURATION_GET = 0x8272,
	LIGHT_HSL_OPCODE_SATURATION_SET = 0x8273,
	LIGHT_HSL_OPCODE_SATURATION_SET_UNACKNOWLEDGED = 0x8274,
	LIGHT_HSL_OPCODE_SATURATION_STATUS = 0x8275,

	LIGHT_HSL_OPCODE_SET = 0x8276,
	LIGHT_HSL_OPCODE_SET_UNACKNOWLEDGED = 0x8277,
	LIGHT_HSL_OPCODE_STATUS = 0x8278,
	LIGHT_HSL_OPCODE_TARGET_GET = 0x8279,
	LIGHT_HSL_OPCODE_TARGET_STATUS = 0x827A,
	LIGHT_HSL_OPCODE_DEFAULT_GET = 0x827B,
	LIGHT_HSL_OPCODE_DEFAULT_STATUS = 0x827C,
	LIGHT_HSL_OPCODE_RANGE_GET = 0x827D,
	LIGHT_HSL_OPCODE_RANGE_STATUS = 0x827E,

	LIGHT_HSL_OPCODE_DEFAULT_SET = 0x827F,
	LIGHT_HSL_OPCODE_DEFAULT_SET_UNACKNOWLEDGED = 0x8280,
	LIGHT_HSL_OPCODE_RANGE_SET = 0x8281,
	LIGHT_HSL_OPCODE_RANGE_SET_UNACKNOWLEDGED = 0x8282,

} light_hsl_opcode_t;

typedef struct
{
	uint16_t hsl_lightness;
	uint16_t hsl_hue;
	uint16_t hsl_saturation;
	uint8_t tid;
	uint8_t trans_time;
	uint8_t delay;
}__attribute((packed))msg_light_hsl_set_msg_pkt_t;

typedef struct
{
	uint16_t hsl_hue;
	uint8_t tid;
	uint8_t transition_time;
	uint8_t delay;
}__attribute((packed))msg_light_hsl_hue_set_msg_pkt_t;

typedef struct
{
	uint16_t hsl_saturation;
	uint8_t tid;
	uint8_t transition_time;
	uint8_t delay;
}__attribute((packed))msg_light_hsl_saturation_set_msg_pkt_t;


typedef struct
{
	uint16_t hsl_lightness;
	uint16_t hsl_hue;
	uint16_t hsl_saturation;
}__attribute((packed))msg_light_hsl_default_set_msg_pkt_t;


typedef struct
{
	uint16_t hue_range_min;
	uint16_t hue_range_max;
	uint16_t saturation_range_min;
	uint16_t saturation_range_max;
}__attribute((packed))msg_light_hsl_range_set_msg_pkt_t;


typedef struct
{
	uint16_t hsl_lightness;
	uint16_t hsl_hue;
	uint16_t hsl_saturation;
	uint8_t remaining_times;
}__attribute((packed))msg_light_hsl_status_msg_pkt_t;


typedef struct
{
	uint16_t hsl_lightness_target;
	uint16_t hsl_hue_target;
	uint16_t hsl_saturation_target;
	uint8_t remaining_times;
}__attribute((packed))msg_light_hsl_target_status_msg_pkt_t;


typedef struct
{
	uint16_t hsl_lightness;
	uint16_t hsl_hue;
	uint16_t hsl_saturation;
}__attribute((packed))msg_light_hsl_default_status_msg_pkt_t;


typedef struct
{
	int8_t status_code;
	uint16_t hue_range_min;
	uint16_t hue_range_max;
	uint16_t saturation_range_min;
	uint16_t saturation_range_max;
}__attribute((packed))msg_light_hsl_range_status_msg_pkt_t;

typedef struct
{
	uint16_t present_saturation;
	uint16_t target_saturation;
	uint8_t remaining_times;
}__attribute((packed))msg_light_hsl_saturation_status_msg_pkt_t;

typedef struct
{
	uint16_t present_hue;
	uint16_t target_hue;
	uint8_t remaining_times;
}__attribute((packed))msg_light_hsl_hue_status_msg_pkt_t;

#endif /* LIGHT_HSL_MESSAGES_H__ */
