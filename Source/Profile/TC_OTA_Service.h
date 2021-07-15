#ifndef __TC_OTA_Service_H
#define __TC_OTA_Service_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * INCLUDES (包含头文件)
 */



/*
 * MACROS (宏定义)
 */

/*
 * CONSTANTS (常量定义)
 */
// Simple Profile attributes index.
enum
{
    TC_OTA_IDX_SERVICE,

    TC_OTA_IDX_DATA_DECLARATION,
    TC_OTA_IDX_DATA_VALUE,
    TC_OTA_IDX_DATA_USER_DESCRIPTION,

    TC_OTA_IDX_CTRL_DECLARATION,
    TC_OTA_IDX_CTRL_VALUE,
    TC_OTA_IDX_CTRL_CFG,
    TC_OTA_IDX_CTRL_USER_DESCRIPTION,

    TC_OTA_IDX_NB,
};





/*
 * TYPEDEFS (类型定义)
 */

/*
 * GLOBAL VARIABLES (全局变量)
 */

extern uint8_t g_TC_OTA_Svc_ID;
extern uint8_t g_ntf_TC_OTA_Ctrl_Enable;


/*
 * LOCAL VARIABLES (本地变量)
 */


/*
 * PUBLIC FUNCTIONS (全局函数)
 */
/*********************************************************************
 * @fn      TC_OTA_gatt_add_service
 *
 * @brief   Simple Profile add GATT service function.
 *			添加GATT service到ATT的数据库里面。
 *
 * @param   None.
 *
 *
 * @return  None.
 */
void TC_OTA_gatt_add_service(void);

#ifdef __cplusplus
}
#endif


#endif







