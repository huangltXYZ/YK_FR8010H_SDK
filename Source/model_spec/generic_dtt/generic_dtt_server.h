
#ifndef GENERIC_DTT_SERVER_H__
#define GENERIC_DTT_SERVER_H__


/**
 * @defgroup GENERIC_DTT_SERVER Generic Default Transition Time server model interface
 * @ingroup GENERIC_DTT_MODEL
 * @{
 */

/** Server model ID */
#define GENERIC_DTT_SERVER_MODEL_ID 0x1004

void generic_dtt_server_msg_handler(mesh_model_msg_ind_t const *p_rx_msg);

/**@} end of GENERIC_DTT_SERVER */
#endif /* GENERIC_DTT_SERVER_H__ */
