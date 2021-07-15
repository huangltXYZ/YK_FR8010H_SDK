#include "YK_APP.h"

#define APP_BLE_PRINTF(...)    printf(__VA_ARGS__)



/*****************************************************************************
Function name :  APP_BLE_Init
Description:	 BLE相关初始化
Input:           void
Return: 	     void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   ########  build this module
******************************************************************************/
void APP_BLE_Init(void)
{
	    // Initialize security related settings.
    gap_security_param_t param =
    {
        .mitm = false,
        .ble_secure_conn = false,
        .io_cap = GAP_IO_CAP_NO_INPUT_NO_OUTPUT,
        .pair_init_mode = GAP_PAIRING_MODE_WAIT_FOR_REQ,
        .bond_auth = false,
        .password = 0,
    };
    
    gap_security_param_init(&param);

    gap_bond_manager_init(BLE_BONDING_INFO_SAVE_ADDR, BLE_REMOTE_SERVICE_SAVE_ADDR, 8, true);
    //gap_bond_manager_delete_all();
    
    mac_addr_t addr;
    gap_address_get(&addr);
    APP_BLE_PRINTF("Local BDADDR: 0x%2X%2X%2X%2X%2X%2X\r\n", addr.addr[0], addr.addr[1], addr.addr[2], addr.addr[3], addr.addr[4], addr.addr[5]);
    
    // Adding services to database
    Google_Gatt_Add_Service();  
	
	TC_OTA_gatt_add_service();
}


