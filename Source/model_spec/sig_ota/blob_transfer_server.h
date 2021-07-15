#ifndef BLOB_TRANSFER_SERVER_H__
#define BLOB_TRANSFER_SERVER_H__


#define BLOB_SERVER_MODEL_ID            0xff00

/* Forward declaration */
typedef struct __blob_transfer_server_t blob_transfer_server_t;


typedef void (*blob_transfer_get_cb_t)(blob_transfer_server_t * p_self,
                                       mesh_model_msg_ind_t const *p_rx_msg,
                                       blob_transfer_status_param_t * p_out);

typedef void (*blob_transfer_start_cb_t)(blob_transfer_server_t * p_self,
        mesh_model_msg_ind_t const *p_rx_msg,
        const blob_transfer_start_param_t * p_in,
        blob_transfer_status_param_t * p_out);


typedef void (*blob_transfer_cancel_cb_t)(blob_transfer_server_t * p_self,
        mesh_model_msg_ind_t const *p_rx_msg,
        const blob_transfer_cancel_param_t * p_in,
        blob_transfer_status_param_t * p_out);


typedef void (*blob_block_get_cb_t)(blob_transfer_server_t * p_self,
                                    mesh_model_msg_ind_t const *p_rx_msg,
                                    blob_block_status_param_t * p_out);


typedef void (*blob_block_start_cb_t)(blob_transfer_server_t * p_self,
                                      mesh_model_msg_ind_t const *p_rx_msg,
                                      const blob_block_start_param_t * p_in,
                                      blob_block_status_param_t * p_out);


typedef void (*blob_chunk_transfer_cb_t)(blob_transfer_server_t * p_self,
        mesh_model_msg_ind_t const *p_rx_msg,
        const blob_chunk_transfer_t * p_in
                                        );


typedef void (*blob_information_get_cb_t)(blob_transfer_server_t * p_self,
        mesh_model_msg_ind_t const *p_rx_msg,
        blob_information_status_param_t * p_out);



/**
 * Transaction callbacks for the BLOB transfer state
 */
typedef struct
{
    blob_transfer_get_cb_t  get_cb;
    blob_transfer_start_cb_t start_cb;
    blob_transfer_cancel_cb_t  cancel_cb;
} blob_transfer_state_cbs_t;

/**
 * Transaction callbacks for the block state
 */
typedef struct
{
    blob_block_get_cb_t  get_cb;
    blob_block_start_cb_t start_cb;
} blob_block_state_cbs_t;


/**
 * Transaction callbacks for the BLOB chunk state
 */
typedef struct
{
    blob_chunk_transfer_cb_t trs_cb;
} blob_chunk_state_cbs_t;


/**
 * Transaction callbacks for the BLOB information state
 */
typedef struct
{
    blob_information_get_cb_t get_cb;
} blob_information_state_cbs_t;


/**
 * BLOB server callback list
 */
typedef struct
{
    blob_transfer_state_cbs_t transfer_cbs;
    blob_block_state_cbs_t    block_cbs;
    blob_chunk_state_cbs_t    chunk_cbs;
    blob_information_state_cbs_t info_cbs;
} blob_server_callbacks_t;

/**
 * User provided settings and callbacks for the model instance
 */
typedef struct
{

    /** Callback list */
    const blob_server_callbacks_t * p_callbacks;

} blob_server_settings_t;



typedef blob_information_status_param_t blob_server_cap_state_t;

typedef struct
{
    uint16_t tr_max_chunks_number;
    uint16_t tr_max_chunks_size;
    uint8_t  tr_max_total_blocks;
    uint8_t  tr_max_block_size_log;
    uint32_t tr_max_blob_size;
    uint16_t tr_mtu_size;
} transfer_cap_state_t;


typedef struct
{
    uint16_t block_number;
    uint16_t block_size;
    uint16_t chunk_size;
    uint8_t *blob_data;
} blob_block_t;

typedef struct
{
    uint64_t blob_id;
    uint32_t blob_size;
    uint8_t  block_size_log;
    uint8_t  *blob_data;
    uint16_t total_blocks;
} blob_state_t;


typedef enum
{
    INACTIVE = 0x00,
    IDLE,
    WAITING_N_BLOCK,
    WAITING_N_CHUNK,
    COMPLETE,
    SUSPNEDED,
    UNKNOWN
} BLOB_TRANSFER_PHASE_STATE_T;

typedef enum
{
    ALL_CHUNKS_MISSING 	= 0,
    NO_MISSING_CHUNKS,
    SOME_CHUNKS_MISSING,
    ENCODE_MISS_CHUNK,
} MISSING_CHUNKS_REP;


typedef struct
{
    /**blob transfer common states*/
    blob_state_t blob_state;
    blob_block_t blob_block_state;
    /**blob transfer common states*/

    /**blob transfer server states**/
    BLOB_TRANSFER_PHASE_STATE_T phase_state;
    uint64_t exp_blob_id;
    uint16_t received_blocks;
    uint8_t received_chunks[1<<MAX_BLOCK_SIZE_LOG/MAX_CHUNK_SIZE/8];
    uint16_t client_mtu_size;
    uint16_t timeout;
    uint16_t current_chunks;
    blob_server_cap_state_t cap_states;
    transfer_cap_state_t  trs_cap_states;
    /**blob transfer server states**/

} blob_server_procedure_t;


/**  */
struct __blob_transfer_server_t
{
    /** Model settings and callbacks for this instance */
    blob_server_settings_t settings;

    blob_server_procedure_t proc;
};

/**
 * Initializes blob transfer server.
 *
 * @note This function should only be called _once_.
 * @note The server handles the model allocation and adding.
 *
 * @param[in]     p_server                 blob transfer server context pointer.
 * @param[in]     element_index            Element index to add the model
 *
 */
void blob_transfer_server_init(blob_transfer_server_t * p_server);
void blob_transfer_server_msg_handler(mesh_model_msg_ind_t const *p_rx_msg);



extern blob_transfer_server_t* mp_blob_transfer_server;

#endif






