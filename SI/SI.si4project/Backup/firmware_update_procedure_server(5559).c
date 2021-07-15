#include "YK_APP.h"


//dfu_state
#define STATE_DFU_SUCCESS                  0x00
#define STATE_DFU_METADATA_CHECK_FAILED    0x01
#define STATE_DFU_INVALID_ID               0x02
#define STATE_DFU_OUT_OF_RESOURCES         0x03
#define STATE_DFU_BLOB_TRANSFER_BUSY       0x04
#define STATE_DFU_INVALID_COMMAND          0x05
#define STATE_DFU_TEMPORARILY_UNAVAILABLE  0x06
#define STATE_DFU_INTERNAL_ERROR           0x07


#define PRODUCT_ID 0x0002
#define TC_PROFILE_SW_VERSION_MAJOR			 2
/// TC APP SW Major Version
#define TC_APP_SW_VERSION_MAJOR              1

#define DEVICE_VERSION_ID (TC_PROFILE_SW_VERSION_MAJOR << 9 | TC_APP_SW_VERSION_MAJOR)

#define DEVICE_PRODUCT_ID (PRODUCT_ID)
/** Device version identifier */
#define DEVICE_VERSION_ID (TC_PROFILE_SW_VERSION_MAJOR << 9 | TC_APP_SW_VERSION_MAJOR)

/** Company ID value for Top-Chip Semiconductor. */
#define ACCESS_COMPANY_ID_TOPCHIP (0x07CE)
/** Device company identifier. */
#define DEVICE_COMPANY_ID (ACCESS_COMPANY_ID_TOPCHIP)
typedef enum
{
    E_VERIFY_SUCCESS = 0x0,
    E_VERIFY_FAILED  = 0x1
} verify_status_t;


//APP_TIMER_DEF(m_ota_apply_reset_timer);

static void ota_apply_reset_cb(void * p_context)
{
    //save ONOFF state for OTA reset
    //save_ota_onoff_status();

    firmware_update_server_t *p_server = (firmware_update_server_t *)p_context;
    if (p_server->state.fw_update_additonal_info & PROVISIONING_NEEDED )
    {

//		((tc_flash_erase_handler)SVC_TC_FLASH_ERASE)((uint32_t *)ERASE_START_ADDR, ERASE_END_ADDR - ERASE_START_ADDR);
//		((tc_flash_erase_handler)SVC_TC_FLASH_ERASE)((uint32_t *)CAL_DATA_ADDR,1024);
//
//		MESH_MODEL_LOG("erase server config data\n");

//		/* software reset */
//		*(volatile uint32_t *)0x50000008 |= 0x1;

    }
    else
    {
//		MESH_MODEL_LOG("chip reset\n");
//		//set a timer to callback reset only
//		/* software reset */
//		*(volatile uint32_t *)0x50000008 |= 0x1;
    }
}

//static void generate_blob_id(device_firmware_update_server_t * p_server)
//{
////	firmware_id_t incoming_firmware_id;
////
////	incoming_firmware_id.company_id    = p_server->state.incoming_firmware_ID.company_id;
////	incoming_firmware_id.version_infor = p_server->state.incoming_firmware_ID.version_infor;

////	p_server->state.BLOB_ID = incoming_firmware_id.blob_id;
//
//	p_server->state.BLOB_ID = p_server->state.incoming_firmware_ID.company_id;
//	MESH_MODEL_LOG("p_server->state.BLOB_ID = 0x%08X%08X\n",p_server->state.BLOB_ID >> 32,p_server->state.BLOB_ID);
//	p_server->state.BLOB_ID <<= 32;
//	MESH_MODEL_LOG("p_server->state.BLOB_ID = 0x%08X%08X\n",p_server->state.BLOB_ID >> 32,p_server->state.BLOB_ID);
//	MESH_MODEL_LOG("p_server->state.incoming_firmware_ID.version_infor = 0x%08X\n",p_server->state.incoming_firmware_ID.version_infor);
//	p_server->state.BLOB_ID = p_server->state.BLOB_ID + p_server->state.incoming_firmware_ID.version_infor;
//	MESH_MODEL_LOG("p_server->state.BLOB_ID = 0x%016llx\n",p_server->state.BLOB_ID);
//}

void firmware_update_info_get(firmware_update_server_t * p_server,
                              mesh_model_msg_ind_t const *p_rx_msg,
                              const firmware_update_info_get_param_t * p_in,
                              firmware_update_info_status_param_t * p_out)
{
    MESH_MODEL_LOG("%s\n",__func__);

    MESH_MODEL_LOG("p_in->entry_index =  %02x\n", p_in->entry_index);
    MESH_MODEL_LOG("p_in->entris_limit = %02x\n", p_in->entris_limit);

    //tag version
    if(p_in->entry_index == 0)
    {
        p_out->list_count = 1;
        p_out->entry_index = p_in->entry_index;
        p_out->update_uri_len = 0;
        p_out->current_fw_id_len = 4;
        p_out->current_fw_id.product_id = p_server->state.fw_info_list[p_in->entry_index].current_fw_id.product_id;
        p_out->current_fw_id.version_id = p_server->state.fw_info_list[p_in->entry_index].current_fw_id.version_id;
        MESH_MODEL_LOG("p_out->current_fw_id.version_id = %04x\n",p_out->current_fw_id.version_id );
    }
    else
    {
        MESH_MODEL_LOG("Can't find available entry index\n");
        p_out->list_count = 1;
        p_out->entry_index = 0;
        p_out->update_uri_len = 0;
        p_out->current_fw_id_len = 4;
        p_out->current_fw_id.product_id = p_server->state.fw_info_list[0].current_fw_id.product_id;
        p_out->current_fw_id.version_id = p_server->state.fw_info_list[0].current_fw_id.version_id;
        MESH_MODEL_LOG("p_out->current_fw_id.version_id = %04x\n", p_out->current_fw_id.version_id );
    }
}


void firmware_update_fw_meta_check(firmware_update_server_t * p_server,
                                   mesh_model_msg_ind_t const *p_rx_msg,
                                   const firmware_update_meta_check_param_t * p_in,
                                   firmware_update_meta_status_param_t * p_out)
{
    bool check_metadata_passed;

//	new_stack_version_t *stack_local_version = (new_stack_version_t *)STACK_VERSION_ADDR;    //device running profile version
//    app_version_t *app_local_version = (app_version_t *)APP_VERSION_ADDR;

    MESH_MODEL_LOG("%s\n",__func__);

    MESH_MODEL_LOG("p_in->fw_meta_len = %02x\n", p_in->fw_meta_len);
//	MESH_MODEL_LOG("p_in->fw_id.product_id = %04x\n", p_in->fw_id.product_id);
//	MESH_MODEL_LOG("p_in->fw_id.version_id = %04x\n", p_in->fw_id.version_id);
    //tag check OK or reject

    if (p_in->metadata.company_id == DEVICE_COMPANY_ID &&
            p_in->metadata.product_id == DEVICE_PRODUCT_ID)
    {
//		if (p_in->metadata.app_ver.version == 0 )    // check profile firmware
//		{
//			if (p_in->metadata.stack_ver_major == stack_local_version->stack_ver_major
//				&& p_in->metadata.stack_ver_minor >= app_local_version->stack_ver_minor)
//			{
//				check_metadata_passed =  true;
//			} else
//			{
//				check_metadata_passed =  false;
//			}
//		}
//		else   // check app firmware
//		{
//			if ( (p_in->metadata.stack_ver_major == stack_local_version->stack_ver_major)
//				&& (p_in->metadata.stack_ver_minor <= stack_local_version->stack_ver_minor))
//			{
//				check_metadata_passed =  true;
//			} else
//			{
//				check_metadata_passed =  false;
//			}
//		}
    }
    if (check_metadata_passed)
    {
        MESH_MODEL_LOG("p_in->firmware_ID.product_id=%04X", p_in->metadata.product_id);
        MESH_MODEL_LOG("dfu_metadata_check success\n");

        p_server->state.incoming_fw_id.product_id = p_in->metadata.product_id;
        p_server->state.incoming_fw_id.version_id = p_in->metadata.version_id;

        //generate_blob_id(p_server);

        p_out->status = STATE_DFU_SUCCESS;
        p_out->rfu = 0;
        p_out->fw_id.product_id = p_in->metadata.product_id;
        p_out->fw_id.version_id = p_in->metadata.version_id;
    }
    else
    {
        MESH_MODEL_LOG("The Firmware Metadata field is present, and the firmware is not accepted \n");
        p_out->status = STATE_DFU_METADATA_CHECK_FAILED;
        p_out->rfu = 0;
        p_out->fw_id.product_id = p_in->metadata.product_id;
        p_out->fw_id.version_id = p_in->metadata.version_id;
    }

}


void firmware_update_get(firmware_update_server_t * p_server,
                         mesh_model_msg_ind_t const *p_rx_msg,
                         firmware_update_status_param_t * p_out)
{
    MESH_MODEL_LOG("%s\n",__func__);

    switch (p_server->state.update_phase)
    {
        case UPDATAE_PHASE_IDLE:
            MESH_MODEL_LOG("UPDATAE_PHASE_IDLE\n");
            break;
        case UPDATAE_PHASE_TRANSFER_ERROR:
            MESH_MODEL_LOG("UPDATAE_PHASE_TRANSFER_ERROR\n");
            break;
        case UPDATAE_PHASE_TRANSFER_ACTIVE:
            MESH_MODEL_LOG("UPDATAE_PHASE_TRANSFER_ACTIVE\n");
            break;
        case UPDATAE_PHASE_VERIFICATION_SUCCESS:
            MESH_MODEL_LOG("UPDATAE_PHASE_VERIFICATION_SUCCESS\n");
            break;
        case UPDATAE_PHASE_VERIFICATION_FAILED:
            MESH_MODEL_LOG("UPDATAE_PHASE_VERIFICATION_FAILED\n");
            break;
        case UPDATAE_PHASE_APPLY_ACTIVE:
            MESH_MODEL_LOG("UPDATAE_PHASE_APPLY_ACTIVE\n");
            break;
        case UPDATAE_PHASE_TRANSFER_CANCELLED:
            MESH_MODEL_LOG("UPDATAE_PHASE_TRANSFER_CANCELLED\n");
            break;
        case UPDATAE_PHASE_UNKOWN:
            MESH_MODEL_LOG("UPDATAE_PHASE_UNKOWN\n");
            break;
    }

    p_out->status = STATE_DFU_SUCCESS;
    p_out->rfu = 0;
    p_out->update_phase = p_server->state.update_phase;
    p_out->additional_info  = p_server->state.fw_update_additonal_info;
    p_out->distribution_ttl = p_server->state.distribution_ttl;
    p_out->blob_id = p_server->state.blob_id;
    p_out->image_index = 0;
//	p_out->incoming_fw_id.product_id = p_self->state.incoming_fw_id.product_id;
//	p_out->incoming_fw_id.version_id = p_self->state.incoming_fw_id.version_id;

}

void firmware_update_start(firmware_update_server_t * p_server,
                           mesh_model_msg_ind_t const *p_rx_msg,
                           const firmware_update_start_param_t *p_in,
                           firmware_update_status_param_t * p_out)
{
    MESH_MODEL_LOG("%s\n",__func__);

    //tag start blob transfer
    switch (p_server->state.update_phase)
    {
        case UPDATAE_PHASE_IDLE:
            MESH_MODEL_LOG("UPDATAE_PHASE_IDLE\n");
            break;
        case UPDATAE_PHASE_TRANSFER_ERROR:
            MESH_MODEL_LOG("UPDATAE_PHASE_TRANSFER_ERROR\n");
            break;
        case UPDATAE_PHASE_TRANSFER_ACTIVE:
            MESH_MODEL_LOG("UPDATAE_PHASE_TRANSFER_ACTIVE\n");
            break;
        case UPDATAE_PHASE_VERIFICATION_SUCCESS:
            MESH_MODEL_LOG("UPDATAE_PHASE_VERIFICATION_SUCCESS\n");
            break;
        case UPDATAE_PHASE_VERIFICATION_FAILED:
            MESH_MODEL_LOG("UPDATAE_PHASE_VERIFICATION_FAILED\n");
            break;
        case UPDATAE_PHASE_APPLY_ACTIVE:
            MESH_MODEL_LOG("UPDATAE_PHASE_APPLY_ACTIVE\n");
            break;
        case UPDATAE_PHASE_TRANSFER_CANCELLED:
            MESH_MODEL_LOG("UPDATAE_PHASE_TRANSFER_CANCELLED\n");
            break;
        case UPDATAE_PHASE_UNKOWN:
            MESH_MODEL_LOG("UPDATAE_PHASE_UNKOWN\n");
            break;
    }
    if(p_server->state.update_phase == UPDATAE_PHASE_IDLE ||
            p_server->state.update_phase == UPDATAE_PHASE_TRANSFER_ERROR ||
            p_server->state.update_phase == UPDATAE_PHASE_VERIFICATION_FAILED )
    {
        p_server->state.update_phase = UPDATAE_PHASE_TRANSFER_ACTIVE;
        p_server->state.distribution_ttl = p_in->distribution_ttl;
//		if(p_server->state.blob_id != p_in->blob_id || p_in->metadata_len != 0
////			|| p_in->fw_metadata.fw_id.product_id != p_server->state.incoming_fw_id.product_id
////			|| p_in->fw_metadata.fw_id.version_id != p_server->state.incoming_fw_id.version_id
//		)
//		{
//			MESH_MODEL_LOG("p_server->state.blob_id =%x, p_in->blob_id = %x, p_in->metadata_len = %d\n",
//				p_server->state.blob_id, p_in->blob_id, p_in->metadata_len);
//
//
//
//			p_out->status = STATE_DFU_INVALID_COMMAND;
//			MESH_MODEL_LOG("p_out->status = STATE_DFU_INVALID_COMMAND\n");
//			p_server->state.update_phase = UPDATAE_PHASE_IDLE;
//		}
//		else
        {
//			extern uint8_t app_ble_conn_param_update_request(uint16_t intv_min, uint16_t intv_max, uint16_t lantency, uint16_t timeout);

//			//update connection param
//			if (!app_ble_conn_param_update_request(120, 120, 0, 300)) {
//				MESH_MODEL_LOG("successful update connection param\n");
//			} else {
//				MESH_MODEL_LOG("failed update connection param\n");
//			}

            //set Metadata to message use in fultrue
            p_server->state.blob_id = p_in->blob_id;

            MESH_MODEL_LOG("p_server->state.blob_id =%016llx, p_in->blob_id = %016llx, p_in->metadata_len = %x\n",
                           p_server->state.blob_id, p_in->blob_id, p_in->metadata_len);


            p_out->status = STATE_DFU_SUCCESS;
            MESH_MODEL_LOG("p_out->status = STATE_DFU_SUCCESS\n");
            p_out->rfu = 0;
            p_out->update_phase = p_server->state.update_phase;
            p_out->additional_info  = p_server->state.fw_update_additonal_info;

            p_out->distribution_ttl = p_server->state.distribution_ttl;
            p_out->blob_id = p_server->state.blob_id;
//			p_out->incomming_firmware_ID.company_id = p_server->state.incoming_firmware_ID.company_id;
//			p_out->incomming_firmware_ID.version_infor = p_server->state.incoming_firmware_ID.version_infor;
            //start BLOB transfer procedure

            if(mp_blob_transfer_server)
            {
                MESH_MODEL_LOG("receive_blob_procedure_start\n");
                receive_blob_procedure_start(mp_blob_transfer_server, p_server->state.blob_id);
            }


        }
    }
    else if(0) //The BLOB Transfer Server is executing some other procedure
    {
        p_out->status = STATE_DFU_BLOB_TRANSFER_BUSY;
    }
    else if(0) //The firmware metadata check failed
    {
        p_out->status = STATE_DFU_METADATA_CHECK_FAILED;
    }
    else if(0) //The server cannot allocate necessary resources to receive firmware image
    {
        p_out->status = STATE_DFU_OUT_OF_RESOURCES;
    }
    else if(0) //The server wants to reject the update (e.g. low battery level)
    {
        p_out->status = STATE_DFU_TEMPORARILY_UNAVAILABLE;
    }



}

void firmware_update_cancel(firmware_update_server_t * p_server,
                            mesh_model_msg_ind_t const *p_rx_msg,
                            firmware_update_status_param_t * p_out)
{
    MESH_MODEL_LOG("%s\n", __func__);

    p_server->state.update_phase = UPDATAE_PHASE_IDLE;
    p_server->state.fw_update_additonal_info = 0;
    //p_server->state.bl = 0;
    p_server->state.distribution_ttl = 0;
    p_server->state.incoming_fw_id.product_id = 0;
    p_server->state.incoming_fw_id.version_id = 0;

    p_out->status = STATE_DFU_SUCCESS;
    p_out->rfu = 0;
    p_out->update_phase = p_server->state.update_phase;
    p_out->additional_info  = p_server->state.fw_update_additonal_info;
    p_out->distribution_ttl = p_server->state.distribution_ttl;
    p_out->blob_id = p_server->state.blob_id;
//	p_out->incoming_fw_id.product_id= p_server->state.incoming_fw_id.product_id;
//	p_out->incoming_fw_id.version_id= p_server->state.incoming_fw_id.version_id;


}


static verify_status_t verify_firmware(void)
{
    MESH_MODEL_LOG("%s\n", __func__);

//	if(data_xor_check() == 1)
//	{
    return E_VERIFY_SUCCESS;

//	}else
//	{
//		return E_VERIFY_FAILED;
//	}
}

void firmware_update_apply(firmware_update_server_t * p_server,
                           mesh_model_msg_ind_t const *p_rx_msg,
                           firmware_update_status_param_t * p_out)

{
    MESH_MODEL_LOG("%s\n", __func__);

    /*The Firmware Update Server model shall use either the Firmware Metadata state (if not empty)
    or any other means that are implementation specific to verify the firmware image provided as an input.*/
    verify_status_t verify_status = verify_firmware();
    if(verify_status == E_VERIFY_SUCCESS)
    {
        MESH_MODEL_LOG("E_VERIFY_SUCCESS\n");
        p_server->state.update_phase = UPDATAE_PHASE_VERIFICATION_SUCCESS;

        p_out->status = STATE_DFU_SUCCESS;
        p_out->rfu = 0;
        p_out->update_phase = p_server->state.update_phase;
        p_out->additional_info  = p_server->state.fw_update_additonal_info;
        p_out->distribution_ttl = p_server->state.distribution_ttl;
        p_out->blob_id = p_server->state.blob_id;
//		p_out->incoming_fw_id.product_id= p_server->state.incoming_fw_id.product_id;
//		p_out->incoming_fw_id.version_id= p_server->state.incoming_fw_id.version_id;
//
//		//save ONOFF state for OTA reset
//		save_ota_onoff_status();
//		//set a timer to callback erase net infor config and reset
//		app_timer_create(&m_ota_apply_reset_timer, APP_TIMER_MODE_SINGLE_SHOT, ota_apply_reset_cb);
//		app_timer_start(m_ota_apply_reset_timer, HAL_SECS_TO_RTC_TICKS(3), p_server);
    }
    else if(verify_status == E_VERIFY_FAILED)
    {
        MESH_MODEL_LOG("E_VERIFY_FAILED\n");
        p_server->state.update_phase = UPDATAE_PHASE_VERIFICATION_FAILED;
        p_server->state.fw_update_additonal_info = 0;
        p_server->state.blob_id = 0;
        p_server->state.distribution_ttl = 0;
        p_server->state.incoming_fw_id.product_id = 0;
        p_server->state.incoming_fw_id.version_id = 0;

        //set a timer to callback erase exchange block

        p_out->status = STATE_DFU_SUCCESS;
        p_out->rfu = 0;
        p_out->update_phase = p_server->state.update_phase;
        p_out->additional_info = p_server->state.fw_update_additonal_info;
        p_out->distribution_ttl = p_server->state.distribution_ttl;
        p_out->blob_id = p_server->state.blob_id;
//		p_out->incoming_fw_id.product_id= p_server->state.incoming_fw_id.product_id;
//		p_out->incoming_fw_id.version_id= p_server->state.incoming_fw_id.version_id;
    }

}

void firmware_update_init(firmware_update_server_t * p_server)
{
    p_server->state.fw_info_list[0].current_fw_id.product_id = DEVICE_PRODUCT_ID;
    p_server->state.fw_info_list[0].current_fw_id.version_id = DEVICE_VERSION_ID;

    p_server->state.update_phase = UPDATAE_PHASE_IDLE;
    p_server->state.fw_update_additonal_info = 0;
    p_server->state.blob_id = 0;
    p_server->state.distribution_ttl = 0;
    p_server->state.incoming_fw_id.product_id = 0;
    p_server->state.incoming_fw_id.version_id = 0;
}


