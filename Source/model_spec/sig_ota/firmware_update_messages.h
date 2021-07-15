#ifndef FRIEWARE_UPDATE_MESSAGES__H
#define FRIEWARE_UPDATE_MESSAGES__H


typedef struct
{
    uint8_t entry_index;
    uint8_t entris_limit;
} __attribute((packed)) msg_firmware_update_info_get_t;

typedef struct
{
    //firmware_infor_entry
    uint8_t list_count;
    uint8_t entry_index;
    uint8_t current_fw_id_len;
    firmware_id_t current_fw_id;
    uint8_t update_uri_len;
    //uint8_t update_uri[URI_LENGTH];
} __attribute((packed)) msg_firmware_update_info_status_t ;

typedef struct
{
    uint8_t metadata_len;
    // 0...255 metadata
    firmware_metadata_t metadata;
    //firmware_id_t fw_id;
} __attribute((packed)) msg_firmware_update_meta_check_t;

typedef struct
{
    uint8_t status 	:4;
    uint8_t rfu     :4;
    firmware_id_t fw_id;
} __attribute((packed)) msg_firmware_update_meta_status_t;


typedef struct
{
    uint8_t distribution_ttl;//0x00,0x02-0x7f
    uint16_t timeout_base; // unit: 10 second
    uint64_t blob_id;
    uint8_t image_index;  // Index of the firmware image in the Firmware information List state that is being updated
    // 0...255 metadata
    //firmware_metadata_t metadata;
} __attribute((packed)) msg_firmware_update_start_t;

typedef struct
{
    uint8_t status 				:4;//seen at dfu_status
    uint8_t rfu     			:4;
    uint8_t phase 				:3;
    uint8_t additional_info     	:5;
    uint8_t distribution_ttl;//0x00,0x02-0x7f  (Optional)
    uint64_t blob_id;
    firmware_id_t fw_id;
} __attribute((packed)) msg_firmware_update_status_t;


#endif


