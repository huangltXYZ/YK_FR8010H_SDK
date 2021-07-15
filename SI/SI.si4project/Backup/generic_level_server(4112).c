#include "YK_APP.h"





static bool status_send(mesh_model_msg_ind_t const *p_rx_msg, const generic_level_status_params_t * p_params)
{
    generic_level_status_msg_pkt_t msg_pkt;

    msg_pkt.present_level = p_params->present_level;
    if (p_params->remaining_time_ms > 0)
    {
        msg_pkt.target_level = p_params->target_level;
        msg_pkt.remaining_time = model_transition_time_encode(p_params->remaining_time_ms);
    }
	
	app_mesh_status_send_rsp(p_rx_msg, 
	                         GENERIC_LEVEL_OPCODE_STATUS,
				             (uint8_t *)&msg_pkt, 
				             p_params->remaining_time_ms > 0 ? GENERIC_LEVEL_STATUS_MAXLEN : GENERIC_LEVEL_STATUS_MINLEN);
	
	return true;

}


/** Opcode Handlers */

static void handle_set(mesh_model_msg_ind_t const *p_rx_msg)
{
    U16 Opcode = BigtoLittle16(p_rx_msg->opcode);
	
    generic_level_set_params_t in_data = {0};
    model_transition_t in_data_tr = {0};
    generic_level_status_params_t out_data = {0};

    if (p_rx_msg->msg_len == GENERIC_LEVEL_SET_MINLEN || p_rx_msg->msg_len == GENERIC_LEVEL_SET_MAXLEN)
    {
        generic_level_set_msg_pkt_t * p_msg_params_packed = (generic_level_set_msg_pkt_t *) p_rx_msg->msg;
        in_data.level = p_msg_params_packed->level;
        in_data.tid = p_msg_params_packed->tid;

        if (p_rx_msg->msg_len == GENERIC_LEVEL_SET_MAXLEN)
        {
            if (!model_transition_time_is_valid(p_msg_params_packed->transition_time))
            {
                return;
            }

            in_data_tr.transition_time_ms = model_transition_time_decode(p_msg_params_packed->transition_time);
            in_data_tr.delay_ms = model_delay_decode(p_msg_params_packed->delay);
        }

//        p_server->settings.p_callbacks->level_cbs.set_cb(p_server,
//                                                        &p_rx_msg->meta_data, &in_data,
//                                                        (p_rx_msg->msg_len == GENERIC_LEVEL_SET_MINLEN) ? NULL : &in_data_tr,
//                                                        (p_rx_msg->opcode.opcode == GENERIC_LEVEL_OPCODE_SET) ? &out_data : NULL);

        if (Opcode == GENERIC_LEVEL_OPCODE_SET)
        {
            status_send(p_rx_msg, &out_data);
        }
    }
}

static void handle_delta_set(mesh_model_msg_ind_t const *p_rx_msg)
{
	U16 Opcode = BigtoLittle16(p_rx_msg->opcode);
	int32_t target, delta;	
    uint16_t last_level;
    
    generic_level_delta_set_params_t in_data = {0};
    model_transition_t in_data_tr = {0};
    generic_level_status_params_t out_data = {0};

    if (p_rx_msg->msg_len == GENERIC_LEVEL_DELTA_SET_MINLEN || p_rx_msg->msg_len == GENERIC_LEVEL_DELTA_SET_MAXLEN)
    {
        generic_level_delta_set_msg_pkt_t * p_msg_params_packed = (generic_level_delta_set_msg_pkt_t *) p_rx_msg->msg;
        in_data.delta_level = p_msg_params_packed->delta_level;
        in_data.tid = p_msg_params_packed->tid;

        if (p_rx_msg->msg_len == GENERIC_LEVEL_DELTA_SET_MAXLEN)
        {
            if (!model_transition_time_is_valid(p_msg_params_packed->transition_time))
            {
                return;
            }

            in_data_tr.transition_time_ms = model_transition_time_decode(p_msg_params_packed->transition_time);
            in_data_tr.delay_ms = model_delay_decode(p_msg_params_packed->delay);
            MESH_MODEL_LOG("transition_time_ms = %d delay_ms = %d\r\n", in_data_tr.transition_time_ms, in_data_tr.delay_ms);
        }
        delta = in_data.delta_level % ((int32_t)UINT16_MAX + 1);
	
	    last_level = (int16_t) Light_Status_Get_Target(LIGHT_STATUS_TYPE_LIGHT_LEVEL);
	    target = last_level + delta;

        Light_Status_Set_Target(LIGHT_STATUS_TYPE_LIGHT_LEVEL, &target);
        
        APP_Light_SetLevel(U16 TargetLevel, U16 usTransitionTime);

        if (Opcode == GENERIC_LEVEL_OPCODE_DELTA_SET)
        {
            status_send(p_rx_msg, &out_data);
        }
    }
}

static void handle_move_set(mesh_model_msg_ind_t const *p_rx_msg)
{
	U16 Opcode = BigtoLittle16(p_rx_msg->opcode);
	
    generic_level_move_set_params_t in_data = {0};
    model_transition_t in_data_tr = {0};
    generic_level_status_params_t out_data = {0};

    if (p_rx_msg->msg_len == GENERIC_LEVEL_MOVE_SET_MINLEN || p_rx_msg->msg_len == GENERIC_LEVEL_MOVE_SET_MAXLEN)
    {
        generic_level_move_set_msg_pkt_t * p_msg_params_packed = (generic_level_move_set_msg_pkt_t *) p_rx_msg->msg;
        in_data.move_level = p_msg_params_packed->move_level;
        in_data.tid = p_msg_params_packed->tid;

        if (p_rx_msg->msg_len == GENERIC_LEVEL_MOVE_SET_MAXLEN)
        {
            if (!model_transition_time_is_valid(p_msg_params_packed->transition_time))
            {
                return;
            }

            in_data_tr.transition_time_ms = model_transition_time_decode(p_msg_params_packed->transition_time);
            in_data_tr.delay_ms = model_delay_decode(p_msg_params_packed->delay);
            
            MESH_MODEL_LOG("transition_time_ms = %d delay_ms = %d\r\n", in_data_tr.transition_time_ms, in_data_tr.delay_ms);
        }
        LightStatus.lightness.delta = in_data.move_level;
        MESH_MODEL_LOG("move_level = %d", in_data.move_level);

        if (Opcode == GENERIC_LEVEL_OPCODE_MOVE_SET)
        {
            out_data.present_level = in_data.move_level;
            
            status_send(p_rx_msg, &out_data);
        }   
    }
}

static void handle_get(mesh_model_msg_ind_t const *p_rx_msg)
{
    generic_level_status_params_t out_data = {0};

    if (p_rx_msg->msg_len == 0)
    {
        out_data.present_level = Light_Status_Get_Current(LIGHT_STATUS_TYPE_LIGHT_LEVEL);
        out_data.target_level = Light_Status_Get_Target(LIGHT_STATUS_TYPE_LIGHT_LEVEL);
        out_data.remaining_time_ms = 0;
		
        status_send(p_rx_msg, &out_data);
    }
}

static const access_opcode_handler_t m_opcode_handlers[] =
{
    {ACCESS_OPCODE_SIG(GENERIC_LEVEL_OPCODE_GET), handle_get},
    {ACCESS_OPCODE_SIG(GENERIC_LEVEL_OPCODE_SET), handle_set},
    {ACCESS_OPCODE_SIG(GENERIC_LEVEL_OPCODE_SET_UNACKNOWLEDGED), handle_set},
    {ACCESS_OPCODE_SIG(GENERIC_LEVEL_OPCODE_DELTA_SET), handle_delta_set},
    {ACCESS_OPCODE_SIG(GENERIC_LEVEL_OPCODE_DELTA_SET_UNACKNOWLEDGED), handle_delta_set},
    {ACCESS_OPCODE_SIG(GENERIC_LEVEL_OPCODE_MOVE_SET), handle_move_set},
    {ACCESS_OPCODE_SIG(GENERIC_LEVEL_OPCODE_MOVE_SET_UNACKNOWLEDGED), handle_move_set}
};


/*****************************************************************************
Function name :  generic_level_server_msg_handler
Description:	 Generic Level Server 消息处理函数
Input:	         p_rx_msg->消息内容
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void generic_level_server_msg_handler(mesh_model_msg_ind_t const *p_rx_msg)
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


