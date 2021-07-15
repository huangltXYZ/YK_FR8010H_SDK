
#include "YK_APP.h"



 /** Tid tracker structure. */
static tid_tracker_t tid_tracker;

static bool status_send(mesh_model_msg_ind_t const *p_rx_msg,
							const msg_light_hsl_saturation_status_params_t * p_params)
{
	msg_light_hsl_saturation_status_msg_pkt_t msg_pkt;

	msg_pkt.present_saturation = p_params->present_saturation;
	if (p_params->remaining_times > 0)
	{
		msg_pkt.target_saturation = p_params->target_saturation;
		msg_pkt.remaining_times = model_transition_time_encode(p_params->remaining_times);
	}

	APP_MESH_Status_Send(p_rx_msg, 
	                         LIGHT_HSL_OPCODE_SATURATION_STATUS,
				             (uint8_t *)&msg_pkt, 
				             p_params->remaining_times > 0 ? LIGHT_HSL_STATUS_MAXLEN : LIGHT_HSL_STATUS_MINLEN);
	return true;
}


/** Opcode Handlers */
static void handle_set(mesh_model_msg_ind_t const *p_rx_msg)
{
	MESH_MODEL_LOG("%s\n", __func__ );
    U16 Opcode = BigtoLittle16(p_rx_msg->opcode);
	
	msg_light_hsl_saturation_set_params_t in_data = {0};
	model_transition_t in_data_tr = {0};
	msg_light_hsl_saturation_status_params_t out_data = {0};

	if (p_rx_msg->msg_len == LIGHT_HSL_SATURATION_SET_MINLEN || p_rx_msg->msg_len == LIGHT_HSL_SATURATION_SET_MAXLEN)
	{
		msg_light_hsl_saturation_set_msg_pkt_t * p_msg_params_packed = (msg_light_hsl_saturation_set_msg_pkt_t *) p_rx_msg->msg;
		in_data.hsl_saturation = p_msg_params_packed->hsl_saturation;
		in_data.tid = p_msg_params_packed->tid;

        if(model_tid_validate(&tid_tracker, p_rx_msg, LIGHT_HSL_OPCODE_SATURATION_SET, in_data.tid))
		{
			if (p_rx_msg->msg_len == LIGHT_HSL_SATURATION_SET_MAXLEN)
			{
				if (!model_transition_time_is_valid(p_msg_params_packed->transition_time))
                {
                    return;
                }

                in_data_tr.transition_time_ms = model_transition_time_decode(p_msg_params_packed->transition_time);
                in_data_tr.delay_ms = model_delay_decode(p_msg_params_packed->delay);
				MESH_MODEL_LOG("transition_time_ms = %d delay_ms = %d\r\n", in_data_tr.transition_time_ms, in_data_tr.delay_ms);
			}
            
            Light_Status_Set_Target(LIGHT_STATUS_TYPE_HSL_SATURATION, &in_data.hsl_saturation);
            APP_Light_SetHSL(LightStatus.hue.target   , LightStatus.saturation.target , LightStatus.lightness.target, APP_Light_TRANSITION_TIME);
            
			if (Opcode == LIGHT_HSL_OPCODE_SATURATION_SET)
			{
                out_data.present_saturation = Light_Status_Get_Current(LIGHT_STATUS_TYPE_HSL_SATURATION);
                out_data.target_saturation = Light_Status_Get_Target(LIGHT_STATUS_TYPE_HSL_SATURATION);
                out_data.remaining_times = 0;
                
				status_send(p_rx_msg, &out_data);
			}
		}
	}
}

static void handle_get(mesh_model_msg_ind_t const *p_rx_msg)
{
	msg_light_hsl_saturation_status_params_t out_data = {0};

	if (p_rx_msg->msg_len == 0)
	{
        out_data.present_saturation = Light_Status_Get_Current(LIGHT_STATUS_TYPE_HSL_SATURATION);
        out_data.target_saturation = Light_Status_Get_Target(LIGHT_STATUS_TYPE_HSL_SATURATION);
        out_data.remaining_times = 0;

		status_send(p_rx_msg, &out_data);
	}
}

static const access_opcode_handler_t m_opcode_handlers[] =
{
	{ACCESS_OPCODE_SIG(LIGHT_HSL_OPCODE_SATURATION_GET), handle_get},
	{ACCESS_OPCODE_SIG(LIGHT_HSL_OPCODE_SATURATION_SET), handle_set},
	{ACCESS_OPCODE_SIG(LIGHT_HSL_OPCODE_SATURATION_SET_UNACKNOWLEDGED), handle_set}
};

/*****************************************************************************
Function name :  light_saturation_server_msg_handler
Description:	 Light HSL Saturation Server 消息处理函数
Input:	         p_rx_msg->消息内容
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void light_hsl_saturation_server_msg_handler(mesh_model_msg_ind_t const *p_rx_msg)
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


