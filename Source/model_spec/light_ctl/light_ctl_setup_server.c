

#include "YK_APP.h"


//#include "light_ctl_utils.h"
 /** Tid tracker structure. */
static tid_tracker_t tid_tracker;


static bool status_send(mesh_model_msg_ind_t const *p_rx_msg,
                            const light_ctl_status_params_t * p_params)
{
    light_ctl_status_msg_pkt_t msg_pkt;

    msg_pkt.present_lightness = p_params->present_lightness;
    msg_pkt.present_temperature = p_params->present_temperature16;
    if (p_params->remaining_time_ms > 0)
    {
        msg_pkt.target_lightness = p_params->target_lightness;
        msg_pkt.target_temperature = p_params->target_temperature16;
        msg_pkt.remaining_time = model_transition_time_encode(p_params->remaining_time_ms);
    }

	APP_MESH_Status_Send(p_rx_msg, 
	                         LIGHT_CTL_OPCODE_STATUS,
				             (uint8_t *)&msg_pkt, 
				             (p_params->remaining_time_ms > 0 ?
                             LIGHT_CTL_STATUS_MAXLEN :
                             LIGHT_CTL_STATUS_MINLEN));
	return true;
}


static bool status_temperature_send(mesh_model_msg_ind_t const *p_rx_msg,
                                        const light_ctl_temperature_status_params_t * p_params)
{
    light_ctl_temperature_status_msg_pkt_t msg_pkt;

    msg_pkt.present_temperature = p_params->present_temperature16;
    msg_pkt.present_delta_uv = p_params->present_delta_uv;
    if (p_params->remaining_time_ms > 0)
    {
        msg_pkt.target_temperature = p_params->target_temperature16;
        msg_pkt.target_delta_uv = p_params->target_delta_uv;
        msg_pkt.remaining_time = model_transition_time_encode(p_params->remaining_time_ms);
    }

	APP_MESH_Status_Send(p_rx_msg, 
	                         LIGHT_CTL_OPCODE_TEMPERATURE_STATUS,
				             (uint8_t *)&msg_pkt, 
				             (p_params->remaining_time_ms > 0 ?
                             LIGHT_CTL_TEMPERATURE_STATUS_MAXLEN :
                             LIGHT_CTL_TEMPERATURE_STATUS_MINLEN));
	return true;
}

static bool status_temperature_range_send(mesh_model_msg_ind_t const *p_rx_msg,
                                              const light_ctl_temperature_range_status_params_t * p_params)
{
    light_ctl_temperature_range_status_msg_pkt_t msg_pkt;

    msg_pkt.status_code = p_params->status_code;
    msg_pkt.range_min = p_params->temperature16_range_min;
    msg_pkt.range_max = p_params->temperature16_range_max;

	APP_MESH_Status_Send(p_rx_msg, 
	                         LIGHT_CTL_OPCODE_TEMPERATURE_RANGE_STATUS,
				             (uint8_t *)&msg_pkt, 
				             LIGHT_CTL_TEMPERATURE_RANGE_STATUS_LEN);
	return true;
}

static bool status_default_send(mesh_model_msg_ind_t const *p_rx_msg,
                                    const light_ctl_default_status_params_t * p_params)
{
    light_ctl_default_status_msg_pkt_t msg_pkt;

    msg_pkt.lightness = p_params->lightness;
    msg_pkt.temperature = p_params->temperature16;
    msg_pkt.delta_uv = p_params->delta_uv;

	APP_MESH_Status_Send(p_rx_msg, 
	                         LIGHT_CTL_OPCODE_DEFAULT_STATUS,
				             (uint8_t *)&msg_pkt, 
				             LIGHT_CTL_DEFAULT_STATUS_LEN);
	return true;
}


static bool handle_set_message_validate(uint16_t short_message_bytes,
                                        uint16_t long_message_bytes,
                                        uint16_t message_bytes,
                                        uint8_t transition_time,
                                        uint8_t delay,
                                        model_transition_t ** pp_transition)
{
    model_transition_t * p_transition;

    p_transition = *pp_transition;

    if (short_message_bytes == message_bytes)
    {
        /* The message does not specify a transition. */
        *pp_transition = NULL;
        return true;
    }

    if (long_message_bytes == message_bytes)
    {
        /* The message specifies a transition. */
        if (!model_transition_time_is_valid(transition_time))
        {
            MESH_MODEL_LOG("Invalid parameter (%d) = (transition_time)\n", transition_time);
            return false;
        }
        p_transition->transition_time_ms = model_transition_time_decode(transition_time);
        p_transition->delay_ms = model_delay_decode(delay);
        return true;
    }

    /* The message length is invalid. */
    MESH_MODEL_LOG("Invalid parameter (%d) = (message_bytes)\n", message_bytes);

    return false;
}

/**************************************************************************************************/
/**** CTL server ****/
/** Opcode Handlers */
static void handle_set(mesh_model_msg_ind_t const *p_rx_msg)
{
    U16 Opcode = BigtoLittle16(p_rx_msg->opcode);
	
    light_ctl_set_msg_pkt_t * p_msg_pkt;
    light_ctl_status_params_t out_data = {0};
    light_ctl_set_params_t in_data = {0};
    model_transition_t transition = {0};
    model_transition_t * p_transition;

    p_msg_pkt = (light_ctl_set_msg_pkt_t *) p_rx_msg->msg;
    if (!model_tid_validate(&tid_tracker, p_rx_msg,
                            LIGHT_CTL_OPCODE_SET, p_msg_pkt->tid))
    {
        return;
    }

    p_transition = &transition;
    if (!handle_set_message_validate(LIGHT_CTL_SET_MINLEN, LIGHT_CTL_SET_MAXLEN, p_rx_msg->msg_len,
                                     p_msg_pkt->transition_time, p_msg_pkt->delay, &p_transition))
    {
        return;
    }

    /* Check for prohibited values @tagMeshMdlSp section 6.1.3.1.  For any prohibited values, don't process,
     * and don't respond */
    if ((p_msg_pkt->temperature < YK_TEMPERATURE_MIN) || (p_msg_pkt->temperature > YK_TEMPERATURE_MAX))
    {
        return;
    }

    in_data.lightness = p_msg_pkt->lightness;
    in_data.temperature16 = p_msg_pkt->temperature;
    in_data.delta_uv = p_msg_pkt->delta_uv;
    in_data.tid = p_msg_pkt->tid;

  
    Light_Status_Set_Target(LIGHT_STATUS_TYPE_ACTUAL, &in_data.lightness);
    Light_Status_Set_Target(LIGHT_STATUS_TYPE_CTL_TEMP, &in_data.temperature16);
    Light_Status_Set_Target(LIGHT_STATUS_TYPE_CTL_DELTA_UV, &in_data.delta_uv);

    APP_Light_SetLevel(LightStatus.lightness.target, APP_Light_TRANSITION_TIME);
    APP_Light_SetTemperature(LightStatus.temp.target, LightStatus.temp.delta_target, APP_Light_TRANSITION_TIME);

    if (LIGHT_CTL_OPCODE_SET == Opcode)
    {
        out_data.present_lightness = Light_Status_Get_Current(LIGHT_STATUS_TYPE_ACTUAL);
        out_data.present_temperature16 = Light_Status_Get_Current(LIGHT_STATUS_TYPE_CTL_TEMP);
        out_data.target_lightness = Light_Status_Get_Target(LIGHT_STATUS_TYPE_ACTUAL);
        out_data.target_temperature16 = Light_Status_Get_Target(LIGHT_STATUS_TYPE_CTL_TEMP);
        out_data.remaining_time_ms = 0;
        
        status_send(p_rx_msg, &out_data);
    }

}

static void handle_get(mesh_model_msg_ind_t const *p_rx_msg)
{
    light_ctl_status_params_t out_data = {0};

    if (p_rx_msg->msg_len == 0)
    {
        out_data.present_lightness = Light_Status_Get_Current(LIGHT_STATUS_TYPE_ACTUAL);
        out_data.present_temperature16 = Light_Status_Get_Current(LIGHT_STATUS_TYPE_CTL_TEMP);
        out_data.target_lightness = Light_Status_Get_Target(LIGHT_STATUS_TYPE_ACTUAL);
        out_data.target_temperature16 = Light_Status_Get_Target(LIGHT_STATUS_TYPE_CTL_TEMP);
        out_data.remaining_time_ms = 0;
      
        status_send(p_rx_msg, &out_data);
    }
}    

static void handle_temperature_range_get(mesh_model_msg_ind_t const *p_rx_msg)
{
    light_ctl_temperature_range_status_params_t out_data = {0};

    if (p_rx_msg->msg_len == 0)
    {
        out_data.temperature16_range_min = LightStatus.temp.range_min;
        out_data.temperature16_range_max = LightStatus.temp.range_max;
        
        status_temperature_range_send(p_rx_msg, &out_data);
    }
}

static void handle_default_get(mesh_model_msg_ind_t const *p_rx_msg)
{
    light_ctl_default_status_params_t out_data = {0};

    if (p_rx_msg->msg_len == 0)
    {
        out_data.delta_uv = LightStatus.temp.delta_def;
        out_data.lightness = LightStatus.lightness.def;
        out_data.temperature16 = LightStatus.temp.def;
        
        status_default_send(p_rx_msg, &out_data);
    }
}


/**************************************************************************************************/
/**** CTL Temperature Server */
/** Opcode Handlers */

static void handle_temperature_set(mesh_model_msg_ind_t const *p_rx_msg)
{
    U16 Opcode = BigtoLittle16(p_rx_msg->opcode);
	
    light_ctl_temperature_set_msg_pkt_t * p_msg_pkt;
    light_ctl_temperature_status_params_t out_data = {0};
    light_ctl_temperature_set_params_t in_data = {0};
    model_transition_t transition = {0};
    model_transition_t * p_transition;

    p_msg_pkt = (light_ctl_temperature_set_msg_pkt_t *) p_rx_msg->msg;
	
    if (!model_tid_validate(&tid_tracker, p_rx_msg,
                            LIGHT_CTL_OPCODE_TEMPERATURE_SET, p_msg_pkt->tid))
    {
        return;
    }

    p_transition = &transition;
    if (!handle_set_message_validate(LIGHT_CTL_TEMPERATURE_SET_MINLEN, LIGHT_CTL_TEMPERATURE_SET_MAXLEN,
                                     p_rx_msg->msg_len, p_msg_pkt->transition_time,
                                     p_msg_pkt->delay, &p_transition))
    {
        return;
    }

    /* Check for prohibited values @tagMeshMdlSp section 6.1.3.1.  For any prohibited values, don't process,
     * and don't respond */
    if ((p_msg_pkt->temperature < LIGHT_CTL_TEMPERATURE_MIN_LIMIT) ||
        (p_msg_pkt->temperature > LIGHT_CTL_TEMPERATURE_MAX_LIMIT))
    {
        return;
    }

    in_data.temperature16 = p_msg_pkt->temperature;
    in_data.delta_uv = p_msg_pkt->delta_uv;
    in_data.tid = p_msg_pkt->tid;

    Light_Status_Set_Target(LIGHT_STATUS_TYPE_CTL_TEMP, &in_data.temperature16);
    Light_Status_Set_Target(LIGHT_STATUS_TYPE_CTL_DELTA_UV, &in_data.delta_uv);

    APP_Light_SetTemperature(LightStatus.temp.target, LightStatus.temp.delta_target, APP_Light_TRANSITION_TIME);

	
    if (LIGHT_CTL_OPCODE_TEMPERATURE_SET == Opcode)
    {
        out_data.present_temperature16 = Light_Status_Get_Current(LIGHT_STATUS_TYPE_CTL_TEMP);
        out_data.present_delta_uv = Light_Status_Get_Current(LIGHT_STATUS_TYPE_CTL_DELTA_UV);
        out_data.target_temperature16 = Light_Status_Get_Target(LIGHT_STATUS_TYPE_CTL_TEMP);
        out_data.target_delta_uv = Light_Status_Get_Target(LIGHT_STATUS_TYPE_CTL_DELTA_UV);
        out_data.remaining_time_ms = 0;
        
        status_temperature_send(p_rx_msg, &out_data);
    }
}

static void handle_temperature_get(mesh_model_msg_ind_t const *p_rx_msg)
{
    light_ctl_temperature_status_params_t out_data = {0};

    if (p_rx_msg->msg_len == 0)
    {
        out_data.present_temperature16 = Light_Status_Get_Current(LIGHT_STATUS_TYPE_CTL_TEMP);
        out_data.present_delta_uv = Light_Status_Get_Current(LIGHT_STATUS_TYPE_CTL_DELTA_UV);
        out_data.target_temperature16 = Light_Status_Get_Target(LIGHT_STATUS_TYPE_CTL_TEMP);
        out_data.target_delta_uv = Light_Status_Get_Target(LIGHT_STATUS_TYPE_CTL_DELTA_UV);
        out_data.remaining_time_ms = 0;

        status_temperature_send(p_rx_msg, &out_data);
    }
}

/**************************************************************************************************/
/**** CTL Setup Server */
/** Opcode Handlers */

static void handle_default_set(mesh_model_msg_ind_t const *p_rx_msg)
{
    U16 Opcode = BigtoLittle16(p_rx_msg->opcode);
	
    light_ctl_default_set_msg_pkt_t * p_msg_pkt;
    light_ctl_default_set_params_t in_data = {0};
    light_ctl_default_status_params_t out_data = {0};

    if (sizeof(light_ctl_default_set_msg_pkt_t) == p_rx_msg->msg_len)
    {
        p_msg_pkt = (light_ctl_default_set_msg_pkt_t *) p_rx_msg->msg;

        /* Check for prohibited values @tagMeshMdlSp section 6.1.3.1.  For any prohibited values, don't process,
         * and don't respond */
        if ((p_msg_pkt->temperature < LIGHT_CTL_TEMPERATURE_MIN_LIMIT) ||
            (p_msg_pkt->temperature > LIGHT_CTL_TEMPERATURE_MAX_LIMIT))
        {
            return;
        }

        in_data.lightness = p_msg_pkt->lightness;
        in_data.temperature16 = p_msg_pkt->temperature;
        in_data.delta_uv = p_msg_pkt->delta_uv;

        if((LightStatus.lightness.def != in_data.lightness) ||
           (LightStatus.temp.def != in_data.temperature16) ||
           (LightStatus.temp.delta_def != in_data.delta_uv))
        {
            LightStatus.lightness.def = in_data.lightness;
            LightStatus.temp.def = in_data.temperature16;
            LightStatus.temp.delta_def = in_data.delta_uv;
            Light_Status_Save();
        }

        if (LIGHT_CTL_OPCODE_DEFAULT_SET == Opcode)
        {
            out_data.lightness = in_data.lightness;
            out_data.temperature16 = in_data.temperature16;
            out_data.delta_uv = in_data.delta_uv;
            status_default_send(p_rx_msg, &out_data);
        }
    }
}

static void handle_temperature_range_set(mesh_model_msg_ind_t const *p_rx_msg)
{
    U16 Opcode = BigtoLittle16(p_rx_msg->opcode);
	
    light_ctl_temperature_range_set_msg_pkt_t * p_msg_pkt;
    light_ctl_temperature_range_set_params_t in_data = {0};
    light_ctl_temperature_range_status_params_t out_data = {0};

    if (sizeof(light_ctl_temperature_range_set_msg_pkt_t) == p_rx_msg->msg_len)
    {
        p_msg_pkt = (light_ctl_temperature_range_set_msg_pkt_t *) p_rx_msg->msg;

        /* Check for prohibited values @tagMeshMdlSp section 6.1.3.3.  For any prohibited values, don't
         * process, and don't respond */
        if ((p_msg_pkt->range_min < LIGHT_CTL_TEMPERATURE_MIN_LIMIT) ||
            (p_msg_pkt->range_max > LIGHT_CTL_TEMPERATURE_MAX_LIMIT) ||
            (p_msg_pkt->range_min > p_msg_pkt->range_max))
        {
            return;
        }
        
        if ((p_msg_pkt->range_min < YK_TEMPERATURE_MIN) ||
             (p_msg_pkt->range_max > YK_TEMPERATURE_MAX))
        {
            return;
        }

        in_data.temperature16_range_min = p_msg_pkt->range_min;
        in_data.temperature16_range_max = p_msg_pkt->range_max;

        if((LightStatus.temp.range_min != in_data.temperature16_range_min) ||
          (LightStatus.temp.range_max != in_data.temperature16_range_max))
        {
            LightStatus.temp.range_min = in_data.temperature16_range_min;
            LightStatus.temp.range_max = in_data.temperature16_range_max;
            Light_Status_Save();
        }

        if (LIGHT_CTL_OPCODE_TEMPERATURE_RANGE_SET == Opcode)
        {
            out_data.temperature16_range_min = in_data.temperature16_range_min;
            out_data.temperature16_range_max = in_data.temperature16_range_max;
            status_temperature_range_send(p_rx_msg, &out_data);
        }
    }
}

static const access_opcode_handler_t m_opcode_handlers[] =
{
	{ACCESS_OPCODE_SIG(LIGHT_CTL_OPCODE_GET), handle_get},
    {ACCESS_OPCODE_SIG(LIGHT_CTL_OPCODE_SET), handle_set},
    {ACCESS_OPCODE_SIG(LIGHT_CTL_OPCODE_SET_UNACKNOWLEDGED), handle_set},
    {ACCESS_OPCODE_SIG(LIGHT_CTL_OPCODE_TEMPERATURE_RANGE_GET), handle_temperature_range_get},
    {ACCESS_OPCODE_SIG(LIGHT_CTL_OPCODE_DEFAULT_GET), handle_default_get},
	
    {ACCESS_OPCODE_SIG(LIGHT_CTL_OPCODE_DEFAULT_SET), handle_default_set},
    {ACCESS_OPCODE_SIG(LIGHT_CTL_OPCODE_DEFAULT_SET_UNACKNOWLEDGED), handle_default_set},
    {ACCESS_OPCODE_SIG(LIGHT_CTL_OPCODE_TEMPERATURE_RANGE_SET), handle_temperature_range_set},
    {ACCESS_OPCODE_SIG(LIGHT_CTL_OPCODE_TEMPERATURE_RANGE_SET_UNACKNOWLEDGED), handle_temperature_range_set},
	
	{ACCESS_OPCODE_SIG(LIGHT_CTL_OPCODE_TEMPERATURE_GET), handle_temperature_get},
    {ACCESS_OPCODE_SIG(LIGHT_CTL_OPCODE_TEMPERATURE_SET), handle_temperature_set},
    {ACCESS_OPCODE_SIG(LIGHT_CTL_OPCODE_TEMPERATURE_SET_UNACKNOWLEDGED), handle_temperature_set},
};


/*****************************************************************************
Function name :  light_ctl_server_msg_handler
Description:	 Light CTL Server / Light CTL Temperature Server / Light CTL Setup Server 消息处理函数
Input:	         p_rx_msg->消息内容
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void light_ctl_server_msg_handler(mesh_model_msg_ind_t const *p_rx_msg)
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




