

#ifndef GENERIC_LEVEL_SERVER_H__
#define GENERIC_LEVEL_SERVER_H__


/**
 * @defgroup GENERIC_LEVEL_SERVER Generic Level server model interface
 * @ingroup GENERIC_LEVEL_MODEL
 * @{
 */

/** Server model ID */
#define GENERIC_LEVEL_SERVER_MODEL_ID 0x1002


void generic_level_server_msg_handler(mesh_model_msg_ind_t const *p_rx_msg);



/**@} end of GENERIC_LEVEL_SERVER */
#endif /* GENERIC_LEVEL_SERVER_H__ */
