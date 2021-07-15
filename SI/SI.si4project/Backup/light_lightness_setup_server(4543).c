

#include "YK_APP.h"






 /** Tid tracker structure. */
static tid_tracker_t tid_tracker;


typedef void (*lightness_set_response_t)(mesh_model_msg_ind_t const *p_rx_msg,
                                         light_lightness_status_params_t * p_out_data);

static uint32_t status_actual_send(mesh_model_msg_ind_t const *p_rx_msg,
                                   const light_lightness_status_params_t * p_params)
{
    light_lightness_status_msg_pkt_t msg_pkt;

    msg_pkt.present_lightness = p_params->present_lightness;
    if (p_params->remaining_time_ms > 0)
    {
        msg_pkt.target_lightness = p_params->target_lightness;
        msg_pkt.remaining_time = model_transition_time_encode(p_params->remaining_time_ms);
    }

    app_mesh_status_send_rsp(p_rx_msg, 
	                        LIGHT_LIGHTNESS_OPCODE_LINEAR_STATUS, 
			                (uint8_t *)&msg_pkt, 
			                 (p_params->remaining_time_ms > 0 ?
                              LIGHT_LIGHTNESS_STATUS_MAXLEN :
                              LIGHT_LIGHTNESS_STATUS_MINLEN));
	return true;
}


static uint32_t status_linear_send(mesh_model_msg_ind_t const *p_rx_msg,
                                   const light_lightness_linear_status_params_t * p_params)
{
    light_lightness_linear_status_msg_pkt_t msg_pkt;

    msg_pkt.present_lightness = p_params->present_lightness;
    if (p_params->remaining_time_ms > 0)
    {
        msg_pkt.target_lightness = p_params->target_lightness;
        msg_pkt.remaining_time = model_transition_time_encode(p_params->remaining_time_ms);
    }

    app_mesh_status_send_rsp(p_rx_msg, 
	                        LIGHT_LIGHTNESS_OPCODE_LINEAR_STATUS, 
			                (uint8_t *)&msg_pkt, 
			                 (p_params->remaining_time_ms > 0 ?
                             LIGHT_LIGHTNESS_LINEAR_STATUS_MAXLEN :
                             LIGHT_LIGHTNESS_LINEAR_STATUS_MINLEN));
	
	return true;
}

static bool status_last_send(mesh_model_msg_ind_t const *p_rx_msg,
                                 const light_lightness_last_status_params_t * p_params)
{
    light_lightness_last_status_msg_pkt_t msg_pkt;

    if (p_params->lightness < LIGHT_LIGHTNESS_LAST_MIN)
    {
        return false;
    }

    msg_pkt.lightness = p_params->lightness;

    app_mesh_status_send_rsp(p_rx_msg, 
	                        LIGHT_LIGHTNESS_OPCODE_LAST_STATUS, 
			                (uint8_t *)&msg_pkt, 
			                 LIGHT_LIGHTNESS_LAST_STATUS_LEN);
	
	return true;
}

static bool status_default_send(mesh_model_msg_ind_t const *p_rx_msg,
                                    const light_lightness_default_status_params_t * p_params)
{
    light_lightness_default_status_msg_pkt_t msg_pkt;

    msg_pkt.lightness = p_params->lightness;

	app_mesh_status_send_rsp(p_rx_msg, 
	                        LIGHT_LIGHTNESS_OPCODE_DEFAULT_STATUS, 
			                (uint8_t *)&msg_pkt, 
			                 LIGHT_LIGHTNESS_DEFAULT_STATUS_LEN);
	
	return true;
}

static bool status_range_send(mesh_model_msg_ind_t const *p_rx_msg,
                              const light_lightness_range_status_params_t * p_params)
{
    light_lightness_range_status_msg_pkt_t msg_pkt;

    if (p_params->range_min < LIGHT_LIGHTNESS_RANGE_MIN ||
        p_params->range_max < LIGHT_LIGHTNESS_RANGE_MIN)
    {
        return false;
    }

	app_mesh_status_send_rsp(p_rx_msg, 
	                        LIGHT_LIGHTNESS_OPCODE_RANGE_STATUS, 
			                (uint8_t *)&msg_pkt, 
			                 LIGHT_LIGHTNESS_RANGE_STATUS_LEN);
	
	return true;
}



/* Utility functions */
static uint16_t clip_ll_actual_to_range(mesh_model_msg_ind_t const *p_rx_msg,
                                        uint16_t ll_actual)
{
    light_lightness_range_status_params_t range = {.status = 0};

//    p_s_server->settings.p_callbacks->light_lightness_cbs.range_get_cb(p_s_server,
//                                                                       p_rx_msg,
//                                                                       &range);

    ll_actual = light_lightness_utils_actual_to_range_restrict(ll_actual,
                                                         range.range_min,
                                                         range.range_max);

    return ll_actual;
}

static void actual_set_response(mesh_model_msg_ind_t const *p_rx_msg,
                               light_lightness_status_params_t * p_out_data)
{
    if (p_out_data)
    {
        status_actual_send(p_rx_msg, p_out_data);
    }
}

static void linear_set_response(mesh_model_msg_ind_t const *p_rx_msg,
                               light_lightness_status_params_t * p_out_data)
{
    light_lightness_linear_status_params_t out_linear_data = {0};

    if (p_out_data)
    {
        out_linear_data.present_lightness =
            light_lightness_utils_actual_to_linear(p_out_data->present_lightness);
        out_linear_data.target_lightness =
            light_lightness_utils_actual_to_linear(p_out_data->target_lightness);
        out_linear_data.remaining_time_ms = p_out_data->remaining_time_ms;
        (void) status_linear_send(p_rx_msg, &out_linear_data);
    }
}

static void handle_set(mesh_model_msg_ind_t const *p_rx_msg,
                       light_lightness_set_params_t * p_in_data,
                       light_lightness_status_params_t * p_out_data,
                       model_transition_t * p_transition,
                       lightness_set_response_t response)
{


    /* Clip the actual value to range (binding - 6.1.2.2.5) */
    p_in_data->lightness = clip_ll_actual_to_range(p_rx_msg,
                                                   p_in_data->lightness);

    /* Set clipped actual value */
//    p_s_server->settings.p_callbacks->light_lightness_cbs.set_cb(p_s_server,
//                                                                 &p_rx_msg->meta_data,
//                                                                 p_in_data,
//                                                                 p_transition,
//                                                                 p_out_data);
    response(p_rx_msg, p_out_data);
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
/**** Light Lightness server ****/
/** Opcode Handlers */
static void handle_actual_set(mesh_model_msg_ind_t const *p_rx_msg)
{
    U16 Opcode = BigtoLittle16(p_rx_msg->opcode);
	
    light_lightness_set_msg_pkt_t * p_msg_pkt;
    light_lightness_status_params_t out_data = {0};
    light_lightness_set_params_t in_data = {0};
    model_transition_t transition = {0};
    model_transition_t * p_transition;

    p_msg_pkt = (light_lightness_set_msg_pkt_t *) p_rx_msg->msg;
    if (!model_tid_validate(&tid_tracker,
                            p_rx_msg,
                            LIGHT_LIGHTNESS_OPCODE_SET,
                            p_msg_pkt->tid))
    {
        return;
    }

    p_transition = &transition;
    if (!handle_set_message_validate(LIGHT_LIGHTNESS_SET_MINLEN,
                                     LIGHT_LIGHTNESS_SET_MAXLEN,
                                     p_rx_msg->msg_len,
                                     p_msg_pkt->transition_time,
                                     p_msg_pkt->delay,
                                     &p_transition))
    {
        return;
    }

    in_data.lightness = p_msg_pkt->lightness;
    in_data.tid = p_msg_pkt->tid;

    handle_set(p_rx_msg,
               &in_data,
               (LIGHT_LIGHTNESS_OPCODE_SET == Opcode) ? &out_data : NULL,
               p_transition,
               actual_set_response);
}

static void handle_linear_set(mesh_model_msg_ind_t const *p_rx_msg)
{
    U16 Opcode = BigtoLittle16(p_rx_msg->opcode);
	
    light_lightness_linear_set_msg_pkt_t * p_msg_pkt;
    light_lightness_status_params_t out_data = {0};
    light_lightness_set_params_t in_data = {0};
    model_transition_t transition = {0};
    model_transition_t * p_transition;

    p_msg_pkt = (light_lightness_linear_set_msg_pkt_t *) p_rx_msg->msg;
    if (!model_tid_validate(&tid_tracker,
                            p_rx_msg,
                            LIGHT_LIGHTNESS_OPCODE_SET,
                            p_msg_pkt->tid))
    {
        return;
    }

    p_transition = &transition;
    if (!handle_set_message_validate(LIGHT_LIGHTNESS_LINEAR_SET_MINLEN,
                                     LIGHT_LIGHTNESS_LINEAR_SET_MAXLEN,
                                     p_rx_msg->msg_len,
                                     p_msg_pkt->transition_time,
                                     p_msg_pkt->delay,
                                     &p_transition))
    {
        return;
    }

    in_data.lightness = light_lightness_utils_linear_to_actual(p_msg_pkt->lightness);
    in_data.tid = p_msg_pkt->tid;

    handle_set(p_rx_msg,
               &in_data,
               (LIGHT_LIGHTNESS_OPCODE_LINEAR_SET == Opcode) ? &out_data : NULL,
               p_transition,
               linear_set_response);
}

static void handle_get(mesh_model_msg_ind_t const *p_rx_msg)
{

    light_lightness_status_params_t out_data = {0};

    if (p_rx_msg->msg_len == 0)
    {
//        p_s_server->settings.p_callbacks->light_lightness_cbs.get_cb(p_s_server,
//                                                                     &p_rx_msg->meta_data,
//                                                                     &out_data);
        status_actual_send(p_rx_msg, &out_data);
    }
}

static void handle_linear_get(mesh_model_msg_ind_t const *p_rx_msg)
{
    light_lightness_status_params_t out_data = {0};
    light_lightness_linear_status_params_t out_linear_data = {0};

    if (p_rx_msg->msg_len == 0)
    {
//        p_s_server->settings.p_callbacks->light_lightness_cbs.get_cb(p_s_server,
//                                                                     &p_rx_msg->meta_data,
//                                                                     &out_data);
        out_linear_data.present_lightness =
            light_lightness_utils_actual_to_linear(out_data.present_lightness);
        out_linear_data.target_lightness =
            light_lightness_utils_actual_to_linear(out_data.target_lightness);
        out_linear_data.remaining_time_ms = out_data.remaining_time_ms;
		
        status_linear_send(p_rx_msg, &out_linear_data);
    }
}

static void handle_last_get(mesh_model_msg_ind_t const *p_rx_msg)
{
    light_lightness_last_status_params_t out_data = {0};

    if (p_rx_msg->msg_len == 0)
    {
//        p_s_server->settings.p_callbacks->light_lightness_cbs.last_get_cb(p_s_server,
//                                                                          &p_rx_msg->meta_data,
//                                                                          &out_data);
        status_last_send(p_rx_msg, &out_data);
    }
}

static void handle_default_get(mesh_model_msg_ind_t const *p_rx_msg)
{
    light_lightness_default_status_params_t out_data = {0};

    if (p_rx_msg->msg_len == 0)
    {
//        p_s_server->settings.p_callbacks->light_lightness_cbs.default_get_cb(p_s_server,
//                                                                             &p_rx_msg->meta_data,
//                                                                             &out_data);

        status_default_send(p_rx_msg, &out_data);

    }
}

static void handle_range_get(mesh_model_msg_ind_t const *p_rx_msg)
{
    light_lightness_range_status_params_t out_data = {.status = 0};

    if (p_rx_msg->msg_len == 0)
    {
//        p_s_server->settings.p_callbacks->light_lightness_cbs.range_get_cb(p_s_server,
//                                                                           &p_rx_msg->meta_data,
//                                                                           &out_data);

        status_range_send(p_rx_msg, &out_data);
    }
}



/**************************************************************************************************/
/**** Setup Server */
/** Opcode Handlers */

static void handle_default_set(mesh_model_msg_ind_t const *p_rx_msg)
{
    U16 Opcode = BigtoLittle16(p_rx_msg->opcode);
	
    light_lightness_default_set_params_t in_data = {0};
    light_lightness_default_status_params_t out_data = {0};
    light_lightness_default_status_params_t * p_out_data = NULL;

    if (sizeof(light_lightness_default_set_msg_pkt_t) == p_rx_msg->msg_len)
    {
        light_lightness_default_set_msg_pkt_t * p_msg_params_packed =
            (light_lightness_default_set_msg_pkt_t *) p_rx_msg->msg;
        in_data.lightness = p_msg_params_packed->lightness;

        if (LIGHT_LIGHTNESS_OPCODE_DEFAULT_SET == Opcode)
        {
            p_out_data = &out_data;
        }

//        p_s_server->settings.p_callbacks->light_lightness_cbs.default_set_cb(p_s_server,
//                                                                             &p_rx_msg->meta_data,
//                                                                             &in_data,
//                                                                             p_out_data);
        if (p_out_data)
        {
            status_default_send(p_rx_msg, &out_data);
        }
    }
}

static void handle_range_set(mesh_model_msg_ind_t const *p_rx_msg)
{
    U16 Opcode = BigtoLittle16(p_rx_msg->opcode);
	
    light_lightness_range_set_params_t in_data = {0};
    light_lightness_range_status_params_t out_data = {.status = 0};
    light_lightness_range_status_params_t * p_out_data = NULL;

    if (sizeof(light_lightness_range_set_msg_pkt_t) == p_rx_msg->msg_len)
    {
        light_lightness_range_set_msg_pkt_t * p_msg_params_packed =
            (light_lightness_range_set_msg_pkt_t *) p_rx_msg->msg;
        in_data.range_min = p_msg_params_packed->range_min;
        in_data.range_max = p_msg_params_packed->range_max;

        /* Check for prohibited values @tagMeshMdlSp section 6.1.2.5.  For any
         * prohibited values, don't process, and don't respond */
        if ((in_data.range_min == 0) ||
            (in_data.range_min > in_data.range_max))
        {
            return;
        }

        if (LIGHT_LIGHTNESS_OPCODE_RANGE_SET == Opcode)
        {
            p_out_data = &out_data;
        }

//        p_s_server->settings.p_callbacks->light_lightness_cbs.range_set_cb(p_s_server,
//                                                                           &p_rx_msg->meta_data,
//                                                                           &in_data,
//                                                                           p_out_data);

        if (LIGHT_LIGHTNESS_OPCODE_RANGE_SET == Opcode)
        {
            status_range_send(p_rx_msg, &out_data);
        }
    }
}


static const access_opcode_handler_t m_opcode_handlers[] =
{
    {ACCESS_OPCODE_SIG(LIGHT_LIGHTNESS_OPCODE_GET), handle_get},
    {ACCESS_OPCODE_SIG(LIGHT_LIGHTNESS_OPCODE_SET), handle_actual_set},
    {ACCESS_OPCODE_SIG(LIGHT_LIGHTNESS_OPCODE_SET_UNACKNOWLEDGED), handle_actual_set},
    {ACCESS_OPCODE_SIG(LIGHT_LIGHTNESS_OPCODE_LINEAR_GET), handle_linear_get},
    {ACCESS_OPCODE_SIG(LIGHT_LIGHTNESS_OPCODE_LINEAR_SET), handle_linear_set},
    {ACCESS_OPCODE_SIG(LIGHT_LIGHTNESS_OPCODE_LINEAR_SET_UNACKNOWLEDGED), handle_linear_set},
    {ACCESS_OPCODE_SIG(LIGHT_LIGHTNESS_OPCODE_LAST_GET), handle_last_get},
    {ACCESS_OPCODE_SIG(LIGHT_LIGHTNESS_OPCODE_DEFAULT_GET), handle_default_get},
    {ACCESS_OPCODE_SIG(LIGHT_LIGHTNESS_OPCODE_RANGE_GET), handle_range_get},
	
	{ACCESS_OPCODE_SIG(LIGHT_LIGHTNESS_OPCODE_DEFAULT_SET), handle_default_set},
    {ACCESS_OPCODE_SIG(LIGHT_LIGHTNESS_OPCODE_DEFAULT_SET_UNACKNOWLEDGED), handle_default_set},
    {ACCESS_OPCODE_SIG(LIGHT_LIGHTNESS_OPCODE_RANGE_SET), handle_range_set},
    {ACCESS_OPCODE_SIG(LIGHT_LIGHTNESS_OPCODE_RANGE_SET_UNACKNOWLEDGED), handle_range_set},
};

/*****************************************************************************
Function name :  lightness_server_msg_handler
Description:	 Light Lightness Server / Light Lightness Setup Server 消息处理函数
Input:	         p_rx_msg->消息内容
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void lightness_server_msg_handler(mesh_model_msg_ind_t const *p_rx_msg)
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


