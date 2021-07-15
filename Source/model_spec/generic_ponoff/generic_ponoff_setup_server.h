
#ifndef GENERIC_PONOFF_SETUP_SERVER_H__
#define GENERIC_PONOFF_SETUP_SERVER_H__



/** Server model ID */
#define GENERIC_PONOFF_SERVER_MODEL_ID 0x1006

/** Setup server model ID */
#define GENERIC_PONOFF_SETUP_SERVER_MODEL_ID 0x1007


void generic_ponoff_server_msg_handler(mesh_model_msg_ind_t const *p_rx_msg);

/**@} end of GENERIC_PONOFF_SETUP_SERVER */
#endif /* GENERIC_PONOFF_SETUP_SERVER_H__ */
