#ifndef __APP_Platfrom_H
#define __APP_Platfrom_H


#ifdef __cplusplus
 extern "C" {
#endif

   




void APP_Platfrom_Init(void);    
void APP_Platfrom_Task(void);
void APP_Platfrom_TimeProc(U32 ulPeriod);
void APP_Platfrom_Set_Provision_Status(PROV_TYPE ProvType);
PROV_TYPE APP_Platfrom_Get_Provision_Status(void);
void APP_Platfrom_Set_Mesh_Default_Light_Status(void);

#ifdef __cplusplus
}
#endif
  
   
#endif


