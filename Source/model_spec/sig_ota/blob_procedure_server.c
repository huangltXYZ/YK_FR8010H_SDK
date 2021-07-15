#include "YK_APP.h"

typedef enum
{
    BLOB_TRANSFER_START  =  0x01,
    BLOB_BLOCK_START     =  0x02,
    PHASE_TIMEOUT        =  0x03,
    BLOB_CHUNK_MISSING   =  0x04,
    BLOB_TRANSFER_CANCEL =  0x05,
    USER_SEPCIFIED       =  0x06
} TRANSFER_PHASE_TYPE_T;



typedef struct
{
    TRANSFER_PHASE_TYPE_T phase_type;
    void *arg;
} transfer_phase_event;


void receive_blob_procedure_start(blob_transfer_server_t *p_server, uint64_t blob_id)
{
    p_server->proc.exp_blob_id = blob_id;
    p_server->proc.phase_state = IDLE;
    p_server->proc.cap_states.max_block_size_log = MAX_BLOCK_SIZE_LOG;
    p_server->proc.cap_states.min_block_size_log = MIN_BLOCK_SIZE_LOG;
    p_server->proc.cap_states.server_mtu_size = MAX_MTU_SIZE;
    p_server->proc.cap_states.max_blob_size = MAX_BLOB_SIZE;
    p_server->proc.cap_states.max_chunks_number = MAX_CHUNKS_NUMBER;
    p_server->proc.cap_states.max_chunks_size = MAX_CHUNK_SIZE;
}

//cancel pendig BLOB transfer
void cancel_blod_reception_procedure(blob_transfer_server_t *p_server)
{
    p_server->proc.blob_state.blob_id = 0;
    p_server->proc.phase_state = INACTIVE;
}


void resume_blob_reception_procedure(void)
{

}

static void blob_transfer_server_phase_transitions(blob_transfer_server_t *p_server, transfer_phase_event *p_event)
{
    switch(p_event->phase_type)
    {
        case BLOB_TRANSFER_START:
        {
            if ((p_server->proc.phase_state == WAITING_N_CHUNK)
                    || (p_server->proc.phase_state == COMPLETE))
            {
                // MMDL/SR/BT/BV-15-C (BLOB Transfer Start-IUT in Waiting for Next Chunk Phase): if it's wait Chunk, keep BLOB_TRANS_PHASE_WAIT_NEXT_CHUNK
            }
            else
            {
                p_server->proc.phase_state = WAITING_N_BLOCK;
            }

            p_server->proc.blob_block_state.block_number = 0;
            MESH_MODEL_LOG("BLOB_TRANSFER_START\n");
            p_server->proc.received_blocks = 0;
            if(p_server->proc.phase_state == IDLE)
            {

            }
        } break;
        case BLOB_BLOCK_START:
        {
            p_server->proc.phase_state = WAITING_N_CHUNK;
            blob_block_start_param_t *p_msg = (blob_block_start_param_t *)p_event->arg;
            p_server->proc.blob_block_state.block_size = (1 << p_server->proc.blob_state.block_size_log);
            p_server->proc.blob_block_state.chunk_size = p_msg->chunk_size;
            p_server->proc.blob_block_state.block_number = p_msg->block_number;
            MESH_MODEL_LOG("p_server->proc.blob_block_state.block_number = %04x,\
	p_msg->block_number = %04x\n",
                           p_server->proc.blob_block_state.block_number, p_msg->block_number);

            if (p_server->proc.blob_block_state.block_number < p_server->proc.blob_state.total_blocks - 1)
            {
                //not last block
                memset(p_server->proc.received_chunks, 0xFF, sizeof(p_server->proc.received_chunks));
            }
            else
            {
                //last block
                uint16_t tail_block_size = p_server->proc.blob_state.blob_size %
                                           (1 << p_server->proc.blob_state.block_size_log);
                if (tail_block_size == 0)
                {
                    memset(p_server->proc.received_chunks, 0xFF, sizeof(p_server->proc.received_chunks));
                }
                else
                {
                    uint8_t chunks, i, j;
                    p_server->proc.blob_block_state.block_size = tail_block_size;
                    chunks = (tail_block_size + (p_server->proc.blob_block_state.chunk_size - 1)) /
                             p_server->proc.blob_block_state.chunk_size;
                    memset(p_server->proc.received_chunks, 0, sizeof(p_server->proc.received_chunks));

                    i = (chunks - 1) / 8;
                    j = (chunks - 1) % 8 ;
                    memset(p_server->proc.received_chunks, 0xFF, i);
                    for(uint8_t k = 0; k < j+1; k++)
                    {
                        p_server->proc.received_chunks[i] |= 1<<k;
                    }


                }
            }

        }
        break;
        case BLOB_CHUNK_MISSING:
        {
            uint8_t state = *(uint8_t *)(p_event->arg);
            if(state == CHUNK_FINISHED)
            {
                p_server->proc.received_blocks = (1 + p_server->proc.blob_block_state.block_number);
                MESH_MODEL_LOG("p_server->proc.received_blocks = %04x\n",p_server->proc.received_blocks );
                p_server->proc.phase_state = WAITING_N_BLOCK;
            }
            else if(state == CHUNK_BLOACK_FINISHED)
            {
                p_server->proc.received_blocks = (1 + p_server->proc.blob_block_state.block_number);
                MESH_MODEL_LOG("p_server->proc.received_blocks = %04x\n",p_server->proc.received_blocks );
                p_server->proc.phase_state = COMPLETE;
                //app_timer_stop(m_blob_server_timer);
            }
        }
        break;
        default:
            break;
    }

}

static uint8_t blob_transfer_all_condiation_is_met(blob_state_t *p_blob, transfer_cap_state_t *p_tcap)
{
    if(p_blob->blob_size > p_tcap->tr_max_blob_size)
    {
        MESH_MODEL_LOG("ST_BLOB_STORANGE_LIMIT\n");
        return ST_BLOB_STORANGE_LIMIT;
    }
    //else if(p_blob->block_size_log < p_tcap->)//TODO waiting for the BLOB sepecification updating to next vesion

    else if(p_blob->block_size_log > p_tcap->tr_max_block_size_log)
    {
        MESH_MODEL_LOG("ST_BLOB_WRONG_BLOCK_SIZE\n");
        return ST_BLOB_WRONG_BLOCK_SIZE;
    }

    else if(p_blob->total_blocks > p_tcap->tr_max_total_blocks)
    {
        MESH_MODEL_LOG("ST_BLOB_INVALID_PARAM\n");
        return ST_BLOB_INVALID_PARAM;
    }

    MESH_MODEL_LOG("ST_BLOB_SUCCESS\n");
    return ST_BLOB_SUCCESS;

}
static void computer_for_transfer_capbilities(transfer_cap_state_t *p_tcap, blob_server_cap_state_t *p_cap)
{
//	p_tcap->tr_max_chunks_size = p_cap->max_chunks_size > (p_tcap->tr_mtu_size - 3)?
//									(p_tcap->tr_mtu_size - 3):p_cap->max_chunks_size;
    p_tcap->tr_max_chunks_size = p_cap->max_chunks_size > (p_tcap->tr_mtu_size)?
                                 (p_tcap->tr_mtu_size):p_cap->max_chunks_size;

//	p_tcap->tr_max_chunks_number = p_cap->max_chunks_number > 8*(p_tcap->tr_mtu_size - 6)?
//									8*(p_tcap->tr_mtu_size - 6):p_cap->max_chunks_number;
    p_tcap->tr_max_chunks_number = p_cap->max_chunks_number;


//	p_tcap->tr_max_total_blocks = 8 * (p_tcap->tr_mtu_size - 20);

    p_tcap->tr_max_total_blocks = p_cap->max_blob_size/(1<<p_cap->max_block_size_log);

//	p_tcap->tr_max_block_size_log = (p_tcap->tr_max_chunks_number*p_tcap->tr_max_chunks_number) > pow(2,p_cap->max_block_size_log)?
//									 pow(2,p_cap->max_block_size_log):(p_tcap->tr_max_chunks_number*p_tcap->tr_max_chunks_number);
    p_tcap->tr_max_block_size_log = p_cap->max_block_size_log;
//	p_tcap->tr_max_total_blocks = (p_tcap->tr_max_total_blocks*pow(2,p_tcap->tr_max_block_size_log)) > p_cap->max_blob_size?
//									p_cap->max_blob_size:(p_tcap->tr_max_total_blocks*pow(2,p_tcap->tr_max_block_size_log));

    MESH_MODEL_LOG(
        "tr_max_chunks_size/tr_max_chunks_number/tr_max_total_blocks/tr_max_block_size_log = %d/%d/%d/%d\n",
        p_tcap->tr_max_chunks_size,
        p_tcap->tr_max_chunks_number,
        p_tcap->tr_max_total_blocks,
        p_tcap->tr_max_block_size_log);
}

//static uint16_t minimum_mtu_size(uint16_t value1,uint16_t value2)
//{
//	if(value1 < value2)
//		return value1;
//	else
//		return value2;
//}


static void chunk_received_set(blob_transfer_server_t * p_server,uint8_t chunk_index)
{
    uint8_t i,j;
    i = chunk_index / 8;
    j = chunk_index % 8 ;
    p_server->proc.received_chunks[i] &= ~(1<<j);
}


/* if all chunks has been received, return true; else return false */
static bool last_missing_chunk_check(blob_transfer_server_t * p_server)
{
    uint16_t block_size = 1 << p_server->proc.blob_state.block_size_log;
    uint16_t chunk_size = p_server->proc.blob_block_state.chunk_size;
    uint8_t chunk_number = (block_size + chunk_size - 1)  / chunk_size;

    p_server->proc.current_chunks = chunk_number;
    uint8_t j,k;
    uint16_t i = 0;
    for(; i < chunk_number; i++)
    {
        j = i / 8;
        k = i % 8;
        if((p_server->proc.received_chunks[j] & (1<<k)) != 0)
        {
            return false;
        }
    }
    return true;

}

static bool last_block_check(blob_transfer_server_t * p_server)
{
    blob_state_t *p_blob_state = &p_server->proc.blob_state;
    blob_block_t *p_block_state = &p_server->proc.blob_block_state;
    if(p_block_state->block_number == p_blob_state->total_blocks - 1)
        return true;
    else
        return false;
}

static bool is_chunk_received(blob_transfer_server_t * p_server,uint8_t chunk_index)
{
    uint8_t j,k;
    j = chunk_index / 8;
    k = chunk_index % 8;
    if((p_server->proc.received_chunks[j] & (1 << (k))) == 0)
        return true;
    else
        return false;
}

static bool chunck_error_condition_check(blob_transfer_server_t *p_server,
        const blob_chunk_transfer_t *p_msg, uint16_t cur_chunk_size)
{
    blob_block_t *p_block_state = &p_server->proc.blob_block_state;
//	blob_state_t *p_blob_state = &p_server->proc.blob_state;
    // transfer_cap_state_t *p_trs_state = &p_server->proc.trs_cap_states;

//	uint16_t block_size;
//	block_size = 1 << p_blob_state->block_size_log;
//	//MESH_MODEL_LOG("block size log %d\n", p_blob_state->block_size_log);
//	MESH_MODEL_LOG("chunk size %d\n", p_block_state->chunk_size);

    uint16_t last_chunk = (p_block_state->block_size + p_block_state->chunk_size - 1) / p_block_state->chunk_size - 1;
    MESH_MODEL_LOG("last chunk index is %d\n",last_chunk);

    if(p_server->proc.phase_state != WAITING_N_CHUNK)
    {
        MESH_MODEL_LOG("state error, %d\n",p_server->proc.phase_state);
        return false;
    }
    else if(is_chunk_received(p_server,p_msg->chunk_number))
    {
        MESH_MODEL_LOG("has received\n");
        return false;
    }
    else if(p_msg->chunk_number > last_chunk)
    {
        MESH_MODEL_LOG("over the last chunk number\n");
        return false;
    }

    else if(p_msg->chunk_number < last_chunk && cur_chunk_size != p_block_state->chunk_size)
    {
        MESH_MODEL_LOG("chunk error\n");
        return false;//TODO may be need to modify;
    }

    return true;

}
static inline uint32_t blob_flash_addr_get(chunk_info_t * chunk_infor)
{
    uint32_t address = (chunk_infor->block_number)* (1 << chunk_infor->block_size_log) +
                       (chunk_infor->chunk_number)*chunk_infor->chunk_size +
                       SIG_OTA_SWITCH_PAGE_ADDR;
    return address;
}

static void chunk_write_flash(chunk_info_t * chunk_infor,const uint8_t * chunk_data)
{
//	MESH_MODEL_LOG("block size log/chunck size/block num/chunk num = %d/%d/%d/%d\n",
//		chunk_infor->block_size_log,chunk_infor->chunk_size,chunk_infor->block_number,chunk_infor->chunk_number);
    uint32_t flash_addr;
    flash_addr = blob_flash_addr_get(chunk_infor);
	SHIELD_WARNING(flash_addr);
//	MESH_MODEL_LOG("flash_addr = 0x%08x\n",flash_addr);
//
//	MESH_MODEL_LOG("chunk_data = 0x");
//	for(uint8_t i=0; i<8; i++)
//	{
//		MESH_MODEL_LOG("%02x",*(chunk_data+i));
//	}
//	MESH_MODEL_LOG("\n");

//    uint32_t flash_data;
//    for(uint8_t i=0; i<(chunk_infor->chunk_size/sizeof(uint32_t)); i++)
//    {

//        memcpy(&flash_data, chunk_data + 4 * i, sizeof(uint32_t));
//        FMC_Write(flash_addr + 4 * i, flash_data);
//    }



    //((tc_flash_write_handler)SVC_TC_FLASH_WRITE)((uint32_t *)flash_addr ,(uint32_t *)(chunk_data),chunk_infor->chunk_size);
//	for(uint8_t i=0;i<(chunk_infor->chunk_size/sizeof(uint32_t));i++)
//	{
//
//		uint32_t flash_data = FMC_Read(flash_addr + 4*i);
//		MESH_MODEL_LOG("flash_data[0x%08x] = 0x%08x\n",flash_addr+4*i,flash_data);
//	}
//	FMC_DISABLE_AP_UPDATE();
//	FMC_Close();
}

void blob_transfer_get(blob_transfer_server_t * p_server,
                       mesh_model_msg_ind_t const *p_rx_msg,
                       blob_transfer_status_param_t * p_out)
{
    p_out->transfer_phase = p_server->proc.phase_state;
    p_out->status = ST_BLOB_SUCCESS;
    p_out->blob_id = p_server->proc.exp_blob_id;

}

void blob_transfer_start(blob_transfer_server_t * p_server,
                         mesh_model_msg_ind_t const *p_rx_msg,
                         const blob_transfer_start_param_t * p_in,
                         blob_transfer_status_param_t * p_out)
{
    MESH_MODEL_LOG("%s\n", __func__);

    uint8_t status;
    uint16_t block_size;


    if(p_server->proc.phase_state == IDLE && p_in->blob_id ==  p_server->proc.exp_blob_id)
    {
        MESH_MODEL_LOG("blob_transfer_start 1 \n");

        //p_server->proc.trs_cap_states.tr_mtu_size = minimum_mtu_size(p_in->mtu_size,p_server->proc.cap_states.mtu_size);
        p_server->proc.trs_cap_states.tr_mtu_size = p_in->mtu_size;
        p_server->proc.blob_state.blob_size = p_in->blob_size;
        p_server->proc.blob_state.block_size_log = p_in->blob_block_size_log;
        p_server->proc.blob_state.blob_id = p_in->blob_id;
        p_server->proc.client_mtu_size = p_in->mtu_size;
        //p_server->proc.timeout = p_in->timeout;
        MESH_MODEL_LOG("blob_size = %d\n",p_server->proc.blob_state.blob_size);

        //erase exchange area for blob size data write
        //((tc_flash_erase_handler)SVC_TC_FLASH_ERASE)((uint32_t *)SIG_OTA_SWITCH_PAGE_ADDR, p_server->proc.blob_state.blob_size);

        MESH_MODEL_LOG("client_mtu_size = %d\n",p_server->proc.client_mtu_size);
        MESH_MODEL_LOG("-----computer for transfer capabilities-----\n");
        computer_for_transfer_capbilities(&p_server->proc.trs_cap_states, &p_server->proc.cap_states);

        block_size = 1 << p_in->blob_block_size_log;
        p_server->proc.blob_state.total_blocks = (p_server->proc.blob_state.blob_size + block_size - 1)/block_size;

        transfer_phase_event event;
        event.arg = NULL;
        event.phase_type = BLOB_TRANSFER_START;
        blob_transfer_server_phase_transitions(p_server, &event);

        status = ST_BLOB_SUCCESS;
    }
    else if(p_server->proc.phase_state == INACTIVE)
    {
        MESH_MODEL_LOG("blob_transfer_start 2 \n");
        status = ST_BLOB_INVALID_STATE;
    }
    else if(p_server->proc.phase_state != INACTIVE && p_in->blob_id !=  p_server->proc.exp_blob_id)
    {
        MESH_MODEL_LOG("blob_transfer_start 3 \n");
        status = ST_BLOB_INVALID_STATE;
    }
    else if(p_server->proc.phase_state != INACTIVE && p_server->proc.phase_state != IDLE &&  p_in->blob_id ==  p_server->proc.exp_blob_id)
    {
        MESH_MODEL_LOG("blob_transfer_start 4 \n");
        status = blob_transfer_all_condiation_is_met(&p_server->proc.blob_state, &p_server->proc.trs_cap_states);

    }
    p_out->status = status;
    p_out->transfer_phase = p_server->proc.phase_state;
    MESH_MODEL_LOG(" p_out->phasel = %02x\n", p_out->transfer_phase);

    //TODO should send send a blob status message with the status field set to the status calculated with the above steps ;

}

void blob_transfer_cancel(blob_transfer_server_t * p_server,
                          mesh_model_msg_ind_t const *p_rx_msg,
                          const blob_transfer_cancel_param_t *p_in,
                          blob_transfer_status_param_t * p_out)
{
    MESH_MODEL_LOG("The cancel BLOB id is %lld", p_in->blob_id);
    // TODO lated
}

void blob_block_get(blob_transfer_server_t * p_server,
                    mesh_model_msg_ind_t const *p_rx_msg,
                    blob_block_status_param_t * p_out)
{
    if(p_server->proc.phase_state == INACTIVE)
    {
        p_out->block_number = 0;
        p_out->chunk_size = 0;
        MESH_MODEL_LOG("p_server->proc.phase_state == INACTIVE");
    }
    else // combine the idle and others situation;
    {
        p_out->status = 0;
        if(last_missing_chunk_check(p_server))
        {
            p_out->format = NO_MISSING_CHUNKS;
            MESH_MODEL_LOG("NO_MISSING_CHUNKS\n");
        }
        else
        {
            p_out->format = SOME_CHUNKS_MISSING;
            MESH_MODEL_LOG("SOME_CHUNKS_MISSING\n");
        }
        p_out->block_number = p_server->proc.blob_block_state.block_number;
        p_out->chunk_size = p_server->proc.blob_block_state.chunk_size;
        memcpy(p_out->mis_chunks, p_server->proc.received_chunks, sizeof(p_server->proc.received_chunks));

        //p_out->format should be computed lated
    }
}

void blob_block_start(blob_transfer_server_t * p_server,
                      mesh_model_msg_ind_t const *p_rx_msg,
                      const blob_block_start_param_t * p_in,
                      blob_block_status_param_t * p_out)
{
    MESH_MODEL_LOG("%s\n", __func__);

    uint8_t status;
    blob_block_t *p_block_state = &p_server->proc.blob_block_state;
    blob_state_t *p_blob_state = &p_server->proc.blob_state;
    transfer_cap_state_t *p_trs_state = &p_server->proc.trs_cap_states;

    if(	p_server->proc.phase_state != IDLE
            && p_server->proc.phase_state != INACTIVE
            && p_server->proc.phase_state != WAITING_N_BLOCK)
    {
        MESH_MODEL_LOG("******p_block_state->block_number %d, p_in->block_number %d\n",
                       p_block_state->block_number, p_in->block_number);
        if(p_block_state->block_number + 1 == p_in->block_number && p_block_state->chunk_size == p_in->chunk_size)
        {
            status = ST_BLOB_SUCCESS;
            MESH_MODEL_LOG("ST_BLOB_SUCCESS 1\n");
        }
        else
        {
            status = ST_BLOB_INVALID_STATE;
            MESH_MODEL_LOG("ST_BLOB_INVALID_STATE\n");
        }
    }
    else if(p_server->proc.phase_state == WAITING_N_BLOCK)
    {
        //This depend on the regulation in Table7.25
        if(p_block_state->block_number >= p_trs_state->tr_max_total_blocks)
        {
            status = ST_BLOB_INVALID_BLOCK_NUMBER;
            MESH_MODEL_LOG("ST_BLOB_INVALID_BLOCK_NUMBER\n");
        }
        else if(p_in->chunk_size > p_trs_state->tr_max_chunks_size)
        {
            status = ST_BLOB_WRONG_CHUNK_SIZE;
            MESH_MODEL_LOG("ST_BLOB_WRONG_CHUNK_SIZE 1\n");
        }
        else if((1 << p_blob_state->block_size_log)/p_block_state->chunk_size > p_trs_state->tr_max_chunks_number)
        {
            status = ST_BLOB_WRONG_CHUNK_SIZE;
            MESH_MODEL_LOG("ST_BLOB_WRONG_CHUNK_SIZE 2\n");
        }
        else
        {
            MESH_MODEL_LOG(" ST_BLOB_SUCCESS blob_transfer_server_phase_transitions\n");

            transfer_phase_event event;
            event.arg = (void *)p_in;
            event.phase_type = BLOB_BLOCK_START;
            blob_transfer_server_phase_transitions(p_server, &event);
            status = ST_BLOB_SUCCESS;
            p_out->block_number = p_server->proc.blob_block_state.block_number;
            p_out->chunk_size = p_server->proc.blob_block_state.chunk_size;
            p_out->format = ALL_CHUNKS_MISSING;
        }
    }
    p_out->status = status;

    //TODO should then respond with a BLOB block status message with the status field set to status calculated above.
}





void blob_chunk_transfer(blob_transfer_server_t * p_server,
                         mesh_model_msg_ind_t const *p_rx_msg,
                         const blob_chunk_transfer_t * p_in)
{
    blob_block_t *p_block_state = &p_server->proc.blob_block_state;
    //blob_state_t *p_blob_state = &p_server->proc.blob_state;
    //transfer_cap_state_t *p_trs_state = &p_server->proc.trs_cap_states;
    uint8_t chunk_phase_state = CHUNK_CONTI;
    MESH_MODEL_LOG("%d/%d/%d<-------------->(chunk/block/total_blocks)\n",
                   p_in->chunk_number,
                   p_block_state->block_number,
                   p_server->proc.blob_state.total_blocks);

    if(chunck_error_condition_check(p_server, p_in, p_in->chunk_size))
    {
        chunk_info_t chunk_infor;
        chunk_infor.block_size_log  = p_server->proc.blob_state.block_size_log;
        chunk_infor.chunk_size = p_server->proc.blob_block_state.chunk_size;
        chunk_infor.block_number = p_block_state->block_number;
        chunk_infor.chunk_number = p_in->chunk_number;

//		MESH_MODEL_LOG("chunk_data = 0x");
//		for(uint8_t i=0;i<8;i++)
//		{
//			MESH_MODEL_LOG("%02x",p_in->chunk_data[i]);
//		}
//		MESH_MODEL_LOG("\n");
        chunk_write_flash(&chunk_infor,p_in->chunk_data);
        chunk_received_set(p_server, p_in->chunk_number);

        MESH_MODEL_LOG("p_server->proc.received_chunks:");
        for(uint8_t i = 0; i < sizeof(p_server->proc.received_chunks); i++)
            MESH_MODEL_LOG("%02x",p_server->proc.received_chunks[i]);
        MESH_MODEL_LOG("\n");

        if(last_missing_chunk_check(p_server))
        {
            chunk_phase_state = CHUNK_FINISHED;
            if(last_block_check(p_server))
            {
                MESH_MODEL_LOG("last_block_check success \n");
                chunk_phase_state = CHUNK_BLOACK_FINISHED;

                //app_sig_mesh_ota_complete_cb();
            }
        }

        transfer_phase_event event;
        event.arg = (void *)&chunk_phase_state;
        event.phase_type = BLOB_CHUNK_MISSING;
        blob_transfer_server_phase_transitions(p_server, &event);
    }
    else
    {
        //ignore the message
    }


}

void blob_information_get(blob_transfer_server_t * p_server,
                          mesh_model_msg_ind_t const *p_rx_msg,
                          blob_information_status_param_t * p_out)
{
    memcpy(p_out, &p_server->proc.cap_states, sizeof(blob_information_status_param_t));
}




