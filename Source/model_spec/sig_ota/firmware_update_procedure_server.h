#ifndef FIRMWARE_UPDATE_PROCEDURE_SERVER__H
#define FIRMWARE_UPDATE_PROCEDURE_SERVER__H



void firmware_update_info_get(firmware_update_server_t * p_self,
                              mesh_model_msg_ind_t const *p_rx_msg,
                              const firmware_update_info_get_param_t * p_in,
                              firmware_update_info_status_param_t * p_out);

void firmware_update_fw_meta_check(firmware_update_server_t * p_self,
                                   mesh_model_msg_ind_t const *p_rx_msg,
                                   const firmware_update_meta_check_param_t * p_in,
                                   firmware_update_meta_status_param_t * p_out);

void firmware_update_get(firmware_update_server_t * p_self,
                         mesh_model_msg_ind_t const *p_rx_msg,
                         firmware_update_status_param_t * p_out);

void firmware_update_start(firmware_update_server_t * p_self,
                           mesh_model_msg_ind_t const *p_rx_msg,
                           const firmware_update_start_param_t *p_in,
                           firmware_update_status_param_t * p_out);

void firmware_update_cancel(firmware_update_server_t * p_self,
                            mesh_model_msg_ind_t const *p_rx_msg,
                            firmware_update_status_param_t * p_out);

void firmware_update_apply(firmware_update_server_t * p_self,
                           mesh_model_msg_ind_t const *p_rx_msg,
                           firmware_update_status_param_t * p_out);


void firmware_update_init(firmware_update_server_t * p_server);


#endif
