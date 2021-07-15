#ifndef MEM_MAP
#define MEM_MAP

#ifdef __cplusplus
 extern "C" {
#endif

/*********************芯片选型相关*********************/
#define FOR_4M_FLASH		// such as FR8016HA FR8012HA
//#define FOR_2M_FLASH		// such as FR8012HB

/*********************FLASH 写保护*********************/
#define FLASH_PROTECT

/*********************BLE相关*********************/
#define JUMP_TABLE_STATIC_KEY_OFFSET        (0x7F000U)
#define BLE_REMOTE_SERVICE_SAVE_ADDR        (0x7E000U)
#define BLE_BONDING_INFO_SAVE_ADDR          (0x7D000U)

/*********************MESH相关*********************/
#define MESH_INFO_STORE_ADDR                (0x7C000U)

/*********************保留*********************/
#define RESERVED_ADDR                       (0x72000U)

/*********************用户相关*********************/
#define NVM_UESR_DATA_ADDR                  (0x70000UL)/*用户存储数据的起始地址*/
#define NVM_UESR_DATA_SIZE                  (0x1000UL)/*用户存储数据的大小*/
#define NVM_UESR_DATA_ADDR_BK               (NVM_UESR_DATA_ADDR + NVM_UESR_DATA_SIZE)/*注：备份扇区的空间用于保证数据在卷绕的时候不丢失*/


#ifdef __cplusplus
}
#endif
  

#endif


