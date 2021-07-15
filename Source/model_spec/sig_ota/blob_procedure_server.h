#ifndef BLOB_PROCEDURE_SERVER__H
#define BLOB_PROCEDURE_SERVER__H


void blob_transfer_get(blob_transfer_server_t * p_server,
                       mesh_model_msg_ind_t const *p_rx_msg,
                       blob_transfer_status_param_t * p_out);

void blob_transfer_start(blob_transfer_server_t * p_server,
                         mesh_model_msg_ind_t const *p_rx_msg,
                         const blob_transfer_start_param_t * p_in,
                         blob_transfer_status_param_t * p_out);

void blob_transfer_cancel(blob_transfer_server_t * p_server,
                          mesh_model_msg_ind_t const *p_rx_msg,
                          const blob_transfer_cancel_param_t * p_in,
                          blob_transfer_status_param_t * p_out);


void blob_block_get(blob_transfer_server_t * p_server,
					mesh_model_msg_ind_t const *p_rx_msg,
					blob_block_status_param_t * p_out);


void blob_block_start(blob_transfer_server_t * p_server,
                      mesh_model_msg_ind_t const *p_rx_msg,
                      const blob_block_start_param_t * p_in,
                      blob_block_status_param_t * p_out);


void blob_chunk_transfer(blob_transfer_server_t * p_server,
                         mesh_model_msg_ind_t const *p_rx_msg,
  	                     const blob_chunk_transfer_t * p_in);

						 
void blob_information_get(blob_transfer_server_t * p_server,
                          mesh_model_msg_ind_t const *p_rx_msg,
                          blob_information_status_param_t * p_out);


						  
void receive_blob_procedure_start(blob_transfer_server_t *p_server,uint64_t blob_id);
						  
#endif





