
#include "YK_APP.h"

/*
 * MACROS
 */

/*���� APP ����ʱ����ʱ��*/
#define APP_TIMER_START()   timer_run(TIMER0)



/*
 * CONSTANTS
 */

/*
 * TYPEDEFS
 */

/*
 * GLOBAL VARIABLES
 */
uint8_t slave_link_conidx = 0xff;
uint8_t slave_link_conidx_tmp = 0xff;
uint8_t master_link_conidx;
uint8_t tick = 1;
#ifdef MESH_QUICK_SWITCH_CTRL
uint8_t first_power_on = 0;
#endif
/*
 * LOCAL VARIABLES
 */

const struct jump_table_version_t _jump_table_version __attribute__((section("jump_table_3"))) =
{
    .firmware_version = Firmware_Version,
};

const struct jump_table_image_t _jump_table_image __attribute__((section("jump_table_1"))) =
{
    .image_type = IMAGE_TYPE_APP,
    .image_size = IMAGE_SIZE,
};


/*
 * EXTERN FUNCTIONS
 */
void pb_remote_central_init(void);
void pb_remote_connected(uint8_t conidx);
void pb_remote_disconnected(uint8_t conidx, uint8_t reason);

/*
 * PUBLIC FUNCTIONS
 */

void initial_static_memory(uint8_t act_num,
                           uint8_t adv_act_num,
                           uint8_t con_num,
                           uint8_t rx_buf_num,
                           uint16_t rx_buf_size,
                           uint8_t tx_buf_num,
                           uint16_t tx_buf_size,
                           uint16_t max_adv_size,
                           uint16_t stack_size);


/*********************************************************************
 * @fn      user_init_static_memory
 *
 * @brief   set memory.
 *
 * @param   None.
 *
 *
 * @return  None.
 */

void user_init_static_memory(void)
{
    /*
     * activity: init, adv+con, adv1, scan, pb-adv, pb-gatt (adv + con)
     * adv: adv * 2 * 2, adv1 * 2, pb-adv + 1, pb-gatt *2 * 2
     * con: init, pb-gatt*2
     */
    initial_static_memory(8, 11, 3, 24, 40, 8, 27, 37, 0x1000);
}


/*********************************************************************
 * @fn      proj_ble_gap_evt_func
 *
 * @brief   Application layer GAP event callback function. Handles GAP evnets.
 *
 * @param   event - GAP events from BLE stack.
 *
 *
 * @return  None.
 */
void proj_ble_gap_evt_func(gap_event_t *event)
{
    static uint8_t adv_stopped = false;
	
    switch(event->type)
    {
        case GAP_EVT_ADV_END:/*���¼�ֻ����BLE�㲥���������Ӻ󣬲Żᴥ��*/
        {
            co_printf("adv_end,status:0x%02x\r\n",event->param.adv_end.status);
            adv_stopped = true;
            slave_link_conidx = slave_link_conidx_tmp;
            slave_link_conidx_tmp = 0xff;
        }
        break;

        case GAP_EVT_ALL_SVC_ADDED:
        {
            co_printf("GAP_EVT_ALL_SVC_ADDED\r\n");
            if(PROV_TYPE_WITH_AMANZON != APP_Platfrom_Get_Provision_Status())
            {
                APP_Google_Start_Advertising();
                adv_stopped = false;
            }
        }
        break;

        case GAP_EVT_MASTER_CONNECT:
        {
            co_printf("master[%d],connect. link_num:%d\r\n",event->param.master_connect.conidx,gap_get_connect_num());
            gatt_mtu_exchange_req(event->param.master_connect.conidx);
            pb_remote_connected(event->param.master_connect.conidx);

        }break;

        case GAP_EVT_SLAVE_RAW_CONNECT:
        {
            co_printf("GAP_EVT_SLAVE_RAW_CONNECT\r\n");
            slave_link_conidx_tmp = event->param.slave_connect.conidx;

        }break;

        case GAP_EVT_SLAVE_CONNECT:
        {
            co_printf("slave[%d],connect. link_num:%d\r\n",event->param.slave_connect.conidx,gap_get_connect_num());
            show_reg((void *)&event->param.slave_connect.peer_addr, 6, 1);
        }break;

        case GAP_EVT_DISCONNECT:
        {
            //gap_bond_manager_info_clr("\x0C\x0C\x0C\x0C\x0C\x0B", 0);
            co_printf("Link[%d] disconnect,reason:0x%02X\r\n",event->param.disconnect.conidx
                      ,event->param.disconnect.reason);
#ifdef USER_MEM_API_ENABLE
            show_mem_list();
            //show_msg_list();
            show_ke_malloc();
#endif
			app_set_ota_state(0);
            pb_remote_disconnected(event->param.disconnect.conidx, event->param.disconnect.reason);

            if(PROV_TYPE_WITH_AMANZON != APP_Platfrom_Get_Provision_Status())
            {
                if((adv_stopped == true) && (slave_link_conidx == event->param.disconnect.conidx))
                {
                    slave_link_conidx = 0xff;
                    adv_stopped = false;

                    APP_Google_Start_Advertising();
                }
            }
        }break;

        default:
            break;
    }
}

/*********************************************************************
 * @fn      user_custom_parameters
 *
 * @brief   initialize several parameters, this function will be called
 *          at the beginning of the program.
 *
 * @param   None.
 *
 *
 * @return  None.
 */
void user_custom_parameters(void)
{
    struct chip_unique_id_t id_data;

    retry_handshake(0);

    efuse_get_chip_unique_id(&id_data);
    __jump_table.addr.addr[0] = 0xBD;
    __jump_table.addr.addr[1] = 0xAD;
    __jump_table.addr.addr[2] = 0xD0;
    __jump_table.addr.addr[3] = 0xF0;
    __jump_table.addr.addr[4] = 0x17;
    __jump_table.addr.addr[5] = 0xc0;

    id_data.unique_id[5] |= 0xc0; // random addr->static addr type:the top two bit must be 1.

#if 0
    extern uint8_t mesh_key_bdaddr[];
    memcpy(__jump_table.addr.addr, mesh_key_bdaddr, 6);
#else
    memcpy(__jump_table.addr.addr, id_data.unique_id, 6);
#endif

    __jump_table.system_clk = SYSTEM_SYS_CLK_48M;
    //__jump_table.diag_port = 0x83000000;
    jump_table_set_static_keys_store_offset(JUMP_TABLE_STATIC_KEY_OFFSET);
}

/*********************************************************************
 * @fn      user_entry_before_sleep_imp
 *
 * @brief   Before system goes to sleep mode, user_entry_before_sleep_imp()
 *          will be called, MCU peripherals can be configured properly before
 *          system goes to sleep, for example, some MCU peripherals need to be
 *          used during the system is in sleep mode.
 *
 * @param   None.
 *
 *
 * @return  None.
 */
__attribute__((section("ram_code"))) void user_entry_before_sleep_imp(void)
{
    uart_putc_noint_no_wait(UART1, 's');
}

/*********************************************************************
 * @fn      user_entry_after_sleep_imp
 *
 * @brief   When system wakes up from sleep, user_entry_after_sleep_imp()
 *          will be called, MCU peripherals need to be initialized again,
 *          this can be done in user_entry_after_sleep_imp(). MCU peripherals
 *          status will not be kept during the sleep.
 *
 * @param   None.
 *
 *
 * @return  None.
 */
__attribute__((section("ram_code"))) void user_entry_after_sleep_imp(void)
{
    /* set PA2 and PA3 for AT command interface */
    system_set_port_pull(GPIO_PA2, true);
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_2, PORTA2_FUNC_UART1_RXD);
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_3, PORTA3_FUNC_UART1_TXD);

    if(__jump_table.system_option & SYSTEM_OPTION_ENABLE_HCI_MODE)
    {
        system_set_port_pull(GPIO_PA4, true);
        system_set_port_mux(GPIO_PORT_A, GPIO_BIT_4, PORTA4_FUNC_UART0_RXD);
        system_set_port_mux(GPIO_PORT_A, GPIO_BIT_5, PORTA5_FUNC_UART0_TXD);
        uart_init(UART0, BAUD_RATE_115200);
        NVIC_EnableIRQ(UART0_IRQn);

        system_sleep_disable();
    }

    uart_init(UART1, BAUD_RATE_115200);
    NVIC_EnableIRQ(UART1_IRQn);

    uart_putc_noint_no_wait(UART1, 'w');

    NVIC_EnableIRQ(PMU_IRQn);
}

/*********************************************************************
 * @fn      user_entry_before_ble_init
 *
 * @brief   Code to be executed before BLE stack to be initialized.
 *          Power mode configurations, PMU part driver interrupt enable, MCU
 *          peripherals init, etc.
 *
 * @param   None.
 *
 *
 * @return  None.
 */
void user_entry_before_ble_init(void)
{
    /* set system power supply in BUCK mode */
    pmu_set_sys_power_mode(PMU_SYS_POW_LDO);
#ifdef FLASH_PROTECT
    flash_protect_enable(1);
#endif
    pmu_enable_irq(PMU_ISR_BIT_ACOK
                   | PMU_ISR_BIT_ACOFF
                   | PMU_ISR_BIT_ONKEY_PO
                   | PMU_ISR_BIT_OTP
                   | PMU_ISR_BIT_LVD
                   | PMU_ISR_BIT_BAT
                   | PMU_ISR_BIT_ONKEY_HIGH);

    BSP_Init();
#ifdef MESH_QUICK_SWITCH_CTRL

    //co_printf("first_power_on=%d\r\n",first_power_on);
    //if(first_power_on)
    {
        app_mesh_store_switch_time();
        co_delay_100us(5000); // 500ms
    }
#endif
}

/*********************************************************************
 * @fn      user_entry_after_ble_init
 *
 * @brief   Main entrancy of user application. This function is called after BLE stack
 *          is initialized, and all the application code will be executed from here.
 *          In that case, application layer initializtion can be startd here.
 *
 * @param   None.
 *
 *
 * @return  None.
 */
void user_entry_after_ble_init(void)
{
    co_printf("user_entry_after_ble_init\r\n");

    if(ool_read(PMU_REG_SYSTEM_STATUS) == 0xc6)
    {
        co_printf("wake up+++++++++++++++++++++++++\r\n");
    }
    else
    {
        co_printf("Normal--------------------------\r\n");
    }
	co_printf("firmware_version2:%d\r\n",__jump_table.firmware_version);
    mac_addr_t addr;
    gap_address_get(&addr);
    show_reg(&addr.addr[0], 6, 1);

    system_sleep_disable();

    //system_set_tx_power(RF_TX_POWER_NEG_16dBm);

#if 0
    pmu_set_pin_pull(GPIO_PORT_D,(1<<GPIO_BIT_6),true);
    pmu_port_wakeup_func_set(GPIO_PD6);
    button_init(GPIO_PD6);
    NVIC_EnableIRQ(PMU_IRQn);
#endif

    // set local device name
    gap_set_dev_name((U8*)DEVICE_NAME, strlen(DEVICE_NAME));
    gap_set_cb_func(proj_ble_gap_evt_func);

#ifdef MESH_QUICK_SWITCH_CTRL
    app_mesh_clear_switch_time();
#endif

    YK_APP_Init();

    /*��������ʱ����ʱ��*/
    APP_TIMER_START();
}

