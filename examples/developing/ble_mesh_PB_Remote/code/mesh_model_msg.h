/**
 * Copyright (c) 2019, Freqchip
 * 
 * All rights reserved.
 * 
 * 
 */

#ifndef _MESH_MODEL_MSG_H
#define _MESH_MODEL_MSG_H

/*
 * INCLUDES
 */

/*
 * CONSTANTS
 */
#define GENERIC_ONOFF_SERVER_MODEL_ID        0x1000
#define GENERIC_LEVEL_SERVER_MODEL_ID        0x1002
#define GENERIC_DTT_SERVER_MODEL_ID          0x1004
#define GENERIC_PONOFF_SERVER_MODEL_ID       0x1006
#define GENERIC_PONOFF_SETUP_SERVER_MODEL_ID 0x1007

#define LIGHT_LIGHTNESS_SERVER_MODEL_ID       0x1300
#define LIGHT_LIGHTNESS_SETUP_SERVER_MODEL_ID 0x1301

#define LIGHT_CTL_SERVER_MODEL_ID             (0x1303)
#define LIGHT_CTL_SETUP_SERVER_MODEL_ID       (0x1304)
#define LIGHT_CTL_TEMPERATURE_SERVER_MODEL_ID (0x1306)
#define LIGHT_HSL_SERVER_MODEL_ID             (0x1307)
#define LIGHT_HSL_SETUP_SERVER_MODEL_ID       (0x1308)
#define LIGHT_HSL_HUE_SERVER_MODEL_ID         (0x130A)
#define LIGHT_HSL_SATURATION_SERVER_MODEL_ID  (0x130B)

#define BLOB_SERVER_VENDOR_MODEL_ID         0xFF000171
#define FW_UPDATE_SERVER_VENDOR_MODEL_ID    0xFE000171




#define MESH_MODEL_ID_VENDOR_ALI            0x01A80000

#define MESH_GROUP_ADDR_LED                0xc000
#define MESH_GROUP_ADDR_FUN                0xc007
#define MESH_SECOND_GROUP_ADDR             0xcfff

#define MESH_PUBLISH_ADDR               0xf000



#define MESH_LIGHTNESS_GET                  0x00004b82
#define MESH_LIGHTNESS_SET                  0x00004c82
#define MESH_LIGHTNESS_SET_UNACK            0x00004d82
#define MESH_LIGHTNESS_STATUS               0x00004e82
#define MESH_LIGHTNESS_LINEAR_GET           0x00004f82
#define MESH_LIGHTNESS_LINEAR_SET           0x00005082
#define MESH_LIGHTNESS_LINEAR_SET_UNACK     0x00005182
#define MESH_LIGHTNESS_LINEAR_STATUS        0x00005282
#define MESH_LIGHTNESS_LAST_GET             0x00005382
#define MESH_LIGHTNESS_LAST_STATUS          0x00005482
#define MESH_LIGHTNESS_DEFAULT_GET          0x00005582
//#define MESH_LIGHTNESS_DEFAULT_SET          0x00005682
//#define MESH_LIGHTNESS_DEFAULT_SET_UNACK    0x00005782
#define MESH_LIGHTNESS_DEFAULT_STATUS       0x00005682
#define MESH_LIGHTNESS_RANGE_GET            0x00005782
//#define MESH_LIGHTNESS_RANGE_SET            0x00005a82
//#define MESH_LIGHTNESS_RANGE_SET_UNACK      0x00005b82
#define MESH_LIGHTNESS_RANGE_STATUS         0x00005882

#define MESH_HSL_GET                        0x00006d82
#define MESH_HSL_SET                        0x00007682
#define MESH_HSL_SET_UNACK                  0x00007782
#define MESH_HSL_STATUS                     0x00007882
#define MESH_HSL_RANGE_GET                  0x00007d82
#define MESH_HSL_RANGE_STATUS               0x00007e82


#define MESH_LIGHT_CTL_SET                  0x00005e82
#define MESH_LIGHT_CTL_SET_UNACK            0x00005f82
#define MESH_LIGHT_CTL_STATUS               0x00006082
#define MESH_TEMPERATURE_GET                0x00006182
#define MESH_TEMPERATURE_RANGE_GET          0x00006282
#define MESH_TEMPERATURE_RANGE_STATUS       0x00006382
#define MESH_TEMPERATURE_SET                0x00006482
#define MESH_TEMPERATURE_SET_UNACK          0x00006582
#define MESH_TEMPERATURE_STATUS             0x00006682

#define MESH_VENDOR_OLD_SET                 0x0001A8c1
#define MESH_VENDOR_OLD_SET_UNACK           0x0001A8c2
#define MESH_VENDOR_OLD_STATUS              0x0001A8c3

#define MESH_VENDOR_SET                     0x0001A8d1
#define MESH_VENDOR_SET_UNACK               0x0001A8d2
#define MESH_VENDOR_STATUS                  0x0001A8d3
#define MESH_VENDOR_INDICATION              0x0001A8d4
#define MESH_VENDOR_CONFIRMATION            0x0001A8d5

#define MESH_EVENT_UPDATA_ID                0xF009
#define MESH_EVENT_DEV_RST                  0x23

#define MESH_LIGHTNESS_STATUS_MIN_SIZE      2
#define MESH_LIGHTNESS_STATUS_FULL_SIZE     5

/*
 * TYPEDEFS
 */
 
 
 
typedef struct mesh_led_state_t
{
    uint8_t on_off_state;
    uint16_t lightness;
    uint16_t hsl_lightness;
    uint16_t hsl_hue;
    uint16_t hsl_saturation;
    uint16_t ctl_lightness;
    uint16_t ctl_temperature;
    uint16_t delta_UV;
	
}MESH_LIGHT_STATE;









/** @defgroup BLE_MESH_MESSAGE_DEFINES for application layer use
 * @{
 */


// Light Lightness Set
struct mesh_lightness_model_set_t
{
    uint16_t level;
    uint8_t tid;
    uint8_t ttl;
    uint8_t delay;
} __attribute__((packed));

// Light Lightness Status
struct mesh_lightness_model_status_t
{
    uint16_t current_level;
    uint16_t target_level;
    uint8_t remain;
} __attribute__((packed)) ;

// Light Lightness Status
struct mesh_lightness_model_linear_t
{
    uint16_t lightness;
    uint8_t tid;
    uint8_t trans_time;
    uint8_t delay;
} __attribute__((packed)) ;

// Light Lightness Status
struct mesh_lightness_model_linear_status_t
{
    uint16_t current_lightness;
    uint16_t target_lightness;
    uint8_t remain;
} __attribute__((packed)) ;

struct mesh_lightness_model_range_t {
    uint16_t range_min;
    uint16_t range_max;
} __attribute__((packed)) ;

struct mesh_lightness_model_range_status_t {
    uint8_t status;
    uint16_t range_min;
    uint16_t range_max;
} __attribute__((packed)) ;

// Light HSL Set
struct mesh_hsl_model_set_t
{
    uint16_t lightness;
    uint16_t hue;
    uint16_t hsl_saturation;
    uint8_t tid;
    uint8_t transition_time;
    uint8_t delay;
} __attribute__((packed)) ;

// Light HSL Status
struct mesh_hsl_model_status_t
{
    uint16_t hsl_lightness;
    uint16_t hsl_hue;
    uint16_t hsl_saturation;
    uint8_t remain;
} __attribute__((packed));

// Light HSL Range
struct mesh_hsl_model_range_t
{
    uint8_t status_code;
    uint16_t hsl_range_min;
    uint16_t hsl_range_max;
    uint16_t hsl_saturation_range_min;
    uint16_t hsl_saturation_range_max;
} __attribute__((packed));

// Light CTL Set
struct mesh_CTL_model_set_t
{
    uint16_t lightness;
    uint16_t temperature;
    uint16_t delta_UV;
    uint8_t tid;
    uint8_t trans_time;
    uint8_t delay;
};

// Light ctl Temperature Set
struct mesh_ctl_temperature_set_t
{
    uint16_t ctl_temperature;
    uint16_t ctl_delta_UV;
    uint8_t tid;
} __attribute__((packed)) ;

// Light CTL Status
struct mesh_CTL_model_status_t
{
    uint16_t current_lightness;
    uint16_t current_temperature;
    uint16_t target_lightness;
    uint16_t target_temperature;
    uint8_t remain;
} __attribute__((packed)) ;

// Light CTL temprature Status
struct mesh_CTL_temp_status_t
{
    uint16_t current_temperature;
    uint16_t current_delta_UV;
    uint16_t target_temperature;
    uint16_t target_delta_UV;
    uint8_t remain;
} __attribute__((packed)) ;

// Light CTL Range
struct mesh_CTL_model_range_t
{
    uint8_t status_code;
    uint16_t temperature_range_min;
    uint16_t temperature_range_max;
} __attribute__((packed)) ;

// Vendor Model Set
struct mesh_vendor_model_set_t
{
    uint8_t tid;
    uint8_t num_of_value;
    uint8_t attr_parameter[];
} __attribute__((packed)) ;

// Vendor Model Status
struct mesh_vendor_model_status_t
{
    uint8_t tid;
    uint8_t num_of_value;
    uint8_t attr_parameter[];
} __attribute__((packed)) ;

// Vendor Model Set new
struct mesh_vendor_model_set_new_t
{
    uint8_t tid;
    uint16_t attr_type;
    uint8_t attr_parameter[];
} __attribute__((packed)) ;

// Vendor Model Status new
struct mesh_vendor_model_status_new_t
{
    uint8_t tid;
    uint16_t attr_type;
    uint8_t attr_parameter[];
} __attribute__((packed)) ;

// Vendor Model Indication
struct mesh_vendor_model_indication_t
{
    uint8_t tid;
    uint16_t attr_type;
    uint8_t attr_parameter[];
} __attribute__((packed)) ;



/*************Generic Default Transition Time messages**************/
//typedef enum
//{
//    GENERIC_DTT_OPCODE_GET = 0x820D,
//    GENERIC_DTT_OPCODE_SET = 0x820E,
//    GENERIC_DTT_OPCODE_SET_UNACKNOWLEDGED = 0x820F,
//    GENERIC_DTT_OPCODE_STATUS = 0x8210
//} generic_dtt_opcode_t;

///** Message format for the Default Transition Time Set message. */
//typedef struct __attribute((packed))
//{
//    uint8_t transition_time;                                /**< Encoded transition time value */
//} generic_dtt_set_msg_pkt_t;

///** Message format for the Default Transition Time Status message. */
//typedef struct __attribute((packed))
//{
//    uint8_t transition_time;                                /**< Encoded transition time value */
//} generic_dtt_status_msg_pkt_t;



extern MESH_LIGHT_STATE mesh_led_state;

void mesh_led_on_off_msg_handler(mesh_model_msg_ind_t const *ind);
void mesh_led_dtt_msg_handler(mesh_model_msg_ind_t const *ind);

void mesh_led_ponoff_msg_handler(mesh_model_msg_ind_t const *ind);
void mesh_led_level_msg_handler(mesh_model_msg_ind_t const *ind);

void mesh_lighness_msg_handler(mesh_model_msg_ind_t const *ind);

void mesh_RGB_msg_handler(mesh_model_msg_ind_t const *ind);

void mesh_CTL_msg_handler(mesh_model_msg_ind_t const *ind);

void mesh_dev_led_ctrl_init(void);

#endif  // _MESH_MODEL_MSG_H

