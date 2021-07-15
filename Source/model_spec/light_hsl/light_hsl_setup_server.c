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

	APP_MESH_Status_Send(p_rx_msg, 
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
	msg_pkt.hue_range_min = p_params->hue_range_min;
	msg_pkt.hue_range_max = p_params->hue_range_max;
	msg_pkt.saturation_range_min = p_params->saturation_range_min;
	msg_pkt.saturation_range_max = p_params->saturation_range_max;

	APP_MESH_Status_Send(p_rx_msg, 
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

	APP_MESH_Status_Send(p_rx_msg, 
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

	APP_MESH_Status_Send(p_rx_msg, 
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
	U16 Opcode = BigtoLittle16(p_rx_msg->opcode);
	
	msg_light_hsl_set_msg_pkt_t * p_msg_pkt;
	msg_light_hsl_status_params_t out_data = {0};
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

    Light_Status_Set_Target(LIGHT_STATUS_TYPE_HSL_LIGHT, &in_data.hsl_lightness);
    Light_Status_Set_Target(LIGHT_STATUS_TYPE_HSL_HUE, &in_data.hsl_hue);
    Light_Status_Set_Target(LIGHT_STATUS_TYPE_HSL_SATURATION, &in_data.hsl_saturation);

    APP_Light_SetLevel(LIGHTNESS_TO_LEVEL(LightStatus.lightness.target), APP_Light_TRANSITION_TIME);
    APP_Light_SetHSL(LightStatus.hue.target, LightStatus.saturation.target , LightStatus.lightness.target, APP_Light_TRANSITION_TIME);
	
	if(LIGHT_HSL_OPCODE_SET == Opcode)
	{
        out_data.present_hsl_lightness = Light_Status_Get_Current(LIGHT_STATUS_TYPE_HSL_LIGHT);
        out_data.present_hsl_hue = Light_Status_Get_Current(LIGHT_STATUS_TYPE_HSL_HUE);
        out_data.present_hsl_saturation = Light_Status_Get_Current(LIGHT_STATUS_TYPE_HSL_SATURATION);
        
        out_data.target_hsl_lightness = Light_Status_Get_Target(LIGHT_STATUS_TYPE_HSL_LIGHT);
        out_data.target_hsl_hue = Light_Status_Get_Target(LIGHT_STATUS_TYPE_HSL_HUE);
        out_data.target_hsl_saturation = Light_Status_Get_Target(LIGHT_STATUS_TYPE_HSL_SATURATION);

        out_data.remaining_times = 0;

		status_send(p_rx_msg, &out_data);
	}
}

static void handle_get(mesh_model_msg_ind_t const *p_rx_msg)
{
	msg_light_hsl_status_params_t out_data = {0};

	if (p_rx_msg->msg_len == 0)
	{
        out_data.present_hsl_lightness = Light_Status_Get_Current(LIGHT_STATUS_TYPE_HSL_LIGHT);
        out_data.present_hsl_hue = Light_Status_Get_Current(LIGHT_STATUS_TYPE_HSL_HUE);
        out_data.present_hsl_saturation = Light_Status_Get_Current(LIGHT_STATUS_TYPE_HSL_SATURATION);
        
        out_data.target_hsl_lightness = Light_Status_Get_Target(LIGHT_STATUS_TYPE_HSL_LIGHT);
        out_data.target_hsl_hue = Light_Status_Get_Target(LIGHT_STATUS_TYPE_HSL_HUE);
        out_data.target_hsl_saturation = Light_Status_Get_Target(LIGHT_STATUS_TYPE_HSL_SATURATION);

        out_data.remaining_times = 0;

		status_send(p_rx_msg, &out_data);
	}
}

static void handle_range_get(mesh_model_msg_ind_t const *p_rx_msg)
{
	MESH_MODEL_LOG("%s\n", __func__ );

	msg_light_hsl_range_status_params_t out_data = {0};

	if (p_rx_msg->msg_len == 0)
	{
        out_data.hue_range_min = LightStatus.hue.range_min;
        out_data.hue_range_max = LightStatus.hue.range_max;
        out_data.saturation_range_min = LightStatus.saturation.range_min;
        out_data.saturation_range_max = LightStatus.saturation.range_max;

		status_range_send(p_rx_msg, &out_data);
	}
}

static void handle_default_get(mesh_model_msg_ind_t const *p_rx_msg)
{
	MESH_MODEL_LOG("%s\n", __func__ );
	
	msg_light_hsl_default_status_params_t out_data = {0};

	if (p_rx_msg->msg_len == 0)
	{
        out_data.hsl_lightness = LightStatus.lightness.def;
        out_data.hsl_hue = LightStatus.hue.def;
        out_data.hsl_saturation = LightStatus.saturation.def;

		status_default_send(p_rx_msg, &out_data);
	}
}

static void handle_target_get(mesh_model_msg_ind_t const *p_rx_msg)
{
	MESH_MODEL_LOG("%s\n", __func__ );
	
	msg_light_hsl_target_status_params_t out_data = {0};

	if (p_rx_msg->msg_len == 0)
	{
        out_data.hsl_lightness_target = Light_Status_Get_Target(LIGHT_STATUS_TYPE_HSL_LIGHT);
        out_data.hsl_hue_target = Light_Status_Get_Target(LIGHT_STATUS_TYPE_HSL_HUE);
        out_data.hsl_saturation_target = Light_Status_Get_Target(LIGHT_STATUS_TYPE_HSL_SATURATION);
        out_data.remaining_times = 0;
        
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

	if (sizeof(msg_light_hsl_default_set_msg_pkt_t) == p_rx_msg->msg_len)
	{
		p_msg_pkt = (msg_light_hsl_default_set_msg_pkt_t *) p_rx_msg->msg;

		in_data.hsl_lightness = p_msg_pkt->hsl_lightness;
		in_data.hsl_hue = (p_msg_pkt->hsl_hue);
		in_data.hsl_saturation = p_msg_pkt->hsl_saturation;

        if((LightStatus.lightness.def != in_data.hsl_lightness) ||
            (LightStatus.hue.def != in_data.hsl_hue) ||
            (LightStatus.saturation.def != in_data.hsl_saturation))
        {
            LightStatus.lightness.def = in_data.hsl_lightness;
            LightStatus.hue.def = in_data.hsl_hue;
            LightStatus.saturation.def = in_data.hsl_saturation;
           
            Light_Status_Save();
        }

		if (LIGHT_HSL_OPCODE_DEFAULT_SET == Opcode)
		{
            out_data.hsl_lightness = in_data.hsl_lightness;
            out_data.hsl_hue = in_data.hsl_hue;
            out_data.hsl_saturation = in_data.hsl_saturation;
            
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

	if (sizeof(msg_light_hsl_range_set_msg_pkt_t) == p_rx_msg->msg_len)
	{
		p_msg_pkt = (msg_light_hsl_range_set_msg_pkt_t *) p_rx_msg->msg;

		in_data.hue_range_min = (p_msg_pkt->hue_range_min);
		in_data.hue_range_max = (p_msg_pkt->hue_range_max);
		in_data.saturation_range_min = (p_msg_pkt->saturation_range_min);
		in_data.saturation_range_max = (p_msg_pkt->saturation_range_max);

        if((in_data.hue_range_min != LightStatus.hue.range_min) || 
           (in_data.hue_range_max != LightStatus.hue.range_max) || 
           (in_data.saturation_range_min != LightStatus.saturation.range_min) || 
           (in_data.saturation_range_max != LightStatus.saturation.range_max))
        {
            LightStatus.hue.range_min = in_data.hue_range_min;
            LightStatus.hue.range_max = in_data.hue_range_max;
            LightStatus.saturation.range_min = in_data.saturation_range_min;
            LightStatus.saturation.range_max = in_data.saturation_range_max;

            Light_Status_Save();

         }
        
		if (LIGHT_HSL_OPCODE_RANGE_SET == Opcode)
		{
            out_data.hue_range_min = in_data.hue_range_min;
            out_data.hue_range_max = in_data.hue_range_max;
            out_data.saturation_range_min = in_data.saturation_range_min;
            out_data.saturation_range_max = in_data.saturation_range_max;
            
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

