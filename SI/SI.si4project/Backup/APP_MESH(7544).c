#include "YK_APP.h"

#define APP_MESH_PRINTF(...)    printf(__VA_ARGS__)

blob_transfer_server_t m_blob_transfer_server;

firmware_update_server_t m_dfu_server;

/*****************************************************************************
Function name :  MESH_SIG_OTA_Init
Description:	 MESH SIG OTA 相关初始化
Input:           void
Return: 	     void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   ########  build this module
******************************************************************************/
static void MESH_SIG_OTA_Init(void)
{
    blob_transfer_server_init(&m_blob_transfer_server);
    firmware_update_server_init(&m_dfu_server);
}

/*****************************************************************************
Function name :  APP_MESH_Init
Description:	 MESH相关初始化
Input:           void
Return: 	     void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   ########  build this module
******************************************************************************/
void APP_MESH_Init(void)
{
	Light_Status_Default_Init();
	
	app_mesh_led_init();
	pb_remote_central_init();
	MESH_SIG_OTA_Init();
	
	TC_OTA_gatt_add_service();
}


/*****************************************************************************
Function name :  APP_MESH_Provision_Complete
Description:	 MESH PROV完成回调
Input:           void
Return: 	     void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   ########  build this module
******************************************************************************/
void APP_MESH_Provision_Complete(void)
{
	if(PROV_TYPE_WITH_AMANZON != gst_AppMemDB.Provision)
    {
        gst_AppMemDB.Provision = PROV_TYPE_WITH_AMANZON;
        APP_Platfrom_Set_Mesh_Default_Light_Status();
    }
}


/*********************************************************************
 * @fn      APP_MESH_Status_Send
 *
 * @brief   used to send response to remote after receiving acknowledged-message.
 *
 * @param   ind     - message received from remote node
 *          opcode  - opcode field should be set in response message
 *          msg     - response message pointer
 *          msg_len - response message length
 *
 * @return  None.
 */
void APP_MESH_Status_Send(mesh_model_msg_ind_t const *ind, uint32_t opcode, uint8_t *msg, uint16_t msg_len)
{
	MESH_MODEL_LOG("Status_Send: opcode = 0x%2X ", opcode);
	
	MESH_MODEL_LOG("msg: ");
	for(U8 i = 0; i < msg_len; i++)
	{
		MESH_MODEL_LOG("%02x ", msg[i]);
	}		
	MESH_MODEL_LOG("\r\n");	
	
    mesh_rsp_msg_t * p_rsp_msg = (mesh_rsp_msg_t *)os_malloc((sizeof(mesh_rsp_msg_t)+msg_len));
    p_rsp_msg->element_idx = ind->element;
    p_rsp_msg->app_key_lid = ind->app_key_lid;
    p_rsp_msg->model_id = ind->model_id;
    p_rsp_msg->opcode = BigtoLittle16(opcode);
    p_rsp_msg->dst_addr = ind->src;
    p_rsp_msg->msg_len = msg_len;
    memcpy(p_rsp_msg->msg, msg, msg_len);

    mesh_send_rsp(p_rsp_msg);
    os_free(p_rsp_msg);
}

