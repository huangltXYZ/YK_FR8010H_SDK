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
	
	APP_MESH_Status_Send(p_rx_msg, 
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
            MESH_MODEL_LOG("transition_time_ms = %d delay_ms = %d\r\n", in_data_tr.transition_time_ms, in_data_tr.delay_ms);
        }
        
        switch(p_rx_msg->element)
        {
            case MESH_ELMENT_MAIN:
            {
                Light_Status_Set_Target(LIGHT_STATUS_TYPE_LIGHT_LEVEL, &in_data.level);
                APP_Light_SetLevel(LIGHTNESS_TO_LEVEL(LightStatus.lightness.target), APP_Light_TRANSITION_TIME);
        
            }break;
        
            case MESH_ELMENT_TEMP:
            {
                Light_Status_Set_Target(LIGHT_STATUS_TYPE_TEMP_LEVEL, &in_data.level);
                APP_Light_SetTemperature(LightStatus.temp.target, LightStatus.temp.delta_target, APP_Light_TRANSITION_TIME);
        
            }break;
            
            case MESH_ELMENT_HUE:
            {
                Light_Status_Set_Target(LIGHT_STATUS_TYPE_HUE_LEVEL, &in_data.level);
                APP_Light_SetHSL(LightStatus.hue.target   , LightStatus.saturation.target , LightStatus.lightness.target, APP_Light_TRANSITION_TIME);
        
            }break;
        
            case MESH_ELMENT_SATURATION:
            {
                Light_Status_Set_Target(LIGHT_STATUS_TYPE_SATURATION_LEVEL, &in_data.level);
                APP_Light_SetHSL(LightStatus.hue.target   , LightStatus.saturation.target , LightStatus.lightness.target, APP_Light_TRANSITION_TIME);
        
            }break;
        
            default:
                break;   
        }
        
		MESH_MODEL_LOG("level = %d\r\n", in_data.level);
        
        if(Opcode == GENERIC_LEVEL_OPCODE_SET)
        {
			out_data.present_level = in_data.level;
			out_data.target_level = in_data.level;
			out_data.remaining_time_ms = 0;
			
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
        switch(p_rx_msg->element)
        {
            case MESH_ELMENT_MAIN:
            {
                delta = in_data.delta_level % ((int32_t)UINT16_MAX + 1);
	
        	    last_level = (int16_t) Light_Status_Get_Target(LIGHT_STATUS_TYPE_LIGHT_LEVEL);
        	    target = last_level + delta;
        
                Light_Status_Set_Target(LIGHT_STATUS_TYPE_LIGHT_LEVEL, &target);
                
                APP_Light_SetLevel(LIGHTNESS_TO_LEVEL(target), APP_Light_TRANSITION_TIME);

                out_data.present_level = Light_Status_Get_Current(LIGHT_STATUS_TYPE_LIGHT_LEVEL);
                out_data.target_level = Light_Status_Get_Target(LIGHT_STATUS_TYPE_LIGHT_LEVEL);
                out_data.remaining_time_ms = 0;
        
            }break;
        
            case MESH_ELMENT_TEMP:
            {
        
            }break;
            
            case MESH_ELMENT_HUE:
            {
        
            }break;
        
            case MESH_ELMENT_SATURATION:
            {
        
            }break;
        
            default:
                break;
        
        }
                
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
        
        switch(p_rx_msg->element)
        {
            case MESH_ELMENT_MAIN:
            {
                LightStatus.lightness.delta = in_data.move_level;
        
            }break;
        
            case MESH_ELMENT_TEMP:
            {
        
            }break;
            
            case MESH_ELMENT_HUE:
            {
        
            }break;
        
            case MESH_ELMENT_SATURATION:
            {
        
            }break;
        
            default:
                break;
        
        }

        Light_Status_Save();
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

    if(p_rx_msg->msg_len == 0)
    {
        switch(p_rx_msg->element)
        {
            case MESH_ELMENT_MAIN:
            {
                out_data.present_level = Light_Status_Get_Current(LIGHT_STATUS_TYPE_LIGHT_LEVEL);
                out_data.target_level = Light_Status_Get_Target(LIGHT_STATUS_TYPE_LIGHT_LEVEL);
                out_data.remaining_time_ms = 0;

            }break;

            case MESH_ELMENT_TEMP:
            {
                out_data.present_level = Light_Status_Get_Current(LIGHT_STATUS_TYPE_TEMP_LEVEL);
                out_data.target_level = Light_Status_Get_Target(LIGHT_STATUS_TYPE_TEMP_LEVEL);
                out_data.remaining_time_ms = 0;                

            }break;
            
            case MESH_ELMENT_HUE:
            {
                out_data.present_level = Light_Status_Get_Current(LIGHT_STATUS_TYPE_HUE_LEVEL);
                out_data.target_level = Light_Status_Get_Target(LIGHT_STATUS_TYPE_HUE_LEVEL);
                out_data.remaining_time_ms = 0;   

            }break;

            case MESH_ELMENT_SATURATION:
            {
                out_data.present_level = Light_Status_Get_Current(LIGHT_STATUS_TYPE_SATURATION_LEVEL);
                out_data.target_level = Light_Status_Get_Target(LIGHT_STATUS_TYPE_SATURATION_LEVEL);
                out_data.remaining_time_ms = 0;   

            }break;

            default:
                break;

        }

		MESH_MODEL_LOG("present_level = %d, target_level = %d, remaining_time_ms = %d\r\n", out_data.present_level, out_data.target_level, out_data.remaining_time_ms);
        
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


