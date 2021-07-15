
#ifndef LIGHT_LIGHTNESS_SERVER_H__
#define LIGHT_LIGHTNESS_SERVER_H__


/** Server model ID */
#define LIGHT_LIGHTNESS_SERVER_MODEL_ID 0x1300

/** Setup server model ID */
#define LIGHT_LIGHTNESS_SETUP_SERVER_MODEL_ID 0x1301


void lightness_server_msg_handler(mesh_model_msg_ind_t const *p_rx_msg);

/**@} end of LIGHT_LIGHTNESS_SETUP_SERVER */
#endif /* LIGHT_LIGHTNESS_SERVER_H__ */
