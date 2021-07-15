#ifndef FIRMWARE_UPDATE_COMMON__H
#define FIRMWARE_UPDATE_COMMON__H


#pragma anon_unions

typedef enum
{
    FIRMWARE_UPDATE_INFO_OPCODE_GET = 0xDC,
    FIRMWARE_UPDATE_INFO_OPCODE_STATUS = 0xDD,
    FIRMWARE_UPDATE_FW_META_OPCODE_CHECK = 0xDE,
    FIRMWARE_UPDATE_FW_META_OPCODE_STATUS = 0xDF,
    FIRMWARE_UPDATE_OPCODE_GET = 0xE0,
    FIRMWARE_UPDATE_OPCODE_START = 0xE1,
    FIRMWARE_UPDATE_OPCODE_CANCEL = 0xE2,
    FIRMWARE_UPDATE_OPCODE_APPLY = 0xE3,
    FIRMWARE_UPDATE_OPCODE_STATUS = 0xE4,

} firmware_update_opcode_t;


typedef struct
{
    uint8_t entry_index;
    uint8_t entris_limit;
} firmware_update_info_get_param_t;

typedef struct
{
    uint16_t product_id;
    uint16_t version_id;//0...111 Bytes , 4Bytes for example

} __attribute((packed)) firmware_id_t;

typedef struct
{
    uint16_t company_id;
    uint16_t product_id;
    uint32_t version_id;
} __attribute((packed)) firmware_metadata_t;

typedef struct
{
    //firmware_infor_entry
    uint8_t list_count;
    uint8_t entry_index;
//#if 1   // only one firmware
//    uint8_t fw_id_len;
//    fw_id_t fw_id;
//    uint8_t uri_len; // set 0 now.
//    // uri[];
//#endif
    uint8_t current_fw_id_len;
    firmware_id_t current_fw_id;
    uint8_t update_uri_len;
    //uint8_t update_uri[URI_LENGTH];
} firmware_update_info_status_param_t;

typedef struct
{
    uint8_t fw_meta_len;
    firmware_metadata_t metadata;
    // 0...255 metadata
    //firmware_id_t fw_id;
} firmware_update_meta_check_param_t;

typedef struct
{
    uint8_t status 	:4;
    uint8_t rfu     :4;
    firmware_id_t fw_id;
} firmware_update_meta_status_param_t;

typedef struct
{
    uint8_t distribution_ttl;//0x00,0x02-0x7f
    uint16_t timeout_base; // unit: 10 second
    uint64_t blob_id;
    uint8_t metadata_len;
    uint8_t image_index;  // Index of the firmware image in the Firmware information List state that is being updated
    // 0...255 metadata
    firmware_metadata_t fw_metadata;
} firmware_update_start_param_t;

typedef struct
{
    uint8_t status 				:3;//seen at dfu_status
    uint8_t rfu     			:2;
    uint8_t update_phase		:3;
    // --below is optional
    uint8_t distribution_ttl;	//0x00,0x02-0x7f  (Optional)
    uint8_t additional_info  	:5;
    uint8_t rfu2             	:3;
    uint16_t timeout_base;
    uint64_t blob_id;
    uint8_t image_index;
} firmware_update_status_param_t;

#endif



