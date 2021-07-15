
#ifndef LIGHT_CTL_SETUP_SERVER_H__
#define LIGHT_CTL_SETUP_SERVER_H__



/**
 * @defgroup LIGHT_CTL_SETUP_SERVER Light CTL Setup Server model interface
 * @ingroup LIGHT_CTL_MODELS
 *
 * This model is defined as a top level model in the SDK to enable implementation of the tunable
 * white light.
 *
 * This model implements Light CTL Server, Light CTL Temperature Server, and Generic Level Server
 * models. This model requires an instance of Light Lightness Setup Server to be initialized
 * seperately and provided as an input during initialization.
 *
 * @{
 */

/** Server model ID */
#define LIGHT_CTL_SERVER_MODEL_ID (0x1303)

/** Setup server model ID */
#define LIGHT_CTL_SETUP_SERVER_MODEL_ID (0x1304)

/** Temperature server model ID */
#define LIGHT_CTL_TEMPERATURE_SERVER_MODEL_ID (0x1306)


void light_ctl_server_msg_handler(mesh_model_msg_ind_t const *p_rx_msg);


/**@} end of LIGHT_CTL_SETUP_SERVER  */
#endif /* LIGHT_CTL_SETUP_SERVER_H__ */
