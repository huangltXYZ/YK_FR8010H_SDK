#include "YK_APP.h"
#include "light_hsl_common.h"
#include "light_hsl_messages.h"


static tid_tracker_t tid_tracker;


static uint32_t status_send(mesh_model_msg_ind_t const *p_rx_msg,
							const msg_light_hsl_status_params_t * p_params)
{
	msg_light_hsl_status_msg_pkt_t msg_pkt;

	msg_pkt.hsl_hue = p_params->present_hsl_hue;
	msg_pkt.hsl_lightness = p_params->present_hsl_lightness;
	msg_pkt.hsl_saturation = p_params->present_hsl_saturation;

	app_mesh_status_send_rsp(p_rx_msg, 
	                         LIGHT_HSL_OPCODE_STATUS,
				             (uint8_t *)&msg_pkt, 
				             (p_params->remaining_times > 0 ?
					          LIGHT_HSL_STATUS_MAXLEN :
					          LIGHT_HSL_STATUS_MINLEN));
	return true;
}

static bool status_range_send(mesh_model_msg_ind_t const *p_rx_msg,
								  const msg_light_hsl_range_status_params_t * p_params)
{
	msg_light_hsl_range_status_msg_pkt_t msg_pkt;

	msg_pkt.status_code = p_params->status_code;
	msg_pkt.hue_range_min = (p_params->hue_range_min);
	msg_pkt.hue_range_max = (p_params->hue_range_max);
	msg_pkt.saturation_range_min = (p_params->saturation_range_min);
	msg_pkt.saturation_range_max = (p_params->saturation_range_max);

	app_mesh_status_send_rsp(p_rx_msg, 
	                         LIGHT_HSL_OPCODE_RANGE_STATUS,
				             (uint8_t *)&msg_pkt, 
				             LIGHT_HSL_RANGE_STATUS_LEN);
	return true;
}

static bool status_default_send(mesh_model_msg_ind_t const *p_rx_msg,
									const msg_light_hsl_default_status_params_t * p_params)
{
	msg_light_hsl_default_status_msg_pkt_t msg_pkt;

	msg_pkt.hsl_lightness = p_params->hsl_lightness;
	msg_pkt.hsl_hue = p_params->hsl_hue;
	msg_pkt.hsl_saturation = p_params->hsl_saturation;

	app_mesh_status_send_rsp(p_rx_msg, 
	                         LIGHT_HSL_OPCODE_DEFAULT_STATUS,
				             (uint8_t *)&msg_pkt, 
				             LIGHT_HSL_DEFAULT_STATUS_LEN);
	return true;
}

static bool status_target_send(mesh_model_msg_ind_t const *p_rx_msg,
								   const msg_light_hsl_target_status_params_t * p_params)
{
	msg_light_hsl_target_status_msg_pkt_t msg_pkt;

	msg_pkt.hsl_lightness_target = p_params->hsl_lightness_target;
	msg_pkt.hsl_hue_target = p_params->hsl_hue_target;
	msg_pkt.hsl_saturation_target = p_params->hsl_saturation_target;

	app_mesh_status_send_rsp(p_rx_msg, 
	                         LIGHT_HSL_OPCODE_TARGET_STATUS,
				             (uint8_t *)&msg_pkt, 
				             (p_params->remaining_times > 0 ?
					          LIGHT_HSL_TARGET_STATUS_MAXLEN :
					          LIGHT_HSL_TARGET_STATUS_MINLEN));
	return true;
}


/** Opcode Handlers */
static void handle_set(mesh_model_msg_ind_t const *p_rx_msg)
{
	MESH_MODEL_LOG("%s\n", __func__ );
	U16 Opcode = BigtoLittle16(p_rx_msg->opcode);
	
	msg_light_hsl_set_msg_pkt_t * p_msg_pkt;
	msg_light_hsl_status_params_t out_data = {0};
	msg_light_hsl_status_params_t * p_out_data = NULL;
	msg_light_hsl_set_params_t in_data = {0};
	model_transition_t transition = {0};
	model_transition_t * p_transition;

	p_msg_pkt = (msg_light_hsl_set_msg_pkt_t *) p_rx_msg->msg;
	if (!model_tid_validate(&tid_tracker, p_rx_msg,
							LIGHT_HSL_OPCODE_SET, p_msg_pkt->tid))
	{
		return;
	}

	p_transition = &transition;
	
	if (!handle_set_message_validate(LIGHT_HSL_SET_MINLEN, LIGHT_HSL_SET_MAXLEN, p_rx_msg->msg_len,
									 p_msg_pkt->trans_time, p_msg_pkt->delay, &p_transition))
	{
		return;
	}

	in_data.hsl_lightness = p_msg_pkt->hsl_lightness;
	in_data.hsl_hue = (p_msg_pkt->hsl_hue);
	in_data.hsl_saturation = p_msg_pkt->hsl_saturation;
	in_data.tid = p_msg_pkt->tid;

//	in_data.temperature32 = clip_temperature32_to_range(p_s_server, &p_rx_msg->meta_data,
//														in_data.temperature32);

	if(LIGHT_HSL_OPCODE_SET == Opcode)
	{
		p_out_data = &out_data;
	}

//	p_s_server->settings.p_callbacks->light_hsl_cbs.set_cb(p_s_server, &p_rx_msg->meta_data, &in_data,
//														   p_transition, p_out_data);

	if (p_out_data != NULL)
	{
		status_send(p_rx_msg, p_out_data);
	}
}

static void handle_get(mesh_model_msg_ind_t const *p_rx_msg)
{
	MESH_MODEL_LOG("%s\n", __func__ );

	msg_light_hsl_status_params_t out_data = {0};

	if (p_rx_msg->msg_len == 0)
	{
//		p_s_server->settings.p_callbacks->light_hsl_cbs.get_cb(p_s_server, &p_rx_msg->meta_data, &out_data);
		status_send(p_rx_msg, &out_data);
	}
}

static void handle_range_get(mesh_model_msg_ind_t const *p_rx_msg)
{
	MESH_MODEL_LOG("%s\n", __func__ );

	msg_light_hsl_range_status_params_t out_data = {0};

	if (p_rx_msg->msg_len == 0)
	{
//		p_s_server->settings.p_callbacks->light_hsl_cbs.range_get_cb(p_s_server,
//																	 &p_rx_msg->meta_data,
//																	 &out_data);

		status_range_send(p_rx_msg, &out_data);
	}
}

static void handle_default_get(mesh_model_msg_ind_t const *p_rx_msg)
{
	MESH_MODEL_LOG("%s\n", __func__ );
	
	msg_light_hsl_default_status_params_t out_data = {0};

	if (p_rx_msg->msg_len == 0)
	{
//		p_s_server->settings.p_callbacks->light_hsl_cbs.default_get_cb(p_s_server, &p_rx_msg->meta_data,
//																	   &out_data);
		status_default_send(p_rx_msg, &out_data);
	}
}

static void handle_target_get(mesh_model_msg_ind_t const *p_rx_msg)
{
	MESH_MODEL_LOG("%s\n", __func__ );
	
	msg_light_hsl_target_status_params_t out_data = {0};

	if (p_rx_msg->msg_len == 0)
	{
//		p_s_server->settings.p_callbacks->light_hsl_cbs.target_get_cb(p_s_server, &p_rx_msg->meta_data,
//																	  &out_data);
		status_target_send(p_rx_msg, &out_data);
	}
}


static void handle_default_set(mesh_model_msg_ind_t const *p_rx_msg)
{
	MESH_MODEL_LOG("%s\n", __func__ );
	U16 Opcode = BigtoLittle16(p_rx_msg->opcode);
	
	msg_light_hsl_default_set_msg_pkt_t * p_msg_pkt;
	msg_light_hsl_default_set_params_t in_data = {0};
	msg_light_hsl_default_status_params_t out_data = {0};
	msg_light_hsl_default_status_params_t * p_out_data = NULL;

	if (sizeof(msg_light_hsl_default_set_msg_pkt_t) == p_rx_msg->msg_len)
	{
		p_msg_pkt = (msg_light_hsl_default_set_msg_pkt_t *) p_rx_msg->msg;

		in_data.hsl_lightness = p_msg_pkt->hsl_lightness;
		in_data.hsl_hue = (p_msg_pkt->hsl_hue);
		in_data.hsl_saturation = p_msg_pkt->hsl_saturation;

		if (LIGHT_HSL_OPCODE_DEFAULT_SET == Opcode)
		{
			p_out_data = &out_data;
		}

//		p_s_server->settings.p_callbacks->light_hsl_cbs.default_set_cb(p_s_server, &p_rx_msg->meta_data,
//																	   &in_data, p_out_data);
		if (p_out_data != NULL)
		{
			status_default_send(p_rx_msg, &out_data);
		}
	}
}

static void handle_range_set(mesh_model_msg_ind_t const *p_rx_msg)
{
	MESH_MODEL_LOG("%s\n", __func__ );

	U16 Opcode = BigtoLittle16(p_rx_msg->opcode);
	
	msg_light_hsl_range_set_msg_pkt_t * p_msg_pkt;
	msg_light_hsl_range_set_params_t in_data = {0};
	msg_light_hsl_range_status_params_t out_data = {0};
	msg_light_hsl_range_status_params_t * p_out_data = NULL;

	if (sizeof(msg_light_hsl_range_set_msg_pkt_t) == p_rx_msg->msg_len)
	{
		p_msg_pkt = (msg_light_hsl_range_set_msg_pkt_t *) p_rx_msg->msg;

		in_data.hue_range_min = (p_msg_pkt->hue_range_min);
		in_data.hue_range_max = (p_msg_pkt->hue_range_max);
		in_data.saturation_range_min = (p_msg_pkt->saturation_range_min);
		in_data.saturation_range_max = (p_msg_pkt->saturation_range_max);


		if (LIGHT_HSL_OPCODE_RANGE_SET == Opcode)
		{
			p_out_data = &out_data;
		}

//		p_s_server->settings.p_callbacks->light_hsl_cbs.range_set_cb(p_s_server,
//																	 &p_rx_msg->meta_data,
//																	 &in_data, p_out_data);
		if (p_out_data != NULL)
		{
			status_range_send(p_rx_msg, &out_data);
		}
	}
}


static const access_opcode_handler_t m_opcode_handlers[] =
{
	{ACCESS_OPCODE_SIG(LIGHT_HSL_OPCODE_GET), handle_get},
	{ACCESS_OPCODE_SIG(LIGHT_HSL_OPCODE_SET), handle_set},
	{ACCESS_OPCODE_SIG(LIGHT_HSL_OPCODE_SET_UNACKNOWLEDGED), handle_set},

	{ACCESS_OPCODE_SIG(LIGHT_HSL_OPCODE_TARGET_GET), handle_target_get},
	{ACCESS_OPCODE_SIG(LIGHT_HSL_OPCODE_DEFAULT_GET), handle_default_get},
	{ACCESS_OPCODE_SIG(LIGHT_HSL_OPCODE_RANGE_GET), handle_range_get},
	
	{ACCESS_OPCODE_SIG(LIGHT_HSL_OPCODE_DEFAULT_SET), handle_default_set},
	{ACCESS_OPCODE_SIG(LIGHT_HSL_OPCODE_DEFAULT_SET_UNACKNOWLEDGED), handle_default_set},
	{ACCESS_OPCODE_SIG(LIGHT_HSL_OPCODE_RANGE_SET), handle_range_set},
	{ACCESS_OPCODE_SIG(LIGHT_HSL_OPCODE_RANGE_SET_UNACKNOWLEDGED), handle_range_set},
};



/*****************************************************************************
Function name :  light_hsl_server_msg_handler
Description:	 Light HSL Server / Light HSL Setup Server 消息处理函数
Input:	         p_rx_msg->消息内容
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void light_hsl_server_msg_handler(mesh_model_msg_ind_t const *p_rx_msg)
{
	U8 ucIndex = 0;
	U16 Opcode = BigtoLittle16(p_rx_msg->opcode);
	
	MESH_MODEL_LOG("%s: opcode = 0x%2X\n",  __func__, Opcode);
	
	for(ucIndex = 0; ucIndex < NumberOfElements(m_opcode_handlers); ucIndex++)
	{
		if(Opcode == m_opcode_handlers[ucIndex].opcode.opcode)
		{
			m_opcode_handlers[ucIndex].handler(p_rx_msg);
			break;
		}
	}
}

