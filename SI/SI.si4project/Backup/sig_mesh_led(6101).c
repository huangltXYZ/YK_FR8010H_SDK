/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */

/*
 * INCLUDES 
 */
#include <stdint.h>
#include <stdbool.h>

#include "sha256.h"
#include "co_printf.h"
#include "os_timer.h"
#include "os_mem.h"
#include "sys_utils.h"
#include "gap_api.h"

#include "mesh_api.h"
#include "mesh_model_msg.h"
#include "sig_mesh_info.h"

#include "driver_plf.h"
#include "driver_system.h"
#include "driver_pmu.h"
#include "driver_gpio.h"
#include "driver_flash.h"
#include "button.h"

#include "flash_usage_config.h"
//#include "demo_clock.h"
//#include "vendor_timer_ctrl.h"
#include "YK_APP.h"
/*
 * MACROS 
 */
//#define SIG_MESH_TIMER

/*
 * CONSTANTS 
 */


uint8_t mesh_key_bdaddr[] = {0xda,0xbb,0x6b,0x07,0xda,0x71};
uint8_t mesh_key_pid[] = {0x0e, 0x01, 0x00, 0x00};
uint8_t mesh_key_secret[] = {0x92,0x37,0x41,0x48,0xb5,0x24,0xd9,0xcf,0x7c,0x24,0x04,0x36,0x0b,0xa8,0x91,0xd0};

/*
 * TYPEDEFS 
 */
typedef struct led_hsl_s
{
    uint16_t led_l;
    uint16_t led_h;
    uint16_t led_s;
}led_hsl_t;

/*
 * GLOBAL VARIABLES 
 */

/*
 * LOCAL VARIABLES 
 */
static os_timer_t sig_mesh_prov_timer;

/*
 * LOCAL FUNCTIONS 
 */
static void app_mesh_recv_gen_onoff_led_msg(mesh_model_msg_ind_t const *ind);
static void app_mesh_recv_gen_dtt_led_msg(mesh_model_msg_ind_t const *ind);
static void app_mesh_recv_gen_ponoff_led_msg(mesh_model_msg_ind_t const *ind);
static void app_mesh_recv_gen_level_led_msg(mesh_model_msg_ind_t const *ind);

static void app_mesh_recv_lightness_msg(mesh_model_msg_ind_t const *ind);
static void app_mesh_recv_hsl_msg(mesh_model_msg_ind_t const *ind);
static void app_mesh_recv_CTL_msg(mesh_model_msg_ind_t const *ind);
static void app_mesh_recv_CTL_setup_msg(mesh_model_msg_ind_t const *ind);
static void app_mesh_recv_CTL_temperature_msg(mesh_model_msg_ind_t const *ind);
//static void app_mesh_recv_vendor_msg(mesh_model_msg_ind_t const *ind);

void app_mesh_start_publish_msg_resend(uint8_t * p_msg,uint8_t p_len);


// Mesh model define
static mesh_model_t light_models[] = 
{
/************Light Lightness Setup Server************/
    [0] = /*Generic OnOff*/
	{
        .model_id = GENERIC_ONOFF_SERVER_MODEL_ID,
        .model_vendor = false,
        .element_idx = 0,
        .msg_handler = generic_onoff_server_msg_handler,
    },
	
	[1] = /*Generic Default Transition Time*/
	{
        .model_id = GENERIC_DTT_SERVER_MODEL_ID,
        .model_vendor = false,
        .element_idx = 0,
        .msg_handler = generic_dtt_server_msg_handler,
    },
	
	[2] = /*Generic OnPowerUp*/
	{
        .model_id = GENERIC_PONOFF_SERVER_MODEL_ID,
        .model_vendor = false,
        .element_idx = 0,
        .msg_handler = generic_ponoff_server_msg_handler,
    },
	
	[3] = /*Generic Power OnOff Setup Server*/
	{
        .model_id = GENERIC_PONOFF_SETUP_SERVER_MODEL_ID,
        .model_vendor = false,
        .element_idx = 0,
        .msg_handler = generic_ponoff_server_msg_handler,
    },
	
	[4] = /*Generic Level*/
	{
        .model_id = GENERIC_LEVEL_SERVER_MODEL_ID,
        .model_vendor = false,
        .element_idx = 0,
        .msg_handler = generic_level_server_msg_handler,
    },
    [5] = /*Light Lightness*/
	{
        .model_id = LIGHT_LIGHTNESS_SERVER_MODEL_ID,
        .model_vendor = false,
        .element_idx = 0,
        .msg_handler = lightness_server_msg_handler,
    },
    [6] = 
	{
        .model_id = LIGHT_LIGHTNESS_SETUP_SERVER_MODEL_ID,
        .model_vendor = false,
        .element_idx = 0,
        .msg_handler = lightness_server_msg_handler,
    },
	
/************Light CTL Setup Server************/	
    [7] = /*Light CTL Server*/
	{
        .model_id = LIGHT_CTL_SERVER_MODEL_ID,
        .model_vendor = false,
        .element_idx = 0,
        .msg_handler = light_ctl_server_msg_handler,
    },
    [8] = /*Light CTL Setup Server*/
	{
        .model_id = LIGHT_CTL_SETUP_SERVER_MODEL_ID,
        .model_vendor = false,
        .element_idx = 0,
        .msg_handler = light_ctl_server_msg_handler,
    },
	
	[9] = /*Generic Level*/
	{
        .model_id = GENERIC_LEVEL_SERVER_MODEL_ID,
        .model_vendor = false,
        .element_idx = 1,
        .msg_handler = app_mesh_recv_gen_level_led_msg,
    },
	
    [10] = /*Light CTL Temperature Server*/
	{
        .model_id = LIGHT_CTL_TEMPERATURE_SERVER_MODEL_ID,
        .model_vendor = false,
        .element_idx = 1,
        .msg_handler = light_ctl_server_msg_handler,
    },
	
/************Light HSL Setup Server************/	 
    [11] = /*Light HSL Server*/
	{
        .model_id = LIGHT_HSL_SERVER_MODEL_ID,
        .model_vendor = false,
        .element_idx = 0,
        .msg_handler = light_hsl_server_msg_handler,
    },
	[12] = /*Light HSL Setup Server*/
	{
        .model_id = LIGHT_HSL_SETUP_SERVER_MODEL_ID,
        .model_vendor = false,
        .element_idx = 0,
        .msg_handler = light_hsl_server_msg_handler,
    },
	[13] = /*Generic Level*/
	{
        .model_id = GENERIC_LEVEL_SERVER_MODEL_ID,
        .model_vendor = false,
        .element_idx = 2,
        .msg_handler = app_mesh_recv_gen_level_led_msg,
    },
	
	[14] = /*Light HSL Hue Server*/
	{
        .model_id = LIGHT_HSL_HUE_SERVER_MODEL_ID,
        .model_vendor = false,
        .element_idx = 2,
        .msg_handler = light_hsl_hue_server_msg_handler,
    },	
		
	[15] = /*Generic Level*/
	{
        .model_id = GENERIC_LEVEL_SERVER_MODEL_ID,
        .model_vendor = false,
        .element_idx = 3,
        .msg_handler = app_mesh_recv_gen_level_led_msg,
    },	
	
	[16] = /*Light HSL Saturation Server*/
	{
        .model_id = LIGHT_HSL_SATURATION_SERVER_MODEL_ID,
        .model_vendor = false,
        .element_idx = 3,
        .msg_handler = light_hsl_saturation_server_msg_handler,
    },

    [17] = 
	{
        .model_id = BLOB_SERVER_VENDOR_MODEL_ID,
        .model_vendor = true,
        .element_idx = 0,
        .msg_handler = PNULL,
    },

    [18] = 
	{
        .model_id = FW_UPDATE_SERVER_VENDOR_MODEL_ID,
        .model_vendor = true,
        .element_idx = 0,
        .msg_handler = PNULL,
    },

};

//static uint8_t publish_msg_buff[32] = {0},publish_msg_len = 0,resend_count = 0;


static void sig_mesh_prov_timer_handler(void *arg)
{
    mesh_info_clear();
#ifdef MESH_USER_DATA_STORE
    app_mesh_user_data_clear();
#endif
    platform_reset_patch(0);
}




/*********************************************************************
 * @fn      app_mesh_recv_gen_onoff_led_msg
 *
 * @brief   used to check new received message whether belongs to generic on-off
 *          model or not.
 *
 * @param   ind     - message received from remote node
 *
 * @return  None.
 */
static void app_mesh_recv_gen_onoff_led_msg(mesh_model_msg_ind_t const *ind)
{
    mesh_led_on_off_msg_handler(ind);
}

/*********************************************************************
 * @fn      app_mesh_recv_gen_dtt_led_msg
 *
 * @brief   Generic Default Transition Time Server 消息处理函数
 *
 * @param   ind     - message received from remote node
 *
 * @return  None.
 */
//static void app_mesh_recv_gen_dtt_led_msg(mesh_model_msg_ind_t const *ind)
//{
//    mesh_led_dtt_msg_handler(ind);
//}




/*********************************************************************
 * @fn      app_mesh_recv_gen_ponoff_led_msg
 *
 * @brief   Generic OnPowerUp Server 消息处理函数
 *
 * @param   ind     - message received from remote node
 *
 * @return  None.
 */
static void app_mesh_recv_gen_ponoff_led_msg(mesh_model_msg_ind_t const *ind)
{
    mesh_led_ponoff_msg_handler(ind);
}

/*********************************************************************
 * @fn      app_mesh_recv_gen_level_led_msg
 *
 * @brief   Generic Level Server 消息处理函数
 *
 * @param   ind     - message received from remote node
 *
 * @return  None.
 */
static void app_mesh_recv_gen_level_led_msg(mesh_model_msg_ind_t const *ind)
{
    mesh_led_level_msg_handler(ind);
}


/*********************************************************************
 * @fn      app_mesh_recv_gen_onoff_led_msg
 *
 * @brief   used to check new received message whether belongs to lightness
 *          model or not.
 *
 * @param   ind     - message received from remote node
 *
 * @return  None.
 */
static void app_mesh_recv_lightness_msg(mesh_model_msg_ind_t const *ind)
{
    mesh_lighness_msg_handler(ind);
}

/*********************************************************************
 * @fn      app_mesh_recv_gen_onoff_led_msg
 *
 * @brief   used to check new received message whether belongs to hsl
 *          model or not.
 *
 * @param   ind     - message received from remote node
 *
 * @return  None.
 */
static void app_mesh_recv_hsl_msg(mesh_model_msg_ind_t const *ind)
{
    mesh_RGB_msg_handler(ind);   
}

/*********************************************************************
 * @fn      app_mesh_recv_gen_onoff_led_msg
 *
 * @brief   used to check new received message whether belongs to CTL
 *          model or not.
 *
 * @param   ind     - message received from remote node
 *
 * @return  None.
 */
static void app_mesh_recv_CTL_msg(mesh_model_msg_ind_t const *ind)
{
    //mesh_CTL_msg_handler(ind);  
}

static void app_mesh_recv_CTL_setup_msg(mesh_model_msg_ind_t const *ind)
{
    //mesh_CTL_msg_handler(ind);  
}

static void app_mesh_recv_CTL_temperature_msg(mesh_model_msg_ind_t const *ind)
{
    mesh_CTL_msg_handler(ind);  
}

///*********************************************************************
// * @fn      app_mesh_recv_gen_onoff_led_msg
// *
// * @brief   used to check new received message whether belongs to vendor
// *          defined model or not.
// *
// * @param   ind     - message received from remote node
// *
// * @return  None.
// */
//static void app_mesh_recv_vendor_msg(mesh_model_msg_ind_t const *ind)
//{
//    
//}

/*********************************************************************
 * @fn      mesh_callback_func
 *
 * @brief   mesh message deal callback function.
 *
 * @param   event   - event to be processed
 *
 * @return  None.
 */
static void mesh_callback_func(mesh_event_t * event)
{
    uint8_t tmp_data[16];
    static uint8_t mesh_in_network = 0;

    if(event->type != MESH_EVT_ADV_REPORT) {
        //co_printf("mesh_callback_func: %d.\r\n", event->type);
    }
    
    switch(event->type) {
        case MESH_EVT_READY:
            mesh_start();
            break;
        case MESH_EVT_STARTED:
            //app_led_init();
            //app_mesh_50Hz_check_enable();
            if(mesh_in_network) {
                mesh_proxy_ctrl(1);
            }
            break;
        case MESH_EVT_STOPPED:
            //system_sleep_enable();
            break;
        case MESH_EVT_PROXY_CTRL_STATUS:
            co_printf("MESH_EVT_PROXY_CTRL_STATUS: status is 0x%04x.\r\n", event->param.proxy_adv_status);
            break;
        case MESH_EVT_IN_NETWORK:
            co_printf("device already in network\r\n");
            mesh_in_network = 1;
            break;    
        case MESH_EVT_RESET:
            mesh_in_network = 0;
            co_printf("removed from network by provisoner.\r\n");
            mesh_info_clear();
#ifdef MESH_USER_DATA_STORE
            app_mesh_user_data_clear();
#endif
            platform_reset_patch(0);
            break;
        case MESH_EVT_PROV_PARAM_REQ:
            //co_printf("=mesh param req=\r\n");
            tmp_data[0] = 0xa8;
            tmp_data[1] = 0x01;
            tmp_data[2] = 0x71;
            memcpy(&tmp_data[3], mesh_key_pid, 4);
            memcpy(&tmp_data[7], mesh_key_bdaddr, 6);
            tmp_data[13] = 0x00;
            tmp_data[14] = 0x00;
            tmp_data[15] = 0x00;
            mesh_send_prov_param_rsp((uint8_t *)tmp_data, 0xd97478b3, 0, 0, 0, 0, 0, 0, 0, 1, 0);
            break;
        case MESH_EVT_PROV_AUTH_DATA_REQ:
            sha256_gen_auth_value((BYTE *)mesh_key_pid, (BYTE *)mesh_key_bdaddr, (BYTE *)mesh_key_secret, tmp_data);
            mesh_send_prov_auth_data_rsp(true, 16, (uint8_t *)tmp_data);
            break;
        case MESH_EVT_PROV_RESULT:
            co_printf("result=%d\r\n",event->param.prov_result.state);
            if(event->param.prov_result.state == 1) 
			{
				if(PROV_TYPE_WITH_AMANZON != gst_AppMemDB.Provision)
				{
					gst_AppMemDB.Provision = PROV_TYPE_WITH_AMANZON;
					APP_Platfrom_Set_Mesh_Default_Light_Status();
				}
				
                //os_timer_start(&sig_mesh_prov_timer, 30000, false);
            }
            break;
        case MESH_EVT_UPDATE_IND:
            //co_printf("=upd_type=%d\r\n",event->param.update_ind->upd_type);
            if(event->param.update_ind->upd_type == MESH_UPD_TYPE_APP_KEY_UPDATED)
            {
                uint16_t length = event->param.update_ind->length;
                uint8_t *data = event->param.update_ind->data;
                co_printf("APP_KEY: ");
                for(uint16_t i=2; i<(2+16); i++) {
                    co_printf("%02x", data[length-i-1]);
                }
                co_printf("\r\n");
            }
            else if(event->param.update_ind->upd_type == MESH_UPD_TYPE_NET_KEY_UPDATED)
            {
                uint16_t length = event->param.update_ind->length;
                uint8_t *data = event->param.update_ind->data;
                co_printf("NETWORK_KEY: ");
                for(uint16_t i=0; i<(0+16); i++) {
                    co_printf("%02x", data[length-i-1]);
                }
                co_printf("\r\n");
            }
            else if(event->param.update_ind->upd_type == MESH_UPD_TYPE_STATE)
            {
                uint16_t length = event->param.update_ind->length;
                uint8_t *data = event->param.update_ind->data;
                if(data[1] == 7/*M_TB_STORE_TYPE_DEV_KEY*/) {
                    co_printf("DEV_KEY: ");
                    for(uint16_t i=2; i<(2+16); i++) {
                        co_printf("%02x", data[length-i-1]);
                    }
                    co_printf("\r\n");
                }
            }
            else if(event->param.update_ind->upd_type == MESH_UPD_TYPE_PUBLI_PARAM)
            {
                uint16_t length = event->param.update_ind->length;
                uint8_t *data = event->param.update_ind->data;
                co_printf("PUBLISH PARAM: ");
                for(uint16_t i=0; i<length; i++) {
                    co_printf("%02x", data[length-i-1]);
                }
                co_printf("\r\n");
            }
            else if(event->param.update_ind->upd_type == MESH_UPD_TYPE_SUBS_LIST)
            {
                os_timer_stop(&sig_mesh_prov_timer);
                uint16_t length = event->param.update_ind->length;
                uint8_t *data = event->param.update_ind->data;
                co_printf("SUBSCRIPT LIST: ");
                for(uint16_t i=0; i<length; i++) {
                    co_printf("%02x", data[length-i-1]);
                }
                co_printf("\r\n");
            }
            else if(event->param.update_ind->upd_type == MESH_UPD_TYPE_BINDING)
            {
                uint16_t length = event->param.update_ind->length;
                uint8_t *data = event->param.update_ind->data;
                co_printf("BINDING: ");
                for(uint16_t i=0; i<length; i++) {
                    co_printf("%02x", data[length-i-1]);
                }
                co_printf("\r\n");
            }
            app_mesh_store_info_timer_start(2000);
            break;
        case MESH_EVT_RECV_MSG:
            {
                const mesh_model_msg_ind_t *ind = &(event->param.model_msg);
                //co_printf("model_id: 0x%04x\r\n", ind->model_id);
                co_printf("opcode: 0x%08x\r\n", ind->opcode);
                //co_printf("src: 0x%04x\r\n", ind->src);
                co_printf("msg: ");
                for(uint8_t i=0; i<ind->msg_len; i++)
                {
                    co_printf("%02x ", ind->msg[i]);
                }
                co_printf("\r\n");
                for(uint8_t i=0; i < sizeof(light_models)/sizeof(light_models[0]); i++)
                {
                    //if((event->param.model_msg.model_id == light_models[i].model_id)
                    //    && (event->param.model_msg.element == light_models[i].element_idx))
                    /* 
                     * lower layer of mesh will loop all models for group address (AKA message destination
                     * address) match checking, and only the primary model inside one element subscript
                     * these group address. The result is that the model_id field inside this message
                     * will only match the primary model, even the opcode is used by the other models.
                     * So only element field is checked here, and the code inside msg_handler takes
                     * responsibility to check which model has to deal with this mesasge.
                     */
                    if((ind->element == light_models[i].element_idx) && (ind->model_id == light_models[i].model_id))
                    {
                        light_models[i].msg_handler(ind);
                    }
                }
            }
            break;
        case MESH_EVT_ADV_REPORT:
            {
                #if 0
                gap_evt_adv_report_t *report = &(event->param.adv_report);
                co_printf("recv adv from: %02x-%02x-%02x-%02x-%02x-%02x\r\n", report->src_addr.addr.addr[5],
                                                                                report->src_addr.addr.addr[4],
                                                                                report->src_addr.addr.addr[3],
                                                                                report->src_addr.addr.addr[2],
                                                                                report->src_addr.addr.addr[1],
                                                                                report->src_addr.addr.addr[0]);
                for(uint16_t i=0; i<report->length; i++) {
                    co_printf("%02x ", report->data[i]);
                }
                co_printf("\r\n");
                #endif
            }
            break;
        case MESH_EVT_PROXY_END_IND:
            co_printf("MESH_EVT_PROXY_END_IND\r\n");
//            show_ke_malloc();
//            show_mem_list();
            mesh_proxy_ctrl(1);
            break;
        case MESH_EVT_FATAL_ERROR:
            co_printf("MESH_EVT_FATAL_ERROR\r\n");
            platform_reset_patch(0);
            break;
        default:
            break;
    }
}

/*********************************************************************
 * @fn      app_mesh_led_init
 *
 * @brief   init mesh model, set callback function, set feature supported by
 *          this application, add models, etc.
 *
 * @param   None.
 *
 * @return  None.
 */
void app_mesh_led_init(void)
{
    mesh_composition_cfg_t dev_cfg = 
    {
        .features = MESH_FEATURE_RELAY | MESH_FEATURE_PROXY | MESH_FEATURE_PB_GATT,
		.cid      = 0x07CE, 
        .vid      = 0x0401,
        .pid      = 0x0002,
    };

//    /* used for debug, reset Node in unprovisioned state at first */
//    flash_erase(MESH_INFO_STORE_ADDR, 0x1000);

#if 1
    mac_addr_t set_addr;
    gap_address_get(&set_addr);
    memcpy(mesh_key_bdaddr,set_addr.addr,6);
    mesh_key_bdaddr[0] += 1; // to distinguish the mesh address and general gatt address
#endif

    /* set mesh message callback function */
    mesh_set_cb_func(mesh_callback_func);

    /* set scan parameters */
    mesh_set_scan_parameter(8, 8);

    /* set scan response in proxy advertising */
	uint8_t scan_rsp_data[31] = {0};
    uint8_t rsp_len = 0;
    
    // add device name 
    scan_rsp_data[0] = (DEVICE_NAME_LEN + 1);
    scan_rsp_data[1] = 0x09;    
    memcpy(scan_rsp_data + 2, DEVICE_NAME, sizeof(DEVICE_NAME));    
	rsp_len += (DEVICE_NAME_LEN + 2);

    mesh_set_scan_rsp_data(rsp_len, scan_rsp_data);

    /* register several models */
    for(uint8_t i=0; i<sizeof(light_models)/sizeof(light_models[0]); i++) {
        mesh_add_model(&light_models[i]);
    }

    /* user to start mesh stack initialization */
    mesh_init(dev_cfg, (void *)mesh_key_bdaddr, MESH_INFO_STORE_ADDR);

    mesh_dev_led_ctrl_init();
    
    app_mesh_store_info_timer_init();
    
#ifdef SIG_MESH_TIMER
    sys_timer_init();
#endif

    os_timer_init(&sig_mesh_prov_timer, sig_mesh_prov_timer_handler, NULL);
}



