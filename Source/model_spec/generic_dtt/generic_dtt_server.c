
#include "YK_APP.h"





static bool status_send(mesh_model_msg_ind_t const *p_rx_msg, const generic_dtt_status_params_t *p_params)
{
    generic_dtt_status_msg_pkt_t msg_pkt;

    msg_pkt.transition_time = model_transition_time_encode(p_params->transition_time_ms);
	
	APP_MESH_Status_Send(p_rx_msg, 
				             GENERIC_DTT_OPCODE_STATUS, 
				             (uint8_t *)&msg_pkt, 
				             sizeof(generic_dtt_status_msg_pkt_t));
	
	return true;						
}



/** Opcode Handlers */

static void handle_set(mesh_model_msg_ind_t const *p_rx_msg)
{
    U16 Opcode = BigtoLittle16(p_rx_msg->opcode);
	
    generic_dtt_set_params_t in_data = {0};
    generic_dtt_status_params_t out_data = {0};

    if (p_rx_msg->msg_len == sizeof(generic_dtt_set_msg_pkt_t))
    {
        generic_dtt_set_msg_pkt_t * p_msg_params_packed = (generic_dtt_set_msg_pkt_t *) p_rx_msg->msg;
        if (!model_transition_time_is_valid(p_msg_params_packed->transition_time))
        {
            return;
        }

        in_data.transition_time_ms = model_transition_time_decode(p_msg_params_packed->transition_time);
        MESH_MODEL_LOG("transition_time_ms = %d", in_data.transition_time_ms);
        LightStatus.dtt = p_msg_params_packed->transition_time;
        Light_Status_Save();	
        
        if (Opcode == GENERIC_DTT_OPCODE_SET)
        {
            status_send(p_rx_msg, &out_data);
        }
    }
}

static void handle_get(mesh_model_msg_ind_t const *p_rx_msg)
{
    generic_dtt_status_params_t out_data = {0};

    if (p_rx_msg->msg_len == 0)
    {
		out_data.transition_time_ms = LightStatus.dtt;
        status_send(p_rx_msg, &out_data);
    }
}

static const access_opcode_handler_t m_opcode_handlers[] =
{
    {ACCESS_OPCODE_SIG(GENERIC_DTT_OPCODE_SET), handle_set},
    {ACCESS_OPCODE_SIG(GENERIC_DTT_OPCODE_SET_UNACKNOWLEDGED), handle_set},
    {ACCESS_OPCODE_SIG(GENERIC_DTT_OPCODE_GET), handle_get},
};



/*****************************************************************************
Function name :  generic_dtt_server_msg_handler
Description:	 Generic DTT Server 消息处理函数
Input:	         p_rx_msg->消息内容
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void generic_dtt_server_msg_handler(mesh_model_msg_ind_t const *p_rx_msg)
{
	U8 ucIndex = 0;
	U16 Opcode = BigtoLittle16(p_rx_msg->opcode);

	for(ucIndex = 0; ucIndex < NumberOfElements(m_opcode_handlers); ucIndex++)
	{
		if(Opcode == m_opcode_handlers[ucIndex].opcode.opcode)
		{
			MESH_MODEL_LOG("%s: opcode = 0x%2X ",  __func__, Opcode);
			
			MESH_MODEL_LOG("msg: ");
            for(U8 i = 0; i < p_rx_msg->msg_len; i++)
            {
                MESH_MODEL_LOG("%02x ", p_rx_msg->msg[i]);
            }
			MESH_MODEL_LOG("\r\n");	
			
			m_opcode_handlers[ucIndex].handler(p_rx_msg);
			break;
		}
	}
}

