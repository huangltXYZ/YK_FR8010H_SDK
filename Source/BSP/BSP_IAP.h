#ifndef __BSP_IAP_H
#define __BSP_IAP_H

#ifdef __cplusplus
 extern "C" {
#endif




uint8_t app_get_ota_state(void);
void app_set_ota_state(uint8_t state_flag);
uint32_t app_otas_get_curr_firmwave_version(void);
uint32_t app_otas_get_curr_code_address(void);
uint32_t app_otas_get_storage_address(void);
uint32_t app_otas_get_image_size(void);
void app_otas_save_data(uint32_t dest, uint8_t *src, uint32_t len);

uint32_t Crc32CalByte(uint32_t crc,uint8_t* ptr, int len);


#ifdef __cplusplus
}
#endif
  
   
#endif

