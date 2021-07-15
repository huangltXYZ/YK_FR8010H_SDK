#ifndef __APP_MESH_H
#define __APP_MESH_H




#ifdef __cplusplus
 extern "C" {
#endif

void APP_MESH_Init(void);
void APP_MESH_Provision_Complete(void);
void APP_MESH_Status_Send(mesh_model_msg_ind_t const *ind, uint32_t opcode, uint8_t *msg, uint16_t msg_len);


#ifdef __cplusplus
}
#endif
  
   
#endif


