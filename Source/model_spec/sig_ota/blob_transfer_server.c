#include "YK_APP.h"

typedef enum
{
    BLOB_TRANS_STATUS,
    BLOB_BLOCKS_STATUS,
    BLOB_INFO_STATUS,
} blob_server_status_t;


static bool status_send(blob_transfer_server_t * p_server,
                        blob_server_status_t status,
                        mesh_model_msg_ind_t const *p_rx_msg,
                        const void * p_params)
{
    uint16_t msg_len = 0;
    switch(status)
    {
        case BLOB_TRANS_STATUS:
        {
            msg_blob_transfer_status_t msg = {0};
            blob_transfer_status_param_t *p_status = (blob_transfer_status_param_t *)p_params;

            msg.status = p_status->status;
            msg.transfer_phase = p_status->transfer_phase;
            msg.blob_id = p_status->blob_id;
            /*other optional parameters should be added in the fature*/

            if (msg.transfer_phase == IDLE)
            {
                msg_len = 10;// TODO when others parameters were enabled
            }
            else if (msg.transfer_phase == WAITING_N_BLOCK)
            {
                msg.blob_size = p_server->proc.blob_state.blob_size;
                msg.blob_block_size_log = p_server->proc.blob_state.block_size_log;
                msg.client_mtu_size = p_server->proc.client_mtu_size;

                memset(msg.block_not_received, 0, sizeof(msg.block_not_received));
                uint8_t i,j;
                i = (p_server->proc.blob_state.total_blocks - 1) / 8;
                j = (p_server->proc.blob_state.total_blocks - 1) % 8 ;
                memset(msg.block_not_received, 0xFF, i);
                for(uint8_t k = 0; k < j+1; k++)
                {
                    msg.block_not_received[i] |= 1<<k;
                }

                MESH_MODEL_LOG("msg.block_not_received:");
                for(uint8_t k = 0; k < i+1; k++)
                {
                    MESH_MODEL_LOG("%02x",msg.block_not_received[k]);
                }
                MESH_MODEL_LOG("\n");

                msg_len = sizeof(msg) - sizeof(msg.block_not_received) + (i + 1);// TODO when others parameters were enabled
            }

            APP_MESH_Status_Send(p_rx_msg, BLOB_TRANSFER_OPCODE_STATUS, (uint8_t *)&msg, msg_len);

        }
        break;

        case BLOB_BLOCKS_STATUS:
        {
            msg_blob_block_status_t msg = {0};
            blob_block_status_param_t *p_status = (blob_block_status_param_t *)p_params;

            if(p_status->format == SOME_CHUNKS_MISSING)
            {
                msg_len = (p_server->proc.current_chunks + 7) / 8 + 5;
            }
            else
            {
                msg_len = 5;
            }
            msg.status = p_status->status;
            msg.format = p_status->format;
            msg.block_number = p_status->block_number;
            msg.chunk_size = p_status->chunk_size;

            memcpy(msg.mis_chunks, p_status->mis_chunks, (p_server->proc.current_chunks + 7) / 8);

            APP_MESH_Status_Send(p_rx_msg, BLOB_BLOCK_OPCODE_STATUS, (uint8_t *)&msg, msg_len);

        }
        break;

        case BLOB_INFO_STATUS:
        {
            msg_blob_information_status_t msg = {0};

            msg.min_block_size_log = p_server->proc.cap_states.min_block_size_log;
            msg.max_block_size_log = p_server->proc.cap_states.max_block_size_log;
            msg.max_chunks_number = p_server->proc.cap_states.max_chunks_number;
            msg.max_chunks_size = p_server->proc.cap_states.max_chunks_size;
            msg.max_blob_size = p_server->proc.cap_states.max_blob_size;
            msg.mtu_size = p_server->proc.cap_states.server_mtu_size;
            msg.transfer_mode = 0x01;

            msg_len = sizeof(msg);

            APP_MESH_Status_Send(p_rx_msg, BLOB_INFORMATION_OPCODE_STATUS, (uint8_t *)&msg, msg_len);

        }
        break;

        default:
            break;
    }
    return true;
}

static void handle_blob_transfer_get(mesh_model_msg_ind_t const *p_rx_msg, void * p_args)
{
    MESH_MODEL_LOG("%s\n", __func__);

    blob_transfer_server_t * p_server = (blob_transfer_server_t *) p_args;
    blob_transfer_status_param_t out_data = {0};

    MESH_MODEL_LOG("p_rx_msg->length = %d\n", p_rx_msg->msg_len);
    if (p_rx_msg->msg_len == 0)
    {
        p_server->settings.p_callbacks->transfer_cbs.get_cb(p_server, p_rx_msg, &out_data);
        (void) status_send(p_server, BLOB_TRANS_STATUS, p_rx_msg, &out_data);
    }
}

static void handle_blob_transfer_start(mesh_model_msg_ind_t const *p_rx_msg, void * p_args)
{
    MESH_MODEL_LOG("%s\n", __func__);

    blob_transfer_server_t * p_server = (blob_transfer_server_t *) p_args;
    blob_transfer_status_param_t out_data = {0};
    blob_transfer_start_param_t in_data = {0};
    msg_blob_transfer_start_t *msg = (msg_blob_transfer_start_t *)p_rx_msg->msg;

    in_data.blob_id = msg->blob_id;
    in_data.blob_size = msg->blob_size;
    in_data.blob_block_size_log = msg->blob_block_size_log;
    in_data.mtu_size = msg->mtu_size;
//	in_data.timeout = msg->timeout;
    MESH_MODEL_LOG("blob id: %016llx ", in_data.blob_id);
    MESH_MODEL_LOG("blob size: %x block size log: %d ",in_data.blob_size, in_data.blob_block_size_log);
    //MESH_MODEL_LOG("mtu: %x,timeout: %x \n", in_data.mtu_size, in_data.timeout);
    MESH_MODEL_LOG("mtu: %x\n", in_data.mtu_size);

    p_server->settings.p_callbacks->transfer_cbs.start_cb(p_server, p_rx_msg, &in_data, &out_data);
    (void) status_send(p_server, BLOB_TRANS_STATUS, p_rx_msg, &out_data);
}

static void handle_transfer_cancel(mesh_model_msg_ind_t const *p_rx_msg, void * p_args)
{
    MESH_MODEL_LOG("%s\n", __func__);

    blob_transfer_server_t * p_server = (blob_transfer_server_t *) p_args;
    blob_transfer_status_param_t out_data = {0};
    blob_transfer_cancel_param_t id;
    msg_blob_transfer_cancel_t *msg = (msg_blob_transfer_cancel_t *)p_rx_msg->msg;
    id.blob_id = msg->blob_id;
    p_server->settings.p_callbacks->transfer_cbs.cancel_cb(p_server, p_rx_msg, &id, &out_data);
    (void) status_send(p_server, BLOB_TRANS_STATUS, p_rx_msg, &out_data);

}

static void handle_blob_block_get(mesh_model_msg_ind_t const *p_rx_msg, void * p_args)
{
    MESH_MODEL_LOG("%s\n", __func__);

    blob_transfer_server_t * p_server = (blob_transfer_server_t *) p_args;
    blob_block_status_param_t out_data = {0};

    if (p_rx_msg->msg_len == 0)
    {
        p_server->settings.p_callbacks->block_cbs.get_cb(p_server, p_rx_msg, &out_data);
        (void) status_send(p_server, BLOB_BLOCKS_STATUS, p_rx_msg, &out_data);
    }
}

static void handle_blob_block_start(mesh_model_msg_ind_t const *p_rx_msg, void * p_args)
{
    MESH_MODEL_LOG("%s\n", __func__);

    blob_transfer_server_t * p_server = (blob_transfer_server_t *) p_args;
    blob_block_status_param_t out_data = {0};
    blob_block_start_param_t in_data = {0};

    msg_blob_block_start_t *msg = (msg_blob_block_start_t *)p_rx_msg->msg;
    in_data.block_number = msg->block_number;
    in_data.chunk_size = msg->chunk_size;

    MESH_MODEL_LOG("msg->block_number = %d, msg->chunk_size = %d\n",
                   msg->block_number, msg->chunk_size);

    p_server->settings.p_callbacks->block_cbs.start_cb(p_server, p_rx_msg, &in_data, &out_data);
    (void) status_send(p_server, BLOB_BLOCKS_STATUS, p_rx_msg, &out_data);

}

static void handle_blob_info_get(mesh_model_msg_ind_t const *p_rx_msg, void * p_args)
{
    MESH_MODEL_LOG("%s\n", __func__);

    blob_transfer_server_t * p_server = (blob_transfer_server_t *) p_args;
    blob_information_status_param_t out_data = {0};
    if (p_rx_msg->msg_len == 0)
    {
        p_server->settings.p_callbacks->info_cbs.get_cb(p_server, p_rx_msg, &out_data);
        (void) status_send(p_server, BLOB_INFO_STATUS, p_rx_msg, NULL);
    }
}

static void handle_chunk_transfer(mesh_model_msg_ind_t const *p_rx_msg, void * p_args)
{
    MESH_MODEL_LOG("%s\n", __func__);

    blob_transfer_server_t * p_server = (blob_transfer_server_t *) p_args;
    blob_chunk_transfer_t in_data = {0};
    msg_blob_chunk_transfer_t *msg = (msg_blob_chunk_transfer_t *)p_rx_msg->msg;

    in_data.chunk_number = msg->chunk_number;
    in_data.chunk_size = p_rx_msg->msg_len - 2;
    memcpy(in_data.chunk_data, msg->chunk_data, p_rx_msg->msg_len - 2);

    MESH_MODEL_LOG("chunk_number:%d, chunk_size:%d\n", in_data.chunk_number, in_data.chunk_size);

    p_server->settings.p_callbacks->chunk_cbs.trs_cb(p_server, p_rx_msg, &in_data);

}

//static void blob_server_state_init()
//{

//}
static const access_opcode_vendor_handler_t m_opcode_handlers[] =
{
    {ACCESS_OPCODE_VENDOR(BLOB_TRANSFER_OPCODE_GET, ACCESS_COMPANY_ID_AMAZON), handle_blob_transfer_get},
    {ACCESS_OPCODE_VENDOR(BLOB_TRANSFER_OPCODE_START, ACCESS_COMPANY_ID_AMAZON), handle_blob_transfer_start},
    {ACCESS_OPCODE_VENDOR(BLOB_TRANSFER_OPCODE_CANCEL, ACCESS_COMPANY_ID_AMAZON), handle_transfer_cancel},
    {ACCESS_OPCODE_VENDOR(BLOB_BLOCK_OPCODE_GET, ACCESS_COMPANY_ID_AMAZON), handle_blob_block_get},
    {ACCESS_OPCODE_VENDOR(BLOB_BLOCK_OPCODE_START, ACCESS_COMPANY_ID_AMAZON), handle_blob_block_start},
    {ACCESS_OPCODE_VENDOR(BLOB_CHUNK_OPCODE_TRANSFER, ACCESS_COMPANY_ID_AMAZON), handle_chunk_transfer},
    {ACCESS_OPCODE_VENDOR(BLOB_INFORMATION_OPCODE_GET, ACCESS_COMPANY_ID_AMAZON), handle_blob_info_get},
};

const blob_server_callbacks_t m_blob_server_calls =
{
    .transfer_cbs.get_cb = blob_transfer_get,
    .transfer_cbs.start_cb = blob_transfer_start,
    .transfer_cbs.cancel_cb = blob_transfer_cancel,
    .block_cbs.get_cb = blob_block_get,
    .block_cbs.start_cb = blob_block_start,
    .chunk_cbs.trs_cb = blob_chunk_transfer,
    .info_cbs.get_cb = blob_information_get
};

blob_transfer_server_t* mp_blob_transfer_server = NULL;

void blob_transfer_server_init(blob_transfer_server_t * p_server)
{
    if (p_server == NULL)
    {
        return;
    }
    p_server->settings.p_callbacks = &m_blob_server_calls;

    //receive_blob_procedure_start(p_server,0x000007d102150157);
    mp_blob_transfer_server = p_server;
}

/*****************************************************************************
Function name :  blob_transfer_server_msg_handler
Description:	 blob_transfer_server_msg_handler Server 消息处理函数
Input:	         p_rx_msg->消息内容
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void blob_transfer_server_msg_handler(mesh_model_msg_ind_t const *p_rx_msg)
{
    U8 ucIndex = 0;
    U16 Opcode = BigtoLittle16(p_rx_msg->opcode);
    U16 CompanyID =  BigtoLittle16((U16)(p_rx_msg->opcode >> 16));

    for(ucIndex = 0; ucIndex < NumberOfElements(m_opcode_handlers); ucIndex++)
    {
        if((Opcode == m_opcode_handlers[ucIndex].opcode.opcode) && (CompanyID == m_opcode_handlers[ucIndex].opcode.company_id))
        {
            MESH_MODEL_LOG("%s: opcode = 0x%2X ",  __func__, Opcode);

            MESH_MODEL_LOG("msg: ");
            for(U8 i = 0; i < p_rx_msg->msg_len; i++)
            {
                MESH_MODEL_LOG("%02x ", p_rx_msg->msg[i]);
            }
            MESH_MODEL_LOG("\r\n");

            m_opcode_handlers[ucIndex].handler(p_rx_msg, mp_blob_transfer_server);
            break;
        }
    }
}
