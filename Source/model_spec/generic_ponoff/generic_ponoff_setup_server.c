
#include "YK_APP.h"


static bool status_send(mesh_model_msg_ind_t const *p_rx_msg, const generic_ponoff_status_params_t * p_params)
{
    generic_ponoff_status_msg_pkt_t msg_pkt;

    if(p_params->on_powerup > GENERIC_ON_POWERUP_MAX)
    {
        return false;
    }

    msg_pkt.on_powerup = p_params->on_powerup;

	APP_MESH_Status_Send(p_rx_msg, 
	                         GENERIC_PONOFF_OPCODE_STATUS, 
				             (uint8_t *)&msg_pkt, 
				             sizeof(generic_ponoff_status_msg_pkt_t));
	
	return true;
}


static void handle_get(mesh_model_msg_ind_t const *p_rx_msg)
{
    generic_ponoff_status_params_t out_data = {0};

    if (p_rx_msg->msg_len == 0)
    {
		//应用层处理
        out_data.on_powerup = LightStatus.onpowerup;
        status_send(p_rx_msg, &out_data);
    }
}


static void handle_set(mesh_model_msg_ind_t const *p_rx_msg)
{
    U16 Opcode = BigtoLittle16(p_rx_msg->opcode);
	
    generic_ponoff_set_params_t in_param = {0};
    generic_ponoff_status_params_t out_param = {0};

    if (p_rx_msg->msg_len == sizeof(generic_ponoff_set_msg_pkt_t))
    {
        generic_ponoff_set_msg_pkt_t * p_msg_params_packed = (generic_ponoff_set_msg_pkt_t *) p_rx_msg->msg;

        if (p_msg_params_packed->on_powerup > GENERIC_ON_POWERUP_MAX)
        {
            return;
        }

        in_param.on_powerup = p_msg_params_packed->on_powerup;
        LightStatus.onpowerup = in_param.on_powerup;
        Light_Status_Save();	
		MESH_MODEL_LOG("on_powerup = %d,\r\n", in_param.on_powerup);

        if (Opcode == GENERIC_PONOFF_OPCODE_SET)
        {
			out_param.on_powerup = in_param.on_powerup;
            status_send(p_rx_msg, &out_param);
        }
    }
}


static const access_opcode_handler_t m_opcode_handlers[] =
{
    {ACCESS_OPCODE_SIG(GENERIC_PONOFF_OPCODE_SET), handle_set},
    {ACCESS_OPCODE_SIG(GENERIC_PONOFF_OPCODE_SET_UNACKNOWLEDGED), handle_set},
	{ACCESS_OPCODE_SIG(GENERIC_PONOFF_OPCODE_GET), handle_get},
};


/*****************************************************************************
Function name :  generic_ponoff_server_msg_handler
Description:	 Generic Power OnOff Server / Generic Power OnOff Setup Server 消息处理函数
Input:	         p_rx_msg->消息内容
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void generic_ponoff_server_msg_handler(mesh_model_msg_ind_t const *p_rx_msg)
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
