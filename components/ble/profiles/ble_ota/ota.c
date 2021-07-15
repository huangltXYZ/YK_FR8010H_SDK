
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "jump_table.h"

#include "os_mem.h"
#include "driver_plf.h"
#include "driver_system.h"
#include "driver_flash.h"
#include "driver_wdt.h"
#include "driver_uart.h"

//#include "qspi.h"
//#include "co_utils.h"
#include "gatt_api.h"
#include "gap_api.h"

#include "sys_utils.h"

#include "ota.h"
#include "ota_service.h"
#include "flash_usage_config.h"
#ifdef OTA_CRC_CHECK
#include "os_timer.h"
uint32_t Crc32CalByByte(int crc,uint8_t* ptr, int len);
uint8_t app_otas_crc_cal(uint32_t firmware_length,uint32_t new_bin_addr,uint32_t crc_data_t);
void enable_cache(uint8_t invalid_ram);
void disable_cache(void);

#endif
struct app_otas_status_t
{
    uint8_t read_opcode;
    uint16_t length;
    uint32_t base_addr;
} app_otas_status;

//static struct buffed_pkt
//{
//    uint8_t *buf;
//    uint16_t len;   //current length in the buffer
//    uint16_t malloced_pkt_num;  //num of pkts
//} first_pkt = {0};

//uint8_t first_loop = false;
//static uint16_t at_data_idx;
//static bool ota_recving_data = false;
//static uint16_t ota_recving_data_index = 0;
//static uint16_t ota_recving_expected_length = 0;
//static uint8_t *ota_recving_buffer = NULL;
//#ifdef OTA_CRC_CHECK
//static os_timer_t os_timer_ota;
//static uint32_t ota_addr_check,ota_addr_check_len = 0;
//#endif
static uint8_t ota_state = 0;

//extern uint8_t app_boot_get_storage_type(void);
//extern void app_boot_save_data(uint32_t dest, uint8_t *src, uint32_t len);
//extern void app_boot_load_data(uint8_t *dest, uint32_t src, uint32_t len);
//extern void system_set_cache_config(uint8_t value, uint8_t count_10us);
//#ifdef OTA_CRC_CHECK
//void os_timer_ota_cb(void *arg);
//#endif

__attribute__((section("ram_code"))) uint8_t app_get_ota_state(void)
{
    return ota_state;
}

__attribute__((section("ram_code"))) void app_set_ota_state(uint8_t state_flag)
{
    if(state_flag)
        ota_state = 1;
    else
        ota_state = 0;
}

static uint32_t app_otas_get_curr_firmwave_version(void)
{
    struct jump_table_t *jump_table_a = (struct jump_table_t *)0x01000000;
    if(system_regs->remap_length != 0)  // part B
    {
        struct jump_table_t *jump_table_b = (struct jump_table_t *)(0x01000000 + jump_table_a->image_size);
        return jump_table_b->firmware_version;
    }
    else        // part A
        return jump_table_a->firmware_version;
}
static uint32_t app_otas_get_curr_code_address(void)
{
    struct jump_table_t *jump_table_tmp = (struct jump_table_t *)0x01000000;
    if(system_regs->remap_length != 0)  // part B
        return jump_table_tmp->image_size;
    else    // part A
        return 0;
}
uint32_t app_otas_get_storage_address(void)
{
    struct jump_table_t *jump_table_tmp = (struct jump_table_t *)0x01000000;
    if(system_regs->remap_length != 0)      //partB, then return partA flash Addr
        return 0;
    else
        return jump_table_tmp->image_size;  //partA, then return partB flash Addr
}
static uint32_t app_otas_get_image_size(void)
{
    struct jump_table_t *jump_table_tmp = (struct jump_table_t *)0x01000000;
    return jump_table_tmp->image_size;
}

__attribute__((section("ram_code"))) void app_otas_save_data(uint32_t dest, uint8_t *src, uint32_t len)
{
    uint32_t current_remap_address, remap_size;
    current_remap_address = system_regs->remap_virtual_addr;
    remap_size = system_regs->remap_length;

    GLOBAL_INT_DISABLE();
    //*(volatile uint32_t *)0x500a0000 = 0x3c;
    //while(((*(volatile uint32_t *)0x500a0004) & 0x03) != 0x00);
    //if(__jump_table.system_option & SYSTEM_OPTION_ENABLE_CACHE)
    //{
    //    system_set_cache_config(0x60, 10);
    //}
    system_regs->remap_virtual_addr = 0;
    system_regs->remap_length = 0;

    flash_write(dest, len, src);

    system_regs->remap_virtual_addr = current_remap_address;
    system_regs->remap_length = remap_size;
    //*(volatile uint32_t *)0x500a0000 = 0x3d;
    //while(((*(volatile uint32_t *)0x500a0004) & 0x03) != 0x02);
    //if(__jump_table.system_option & SYSTEM_OPTION_ENABLE_CACHE)
    //{
    //    system_set_cache_config(0x61, 10);
    //}
    GLOBAL_INT_RESTORE();
    /*
        uint8_t *buffer = (uint8_t *)ke_malloc(len, KE_MEM_NON_RETENTION);
        flash_read(dest, len, buffer);
        for(uint32_t i = 0; i<len; i++ )
        {
            if( buffer[i] != src[i] )
            {
                co_printf("err check[%d]\r\n",i);
                while(1);
            }
        }
        ke_free((void *)buffer);
    */
}



