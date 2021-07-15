#ifndef BLOB_TRANSFER_MESSAGES__H
#define BLOB_TRANSFER_MESSAGES__H



#define BLOB_TRANSFER_STRAT_LEN                    17

#define BLOB_TRANSFER_STATUS_MIN_LEN               2
#define BLOB_BLOCK_STRAT_LEN                       4
#define BLOB_TRANSFER_CANCEL_LEN                   8



typedef struct
{
	uint8_t status           :4;
	uint8_t rfu              :2;
	uint8_t transfer_mode    :2;
	uint8_t transfer_phase;
	uint64_t blob_id;
	uint32_t blob_size;
	uint8_t blob_block_size_log;
	uint16_t client_mtu_size;
	//uint16_t timeout;
	uint8_t block_not_received[BLOCK_BITS_MASK]; //	u8 bk_not_receive[CEIL_8(MESH_OTA_BLOCK_MAX)];   // variable: bit field
}__attribute((packed)) msg_blob_transfer_status_t;


typedef struct
{
	uint8_t status :4;
	uint8_t rfu    :2;
	uint8_t format :2;
	uint16_t block_number;
	uint16_t chunk_size;
	// optional for block get
	uint8_t mis_chunks[1<<MAX_BLOCK_SIZE_LOG/MAX_CHUNK_SIZE/8];
}__attribute((packed)) msg_blob_block_status_t;


typedef struct
{
	uint8_t min_block_size_log;
	uint8_t max_block_size_log;
	uint16_t max_chunks_number;
	uint16_t max_chunks_size;
	uint32_t max_blob_size;
	uint16_t mtu_size;
	#if 1
    uint8_t transfer_mode;
	#else
	union{
	    uint8_t transfer_mode;
	    struct{
        	uint8_t transfer_mode_push   :1;
        	uint8_t transfer_mode_pull   :1;
        	uint8_t transfer_mode_rsv    :6;
    	};
	};
	#endif
}__attribute((packed)) msg_blob_information_status_t;


typedef struct
{
	uint8_t rfu              :6;
    uint8_t transfer_mode    :2;
	uint64_t blob_id;
	uint32_t blob_size;
	uint8_t  blob_block_size_log;
	uint16_t mtu_size;
	//uint16_t timeout;
}__attribute((packed))msg_blob_transfer_start_t;


typedef struct
{
	uint64_t blob_id;
}__attribute((packed))msg_blob_transfer_cancel_t;


typedef struct
{
	uint16_t block_number;
	uint16_t chunk_size;
}__attribute((packed))msg_blob_block_start_t;


typedef struct
{
	uint16_t chunk_number;
	uint8_t chunk_data[MAX_CHUNK_SIZE];
}__attribute((packed))msg_blob_chunk_transfer_t;


#endif


