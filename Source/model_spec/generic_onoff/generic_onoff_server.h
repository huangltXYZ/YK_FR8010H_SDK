
#ifndef GENERIC_ONOFF_SERVER_H__
#define GENERIC_ONOFF_SERVER_H__

//#include <stdint.h>

//#include "generic_onoff_common.h"

/**
 * @defgroup GENERIC_ONOFF_SERVER Generic OnOff server model interface
 * @ingroup GENERIC_ONOFF_MODEL
 * @{
 */

/** Server model ID */
#define GENERIC_ONOFF_SERVER_MODEL_ID 0x1000




void generic_onoff_server_msg_handler(mesh_model_msg_ind_t const *p_rx_msg);

/**@} end of GENERIC_ONOFF_SERVER */
#endif /* GENERIC_ONOFF_SERVER_H__ */
