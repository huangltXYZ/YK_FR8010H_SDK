#include "YK_APP.h"

typedef enum
{
    FW_INFO_STATUS,
    FW_META_STATUS,
    FW_UPDATE_STATUS,
} firmware_update_server_status_t;


static bool status_send(firmware_update_server_t * p_server,
                        firmware_update_server_status_t status,
                        mesh_model_msg_ind_t const *p_rx_msg,
                        const void * p_params)
{
    switch(status)
    {
        case FW_INFO_STATUS:
        {
            msg_firmware_update_info_status_t msg;
            firmware_update_info_status_param_t *p_status = (firmware_update_info_status_param_t *)p_params;

            msg.list_count = p_status->list_count;
            msg.entry_index = p_status->entry_index;
            msg.update_uri_len = p_status->update_uri_len;
            msg.current_fw_id_len = p_status->current_fw_id_len;
            msg.current_fw_id.product_id = p_status->current_fw_id.product_id;
            msg.current_fw_id.version_id = p_status->current_fw_id.version_id;

            /*other optional parameters should be added in the future*/
            APP_MESH_Status_Send(p_rx_msg, FIRMWARE_UPDATE_INFO_OPCODE_STATUS, (uint8_t *)&msg, sizeof(msg));

        }
        break;

        case FW_META_STATUS:
        {
            msg_firmware_update_meta_status_t msg;
            firmware_update_meta_status_param_t  *p_status = (firmware_update_meta_status_param_t  *)p_params;

            msg.status = p_status->status;
            msg.rfu = p_status->rfu;
            msg.fw_id.product_id = p_status->fw_id.product_id;
            msg.fw_id.version_id = p_status->fw_id.version_id;

            /*other optional parameters should be added in the future*/
            APP_MESH_Status_Send(p_rx_msg, FIRMWARE_UPDATE_FW_META_OPCODE_STATUS, (uint8_t *)&msg, sizeof(msg));

        }
        break;

        case FW_UPDATE_STATUS:
        {
            msg_firmware_update_status_t msg;
            firmware_update_status_param_t *p_status = (firmware_update_status_param_t *)p_params;

            msg.status = p_status->status;
            msg.rfu = p_status->rfu;
            msg.phase = p_status->update_phase;
            msg.additional_info = p_status->additional_info;
            msg.distribution_ttl = p_status->distribution_ttl;
            msg.blob_id = p_status->blob_id;
//				msg->incomming_firmware_ID.company_id = p_status->additiona_infor incomming_firmware_ID.company_id;
//				msg->incomming_firmware_ID.version_infor = p_status->incomming_firmware_ID.version_infor;
//
            /*other optional parameters should be added in the future*/
            APP_MESH_Status_Send(p_rx_msg, FIRMWARE_UPDATE_OPCODE_STATUS, (uint8_t *)&msg, sizeof(msg));

        }
        break;
    }

    return true;
}

static void handle_fw_update_info_get(mesh_model_msg_ind_t const *p_rx_msg, void * p_args)
{
    MESH_MODEL_LOG("%s\n", __func__);

    firmware_update_server_t  *p_server = (firmware_update_server_t *) p_args;
    firmware_update_info_get_param_t in_data = {0};
    firmware_update_info_status_param_t out_data = {0};
    msg_firmware_update_info_get_t  *msg = (msg_firmware_update_info_get_t *)p_rx_msg->msg;

//    if (p_rx_msg->length == sizeof(msg_firmware_update_info_get_t))
//    {
    in_data.entry_index = msg->entry_index;
    in_data.entris_limit = msg->entris_limit;
    p_server->settings.p_callbacks->info_cbs.get_cb(p_server, p_rx_msg, &in_data, &out_data);
    (void) status_send(p_server, FW_INFO_STATUS, p_rx_msg, &out_data);
//    }
}

static void handle_fw_update_meta_check(mesh_model_msg_ind_t const *p_rx_msg, void * p_args)
{
    MESH_MODEL_LOG("%s\n",__func__);

    firmware_update_server_t *p_server = (firmware_update_server_t *) p_args;
    firmware_update_meta_status_param_t  out_data = {0};
    firmware_update_meta_check_param_t  in_data = {0};
    msg_firmware_update_meta_check_t *msg = (msg_firmware_update_meta_check_t *)p_rx_msg->msg;

//    if (p_rx_msg->length == sizeof(msg_firmware_updata_meata_check_t))
//    {
    in_data.fw_meta_len  = msg->metadata_len;
    memcpy(&in_data.metadata, &msg->metadata, sizeof(in_data.metadata));
    p_server->settings.p_callbacks->meta_cbs.check_cb(p_server, p_rx_msg, &in_data, &out_data);
    (void) status_send(p_server, FW_META_STATUS, p_rx_msg, &out_data);
//    }
}

static void handle_fw_update_get(mesh_model_msg_ind_t const *p_rx_msg, void * p_args)
{
    MESH_MODEL_LOG("%s\n",__func__);

    firmware_update_server_t  *p_server = (firmware_update_server_t *) p_args;
    firmware_update_status_param_t out_data = {0};

    p_server->settings.p_callbacks->update_cbs.get_cb(p_server, p_rx_msg, &out_data);
    (void) status_send(p_server, FW_UPDATE_STATUS, p_rx_msg, &out_data);
}

static void handle_fw_update_start(mesh_model_msg_ind_t const *p_rx_msg, void * p_args)
{
    MESH_MODEL_LOG("%s\n",__func__);

    firmware_update_server_t *p_server = (firmware_update_server_t *) p_args;
    firmware_update_status_param_t out_data = {0};
    firmware_update_start_param_t in_data = {0};
    msg_firmware_update_start_t *msg = (msg_firmware_update_start_t *)p_rx_msg->msg;

    MESH_MODEL_LOG("handle_fw_update_start length match: %d, %d\n",
                   p_rx_msg->msg_len, sizeof(msg_firmware_update_start_t));

//	uint8_t distribution_ttl;//0x00,0x02-0x7f
//	uint16_t timeout_base; // unit: 10 second
//	uint64_t blob_id;
//	uint8_t image_index;
//	01
//	0A00
//	0807060504030201
//	00
    for (int i=0; i< p_rx_msg->msg_len; i++)
        MESH_MODEL_LOG("%02X", p_rx_msg->msg[i]);
    MESH_MODEL_LOG("\n");

    //memcpy(&in_data.blob_id, (const void*)&msg->blob_id, 8);

//    if (p_rx_msg->length == sizeof(msg_firmware_update_start_t))
//    {
    in_data.distribution_ttl = msg->distribution_ttl;
    in_data.blob_id = msg->blob_id;
    in_data.image_index = msg->image_index;

    MESH_MODEL_LOG("distribution_ttl:%02x,blob_id=%016llx, in_data.image_index=%x\n",
                   in_data.distribution_ttl, in_data.blob_id, in_data.image_index );
    //in_data.fw_metadata.fw_id.product_id = msg->metadata.fw_id.product_id;
    //in_data.fw_metadata.fw_id.version_id = msg->metadata.fw_id.version_id;
    p_server->settings.p_callbacks->update_cbs.start_cb(p_server, p_rx_msg, &in_data, &out_data);
    (void) status_send(p_server, FW_UPDATE_STATUS, p_rx_msg, &out_data);
//    }
}

static void handle_fw_update_cancel(mesh_model_msg_ind_t const *p_rx_msg, void * p_args)
{
    MESH_MODEL_LOG("%s\n",__func__);

    firmware_update_server_t *p_server = (firmware_update_server_t *) p_args;
    firmware_update_status_param_t out_data = {0};

//    if (p_rx_msg->length == 0)
//    {
    p_server->settings.p_callbacks->update_cbs.cancel_cb(p_server, p_rx_msg, &out_data);
    (void) status_send(p_server,FW_UPDATE_STATUS, p_rx_msg, &out_data);
//    }
}

static void handle_fw_update_apply(mesh_model_msg_ind_t const *p_rx_msg, void * p_args)
{
    MESH_MODEL_LOG("%s\n", __func__);

    firmware_update_server_t *p_server = (firmware_update_server_t *) p_args;
    firmware_update_status_param_t out_data = {0};

//    if (p_rx_msg->length == 0)
//    {
    p_server->settings.p_callbacks->update_cbs.apply_cb(p_server, p_rx_msg, &out_data);
    (void) status_send(p_server,FW_UPDATE_STATUS, p_rx_msg, &out_data);
//    }
}

static const access_opcode_vendor_handler_t m_opcode_handlers[] =
{
    {ACCESS_OPCODE_VENDOR(FIRMWARE_UPDATE_INFO_OPCODE_GET, ACCESS_COMPANY_ID_AMAZON), handle_fw_update_info_get},
    {ACCESS_OPCODE_VENDOR(FIRMWARE_UPDATE_FW_META_OPCODE_CHECK, ACCESS_COMPANY_ID_AMAZON), handle_fw_update_meta_check},
    {ACCESS_OPCODE_VENDOR(FIRMWARE_UPDATE_OPCODE_GET, ACCESS_COMPANY_ID_AMAZON), handle_fw_update_get},
    {ACCESS_OPCODE_VENDOR(FIRMWARE_UPDATE_OPCODE_START, ACCESS_COMPANY_ID_AMAZON), handle_fw_update_start},
    {ACCESS_OPCODE_VENDOR(FIRMWARE_UPDATE_OPCODE_CANCEL, ACCESS_COMPANY_ID_AMAZON), handle_fw_update_cancel},
    {ACCESS_OPCODE_VENDOR(FIRMWARE_UPDATE_OPCODE_APPLY, ACCESS_COMPANY_ID_AMAZON), handle_fw_update_apply},
};

const firmware_update_callbacks_t m_fw_update_server_calls  =
{
    .info_cbs.get_cb = firmware_update_info_get,
    .meta_cbs.check_cb = firmware_update_fw_meta_check,
    .update_cbs.get_cb = firmware_update_get,
    .update_cbs.start_cb = firmware_update_start,
    .update_cbs.cancel_cb = firmware_update_cancel,
    .update_cbs.apply_cb = firmware_update_apply
};


firmware_update_server_t* mp_firmware_update_server = NULL;

void firmware_update_server_init(firmware_update_server_t *p_server)
{
    if (p_server == NULL)
    {
        return;
    }
    mp_firmware_update_server = p_server;
    p_server->settings.p_callbacks = &m_fw_update_server_calls;

    firmware_update_init(p_server);
}

/*****************************************************************************
Function name :  firmware_update_server_msg_handler
Description:	 firmware_update_server_msg_handler Server 消息处理函数
Input:	         p_rx_msg->消息内容
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void firmware_update_server_msg_handler(mesh_model_msg_ind_t const *p_rx_msg)
{
    U8 ucIndex = 0;
    U16 Opcode = BigtoLittle16(p_rx_msg->opcode);
    U16 CompanyID =  BigtoLittle16((U16)(p_rx_msg->opcode >> 16));

    for(ucIndex = 0; ucIndex < NumberOfElements(m_opcode_handlers); ucIndex++)
    {
        if((Opcode == m_opcode_handlers[ucIndex].opcode.opcode) && (CompanyID == m_opcode_handlers[ucIndex].opcode.company_id))
        {
            MESH_MODEL_LOG("%s: opcode = 0x%4X ",  __func__, Opcode);

            MESH_MODEL_LOG("msg: ");
            for(U8 i = 0; i < p_rx_msg->msg_len; i++)
            {
                MESH_MODEL_LOG("%02x ", p_rx_msg->msg[i]);
            }
            MESH_MODEL_LOG("\r\n");

            m_opcode_handlers[ucIndex].handler(p_rx_msg, mp_firmware_update_server);
            break;
        }
    }
}

