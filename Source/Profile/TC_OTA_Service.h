#ifndef __TC_OTA_Service_H
#define __TC_OTA_Service_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * INCLUDES (����ͷ�ļ�)
 */



/*
 * MACROS (�궨��)
 */

/*
 * CONSTANTS (��������)
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
 * TYPEDEFS (���Ͷ���)
 */

/*
 * GLOBAL VARIABLES (ȫ�ֱ���)
 */

extern uint8_t g_TC_OTA_Svc_ID;
extern uint8_t g_ntf_TC_OTA_Ctrl_Enable;


/*
 * LOCAL VARIABLES (���ر���)
 */


/*
 * PUBLIC FUNCTIONS (ȫ�ֺ���)
 */
/*********************************************************************
 * @fn      TC_OTA_gatt_add_service
 *
 * @brief   Simple Profile add GATT service function.
 *			���GATT service��ATT�����ݿ����档
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







