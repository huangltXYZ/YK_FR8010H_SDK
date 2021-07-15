#include "BSP.h"


#if (BSP_IAP_USE > 0u)

static uint8_t ota_state = 0;


__attribute__((section("ram_code"))) uint8_t app_get_ota_state(void)
{
    return ota_state;
}

__attribute__((section("ram_code"))) void app_set_ota_state(uint8_t state_flag)
{
    if(state_flag)
	{
		ota_state = 1;
	}    
    else
	{
	    ota_state = 0;
	}      
}

uint32_t app_otas_get_curr_firmwave_version(void)
{
    struct jump_table_t *jump_table_a = (struct jump_table_t *)0x01000000;
	
    if(system_regs->remap_length != 0)  // part B
    {
        struct jump_table_t *jump_table_b = (struct jump_table_t *)(0x01000000 + jump_table_a->image_size);
        return jump_table_b->firmware_version;
    }
    else// part A
	{
	    return jump_table_a->firmware_version;
	}
        
}
uint32_t app_otas_get_curr_code_address(void)
{
    struct jump_table_t *jump_table_tmp = (struct jump_table_t *)0x01000000;
    if(system_regs->remap_length != 0)  // part B
	{
	    return jump_table_tmp->image_size;
	}   
    else// part A
	{
	    return 0;
	}       
}

uint32_t app_otas_get_storage_address(void)
{
    struct jump_table_t *jump_table_tmp = (struct jump_table_t *)0x01000000;
	
    if(system_regs->remap_length != 0)      //partB, then return partA flash Addr
	{
	    return 0;
	}    
    else
	{
	    return jump_table_tmp->image_size;  //partA, then return partB flash Addr
	}      
}

uint32_t app_otas_get_image_size(void)
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

    system_regs->remap_virtual_addr = 0;
    system_regs->remap_length = 0;

    flash_write(dest, len, src);

    system_regs->remap_virtual_addr = current_remap_address;
    system_regs->remap_length = remap_size;

    GLOBAL_INT_RESTORE();
}


__attribute__((section("ram_code")))uint32_t Crc32CalByte(uint32_t crc,uint8_t* ptr, int len)
{
    int i = 0;

	while(len--)
    {
		crc = CRC32_Table[(crc ^ ptr[i]) & 0xFF] ^ (crc >> 8);
        i++;
	}
    return crc;
}


#endif

