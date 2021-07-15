

#include "YK_APP.h"

#define TRANSITION_TIME_STEP_RESOLUTION_100MS   (0x00)
#define TRANSITION_TIME_STEP_RESOLUTION_1S      (0x40)
#define TRANSITION_TIME_STEP_RESOLUTION_10S     (0x80)
#define TRANSITION_TIME_STEP_RESOLUTION_10M     (0xC0)

#define TRANSITION_TIME_STEP_MASK               (0xC0)
#define TRANSITION_TIME_STEP_100MS_FACTOR       (100)
#define TRANSITION_TIME_STEP_1S_FACTOR          (1000)
#define TRANSITION_TIME_STEP_10S_FACTOR         (10*1000)
#define TRANSITION_TIME_STEP_10M_FACTOR         (10*60*1000)

/* As defined in @tagMeshMdlSp */
#define TID_VALIDATION_INTERVAL_US              (TmrSeconds(6))

/* This macro ensures that the remaining timeout is always greater than APP_TIMER_MIN_TIMEOUT_TICKS.*/
#define APP_TIMER_MAX_TIMEOUT                   (MODEL_TIMER_MAX_TIMEOUT_TICKS - (APP_TIMER_MIN_TIMEOUT_TICKS * 2))

static volatile U32 s_ulTick = 0;



static bool Check_Tid_Time(U32 ulTick)
{
	if(ulTick <= s_ulTick)
	{
		if((s_ulTick - ulTick) >= TID_VALIDATION_INTERVAL_US)
		{
			return true;
		}
		else
		{
		    return false;
		}
	}
	else
	{
		if((s_ulTick + INFINITE - ulTick) >= TID_VALIDATION_INTERVAL_US)
		{
			return true;
		}
		else
		{
		    return false;
		}
	}
}


uint32_t model_transition_time_decode(uint8_t enc_transition_time)
{
    uint32_t time = 0;

    if ((enc_transition_time & ~TRANSITION_TIME_STEP_MASK) == TRANSITION_TIME_UNKNOWN)
    {
        return MODEL_TRANSITION_TIME_UNKNOWN;
    }

    switch(enc_transition_time & TRANSITION_TIME_STEP_MASK)
    {
        case TRANSITION_TIME_STEP_RESOLUTION_100MS:
            time = (enc_transition_time & ~TRANSITION_TIME_STEP_MASK) * TRANSITION_TIME_STEP_100MS_FACTOR;
        break;

        case TRANSITION_TIME_STEP_RESOLUTION_1S:
            time = (enc_transition_time & ~TRANSITION_TIME_STEP_MASK) * TRANSITION_TIME_STEP_1S_FACTOR;
        break;

        case TRANSITION_TIME_STEP_RESOLUTION_10S:
            time = (enc_transition_time & ~TRANSITION_TIME_STEP_MASK) * TRANSITION_TIME_STEP_10S_FACTOR;
        break;

        case TRANSITION_TIME_STEP_RESOLUTION_10M:
            time = (enc_transition_time & ~TRANSITION_TIME_STEP_MASK) * TRANSITION_TIME_STEP_10M_FACTOR;
        break;

        default:
            break;
    }

    return time;
}


uint8_t model_transition_time_encode(uint32_t transition_time)
{
    uint8_t enc_time = TRANSITION_TIME_UNKNOWN;

    if(transition_time <= TRANSITION_TIME_STEP_100MS_MAX)
    {
        enc_time = (transition_time / TRANSITION_TIME_STEP_100MS_FACTOR) | TRANSITION_TIME_STEP_RESOLUTION_100MS;
    }
    else if (transition_time <= TRANSITION_TIME_STEP_1S_MAX)
    {
        enc_time = (transition_time / TRANSITION_TIME_STEP_1S_FACTOR) | TRANSITION_TIME_STEP_RESOLUTION_1S;
    }
    else if (transition_time <= TRANSITION_TIME_STEP_10S_MAX)
    {
        enc_time = (transition_time / TRANSITION_TIME_STEP_10S_FACTOR) | TRANSITION_TIME_STEP_RESOLUTION_10S;
    }
    else if (transition_time <= TRANSITION_TIME_STEP_10M_MAX)
    {
        enc_time = (transition_time / TRANSITION_TIME_STEP_10M_FACTOR) | TRANSITION_TIME_STEP_RESOLUTION_10M;
    }

    return enc_time;
}

bool model_transition_time_is_valid(uint8_t enc_transition_time)
{
    return ((enc_transition_time & ~TRANSITION_TIME_STEP_MASK) != TRANSITION_TIME_UNKNOWN);
}

uint32_t model_delay_decode(uint8_t enc_delay)
{
    return (enc_delay * DELAY_TIME_STEP_FACTOR_MS);
}

uint8_t model_delay_encode(uint32_t delay)
{
    if (delay > DELAY_TIME_STEP_MAX)
    {
        return DELAY_TIME_STEP_MAX;
    }
    else
    {
        return (delay/DELAY_TIME_STEP_FACTOR_MS);
    }
}


/* Public APIs for models */
bool model_tid_validate(tid_tracker_t * p_tid_tracker, mesh_model_msg_ind_t const *p_rx_msg,
                        uint32_t message_id, uint8_t tid)
{
    if (p_tid_tracker->src != p_rx_msg->src ||
        //p_tid_tracker->dst != p_meta->dst.value ||
        p_tid_tracker->old_tid != tid ||
        p_tid_tracker->message_id != message_id ||
        Check_Tid_Time(p_tid_tracker->ulTime) == true)
    {
        p_tid_tracker->src = p_rx_msg->src;
        //p_tid_tracker->dst = p_meta->dst.value;
        p_tid_tracker->message_id = message_id;
        p_tid_tracker->old_tid = tid;

        p_tid_tracker->ulTime = s_ulTick;

        p_tid_tracker->new_transaction = true;
    }
    else
    {
		MESH_MODEL_LOG("Tid Old\r\n");
        p_tid_tracker->new_transaction = false;
    }

    return p_tid_tracker->new_transaction; 

  
}


bool handle_set_message_validate(uint16_t short_message_bytes,
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


/*****************************************************************************
Function name :  Mesh_Tid_TimeProc
Description:	 TID有效性检查定时器
Input:	         ulPeriod->函数被调用的周期（ms）		
Return:          void
Notes: 
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void Mesh_Tid_TimeProc(U32 ulPeriod)
{
	s_ulTick += ulPeriod;
}

