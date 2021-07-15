

#include "YK_APP.h"

 /** Tid tracker structure. */
static tid_tracker_t tid_tracker;
	
	
static bool status_send(mesh_model_msg_ind_t const *p_rx_msg, const generic_onoff_status_params_t *p_params)
{
	generic_onoff_status_msg_pkt_t msg_pkt;

    if(p_params->present_on_off > GENERIC_ONOFF_MAX ||
        p_params->target_on_off  > GENERIC_ONOFF_MAX ||
        p_params->remaining_time_ms > TRANSITION_TIME_STEP_10M_MAX)
    {
        return false;
    }

    msg_pkt.present_on_off = p_params->present_on_off;
    if (p_params->remaining_time_ms > 0)
    {
        msg_pkt.target_on_off = p_params->target_on_off;
        msg_pkt.remaining_time = model_transition_time_encode(p_params->remaining_time_ms);
    }
	
	APP_MESH_Status_Send(p_rx_msg, 
				             GENERIC_ONOFF_OPCODE_STATUS, 
				             (uint8_t *)&msg_pkt, 
				             p_params->remaining_time_ms > 0 ? GENERIC_ONOFF_STATUS_MAXLEN : GENERIC_ONOFF_STATUS_MINLEN);
	
	return true;						
}
/*****************************************************************************
Function name :  set_params_validate
Description:	 on off消息参数合法性校验
Input:	         p_rx_msg->消息内容 p_params->消息参数
Return:          true->校验通过;false->校验错误
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
static inline bool set_params_validate(const mesh_model_msg_ind_t * p_rx_msg, const generic_onoff_set_msg_pkt_t * p_params)
{
    return (
            (p_rx_msg->msg_len == GENERIC_ONOFF_SET_MINLEN || p_rx_msg->msg_len == GENERIC_ONOFF_SET_MAXLEN) &&
            (p_params->on_off <= GENERIC_ONOFF_MAX)
           );
}

static void handle_set(mesh_model_msg_ind_t const *p_rx_msg)
{
    U16 Opcode = BigtoLittle16(p_rx_msg->opcode);
	
    generic_onoff_set_params_t in_data = {0};
    model_transition_t in_data_tr = {0};
    generic_onoff_status_params_t out_data = {0};
    generic_onoff_set_msg_pkt_t * p_msg_params_packed = (generic_onoff_set_msg_pkt_t *) p_rx_msg->msg;

    if (set_params_validate(p_rx_msg, p_msg_params_packed))
    {
        in_data.on_off = p_msg_params_packed->on_off;
        in_data.tid = p_msg_params_packed->tid;

        if(model_tid_validate(&tid_tracker, p_rx_msg, GENERIC_ONOFF_OPCODE_SET, in_data.tid))
        {
            if (p_rx_msg->msg_len == GENERIC_ONOFF_SET_MAXLEN)
            {
                if (!model_transition_time_is_valid(p_msg_params_packed->transition_time))
                {
                    return;
                }

                in_data_tr.transition_time_ms = model_transition_time_decode(p_msg_params_packed->transition_time);
                in_data_tr.delay_ms = model_delay_decode(p_msg_params_packed->delay);
				
                MESH_MODEL_LOG("transition_time_ms = %d delay_ms = %d\r\n", in_data_tr.transition_time_ms, in_data_tr.delay_ms);
            }

			/*应用层处理*/
            MESH_MODEL_LOG("on_off = %d\r\n", in_data.on_off);
            Light_Status_Set_Target(LIGHT_STATUS_TYPE_ONOFF, &in_data.on_off);
          
            APP_Light_SetOnOff(in_data.on_off, APP_Light_TRANSITION_TIME);

            if(Opcode == GENERIC_ONOFF_OPCODE_SET)
            {
		        out_data.present_on_off    = Light_Status_Get_Current(LIGHT_STATUS_TYPE_ONOFF);
		        out_data.target_on_off     = Light_Status_Get_Target(LIGHT_STATUS_TYPE_ONOFF);
		        out_data.remaining_time_ms = 0;
				
                status_send(p_rx_msg, &out_data);
            }
        }
    }
}


static inline bool get_params_validate(mesh_model_msg_ind_t const *p_rx_msg)
{
    return (p_rx_msg->msg_len == 0);
}

static void handle_get(mesh_model_msg_ind_t const *p_rx_msg)
{
    generic_onoff_status_params_t out_data = {0};

    if (get_params_validate(p_rx_msg))
    {
		out_data.present_on_off    = Light_Status_Get_Current(LIGHT_STATUS_TYPE_ONOFF);
		out_data.target_on_off     = Light_Status_Get_Target(LIGHT_STATUS_TYPE_ONOFF);
		out_data.remaining_time_ms = 0;
		
        status_send(p_rx_msg, &out_data);
    }
}

static const access_opcode_handler_t m_opcode_handlers[] =
{
    {ACCESS_OPCODE_SIG(GENERIC_ONOFF_OPCODE_SET), handle_set},
    {ACCESS_OPCODE_SIG(GENERIC_ONOFF_OPCODE_SET_UNACKNOWLEDGED), handle_set},
    {ACCESS_OPCODE_SIG(GENERIC_ONOFF_OPCODE_GET), handle_get},
};


/*****************************************************************************
Function name :  generic_onoff_server_msg_handler
Description:	 Generic OnOff Server 消息处理函数
Input:	         p_rx_msg->消息内容
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void generic_onoff_server_msg_handler(mesh_model_msg_ind_t const *p_rx_msg)
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
