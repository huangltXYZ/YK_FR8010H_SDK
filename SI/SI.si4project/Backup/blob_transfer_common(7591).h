#ifndef BLOB_TRANSFER_COMMON__H
#define BLOB_TRANSFER_COMMON__H


#define ST_BLOB_SUCCESS                  0x00
#define ST_BLOB_INVALID_BLOCK_NUMBER     0x01
#define ST_BLOB_WRONG_BLOCK_SIZE         0x02
#define ST_BLOB_WRONG_CHUNK_SIZE         0x03
#define ST_BLOB_INVALID_STATE            0x04
#define ST_BLOB_INVALID_PARAM            0x05
#define ST_BLOB_WRONG_BLOB_ID            0x06
#define ST_BLOB_STORANGE_LIMIT           0x07


#define BLOCK_BITS_MASK     20


#define MIN_BLOCK_SIZE_LOG		10
#define MAX_BLOCK_SIZE_LOG      10
#define MAX_CHUNKS_NUMBER     100
#define MAX_CHUNK_SIZE        128
#define MAX_BLOB_SIZE         0x14000
#define MAX_MTU_SIZE          11
#define SIG_OTA_SWITCH_PAGE_ADDR                0x14000

#define CHUNK_CONTI   0
#define CHUNK_FINISHED   1
#define CHUNK_BLOACK_FINISHED   2

typedef enum 
{
	BLOB_TRANSFER_OPCODE_GET =  0xD1,
	BLOB_TRANSFER_OPCODE_START = 0xD2,
	BLOB_TRANSFER_OPCODE_CANCEL = 0xD3,
	BLOB_TRANSFER_OPCODE_STATUS = 0xD4,
	BLOB_BLOCK_OPCODE_GET = 0xD5,
	BLOB_BLOCK_OPCODE_START = 0xD6,
	BLOB_BLOCK_OPCODE_STATUS = 0xD7,
	BLOB_CHUNK_OPCODE_TRANSFER = 0xD8,
	BLOB_INFORMATION_OPCODE_GET = 0xD9,
	BLOB_INFORMATION_OPCODE_STATUS = 0xDA

}blob_opcode_t;


 
typedef struct
{	
	uint8_t status           :4;
	uint8_t rfu              :2;
	uint8_t transfer_mode    :2;
	uint8_t transfer_phase;
	// -- below is optional
	uint64_t blob_id;
	uint32_t blob_size;
	uint8_t blob_block_size_log;
	uint16_t client_mtu_size;
	//uint16_t timeout;
	//one bit for one block, 1 stand for not received
	uint8_t block_not_received[BLOCK_BITS_MASK]; //	u8 bk_not_receive[CEIL_8(MESH_OTA_BLOCK_MAX)];   // variable: bit field
}blob_transfer_status_param_t;


typedef struct
{
	uint8_t status :4;
	uint8_t rfu    :2;
	uint8_t format :2;
	uint16_t block_number;
	uint16_t chunk_size;
	// optional for block get
	uint8_t mis_chunks[1<<MAX_BLOCK_SIZE_LOG/MAX_CHUNK_SIZE/8];
}blob_block_status_param_t;


typedef struct
{
	uint8_t min_block_size_log;
	uint8_t max_block_size_log;
	uint16_t max_chunks_number;
	uint16_t max_chunks_size;
	uint32_t max_blob_size; //// max firmware size
	uint16_t server_mtu_size;
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
}blob_information_status_param_t;


typedef struct
{
	uint8_t rfu              :6;
    uint8_t transfer_mode    :2;
	uint64_t blob_id;
	uint32_t blob_size;
	uint8_t  blob_block_size_log;
	uint16_t mtu_size;
	//uint16_t timeout;
}blob_transfer_start_param_t;


typedef struct
{
	uint64_t blob_id;
}blob_transfer_cancel_param_t;


typedef struct
{
	uint16_t block_number;
	uint16_t chunk_size;
}blob_block_start_param_t;

typedef struct
{
	uint16_t chunk_number;
	uint8_t chunk_size;
	uint8_t chunk_data[MAX_CHUNK_SIZE];
}blob_chunk_transfer_t;

typedef struct
{
	uint16_t block_number;
	uint16_t chunk_number;
	uint32_t block_size_log;
	uint8_t chunk_size;
	uint8_t *chunk_data;
}chunk_info_t;


#endif



