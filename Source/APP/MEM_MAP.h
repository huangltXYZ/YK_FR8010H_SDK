#ifndef MEM_MAP
#define MEM_MAP

#ifdef __cplusplus
 extern "C" {
#endif

/*********************оƬѡ�����*********************/
#define FOR_4M_FLASH		// such as FR8016HA FR8012HA
//#define FOR_2M_FLASH		// such as FR8012HB

/*********************FLASH д����*********************/
#define FLASH_PROTECT

/*********************BLE���*********************/
#define JUMP_TABLE_STATIC_KEY_OFFSET        (0x7F000U)
#define BLE_REMOTE_SERVICE_SAVE_ADDR        (0x7E000U)
#define BLE_BONDING_INFO_SAVE_ADDR          (0x7D000U)

/*********************MESH���*********************/
#define MESH_INFO_STORE_ADDR                (0x7C000U)

/*********************����*********************/
#define RESERVED_ADDR                       (0x72000U)

/*********************�û����*********************/
#define NVM_UESR_DATA_ADDR                  (0x70000UL)/*�û��洢���ݵ���ʼ��ַ*/
#define NVM_UESR_DATA_SIZE                  (0x1000UL)/*�û��洢���ݵĴ�С*/
#define NVM_UESR_DATA_ADDR_BK               (NVM_UESR_DATA_ADDR + NVM_UESR_DATA_SIZE)/*ע�����������Ŀռ����ڱ�֤�����ھ��Ƶ�ʱ�򲻶�ʧ*/


#ifdef __cplusplus
}
#endif
  

#endif


