#ifndef __FRIMWARE_UPDATE_SERVER__H
#define __FRIMWARE_UPDATE_SERVER__H


#define FW_LIST_SIZE                         2
#define FW_UPDATE_SERVER_MODEL_ID            0xfe00

//Additional infor
#define PROVISIONING_NEEDED                  0x1
#define COMPOSITION_CHANGE_EXPECTED          0x2

/* Forward declaration */
typedef struct __firmware_update_server_t firmware_update_server_t;


typedef void (*firmware_update_info_get_cb_t)(firmware_update_server_t * p_self,
        mesh_model_msg_ind_t const *p_rx_msg,
        const firmware_update_info_get_param_t  * p_in,
        firmware_update_info_status_param_t  * p_out);

typedef void (*firmware_update_fw_meta_check_cb_t)(firmware_update_server_t * p_self,
        mesh_model_msg_ind_t const *p_rx_msg,
        const firmware_update_meta_check_param_t  *p_in,
        firmware_update_meta_status_param_t  *p_out);


typedef void (*firmware_update_get_cb_t)(firmware_update_server_t * p_self,
        mesh_model_msg_ind_t const *p_rx_msg,
        firmware_update_status_param_t  * p_out);


typedef void (*firmware_update_start_cb_t)(firmware_update_server_t * p_self,
        mesh_model_msg_ind_t const *p_rx_msg,
        const firmware_update_start_param_t  *p_in,
        firmware_update_status_param_t  * p_out);


typedef void (*firmware_update_cancel_cb_t)(firmware_update_server_t * p_self,
        mesh_model_msg_ind_t const *p_rx_msg,
        firmware_update_status_param_t  * p_out);


typedef void (*firmware_update_apply_cb_t)(firmware_update_server_t * p_self,
        mesh_model_msg_ind_t const *p_rx_msg,
        firmware_update_status_param_t  * p_out);

/**
 * DFU server callback list
 */
/**
 * Transaction callbacks for the firmware update state
 */
typedef struct
{
    firmware_update_get_cb_t     get_cb;
    firmware_update_start_cb_t   start_cb;
    firmware_update_cancel_cb_t  cancel_cb;
    firmware_update_apply_cb_t   apply_cb;
} firmware_update_state_cbs_t;

/**
 * Transaction callbacks for meta state
 */
typedef struct
{
    firmware_update_fw_meta_check_cb_t check_cb;
} firmware_update_fw_meta_state_cbs_t;


/**
 * Transaction callbacks for the firmware information state
 */
typedef struct
{
    firmware_update_info_get_cb_t get_cb;
} firmware_update_information_state_cbs_t;

typedef struct
{
    firmware_update_state_cbs_t             update_cbs;
    firmware_update_fw_meta_state_cbs_t     meta_cbs;
    firmware_update_information_state_cbs_t info_cbs;
} firmware_update_callbacks_t;

/**
 * User provided settings and callbacks for the model instance
 */
typedef struct
{
    /** Callback list */
    const firmware_update_callbacks_t * p_callbacks;

} firmware_update_server_settings_t;

typedef struct
{
    firmware_id_t current_fw_id;
    // Update URI The string values contained in this state can be of zero length

} firmware_info_list_t;

typedef enum
{
    UPDATAE_PHASE_IDLE              			=0x0,
    UPDATAE_PHASE_TRANSFER_ERROR		        =0x1,
    UPDATAE_PHASE_TRANSFER_ACTIVE  				=0x2,
    UPDATAE_PHASE_VERIFICATION_SUCCESS          =0x3,
    UPDATAE_PHASE_VERIFICATION_FAILED 			=0x4,
    UPDATAE_PHASE_APPLY_ACTIVE			        =0x5,
    UPDATAE_PHASE_TRANSFER_CANCELLED	        =0x6,
    UPDATAE_PHASE_UNKOWN				        =0x7,
} update_phase_t;

typedef struct
{
    /**dfu server states**/
    firmware_info_list_t fw_info_list[1];
    update_phase_t update_phase;	//value seen at update phase 3bit
    uint8_t fw_update_additonal_info;
    firmware_id_t incoming_fw_id;
    //new_firmware
    uint64_t blob_id;
    //firmware Metadata

    //add distribution_TTL
    uint8_t distribution_ttl;
    /**dfu server states**/

} firmware_update_server_state_t;

struct __firmware_update_server_t
{
    /** Model settings and callbacks for this instance */
    firmware_update_server_settings_t settings;

    firmware_update_server_state_t state;

    //need or not?
    //dfu_server_procedure_t proc;
};

/**
 * Initializes firmware update server.
 *
 * @note This function should only be called _once_.
 * @note The server handles the model allocation and adding.
 *
 * @param[in]     p_server                 firmware update server context pointer.
 * @param[in]     element_index            Element index to add the model
 *
 */
void firmware_update_server_init(firmware_update_server_t *p_server);
void firmware_update_server_msg_handler(mesh_model_msg_ind_t const *p_rx_msg);

#endif
