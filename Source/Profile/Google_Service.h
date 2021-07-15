#ifndef __GOOGLE_SERVER_H
#define __GOOGLE_SERVER_H

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
    GOOGLE_IDX_SERVICE,

    GOOGLE_IDX_CHAR1_DECLARATION,
    GOOGLE_IDX_CHAR1_VALUE,
    GOOGLE_IDX_CHAR1_USER_DESCRIPTION,

    GOOGLE_IDX_CHAR2_DECLARATION,
    GOOGLE_IDX_CHAR2_VALUE,
    GOOGLE_IDX_CHAR2_USER_DESCRIPTION,

    GOOGLE_IDX_CHAR3_DECLARATION,
    GOOGLE_IDX_CHAR3_VALUE,
    GOOGLE_IDX_CHAR3_USER_DESCRIPTION,

    GOOGLE_IDX_CHAR4_DECLARATION,
    GOOGLE_IDX_CHAR4_VALUE,
    GOOGLE_IDX_CHAR4_USER_DESCRIPTION,

    GOOGLE_IDX_CHAR5_DECLARATION,
    GOOGLE_IDX_CHAR5_VALUE,
    GOOGLE_IDX_CHAR5_USER_DESCRIPTION,

    GOOGLE_IDX_CHAR6_DECLARATION,
    GOOGLE_IDX_CHAR6_VALUE,
    GOOGLE_IDX_CHAR6_CFG,
    GOOGLE_IDX_CHAR6_USER_DESCRIPTION,

    GOOGLE_IDX_NB,
};

// Simple GATT Profile Service UUID
#define GOOGLE_SVC_UUID             (0x1800)

#define GOOGLE_CHAR3_UUID           (0xFFF3)
#define GOOGLE_CHAR4_UUID           (0xFFF4)
#define GOOGLE_CHAR5_UUID           (0xFFF5)
#define GOOGLE_CHAR6_UUID           (0xFFF6)

#define GOOGLE_CHAR2_CAPABILITY     { 0x35, 0xb1, 0xc3, 0xd8, 0x7e, 0xd4, 0x21, 0x81, 0x13, 0x46, 0xb7, 0x9c, 0x43, 0xaf, 0x83, 0x76 }
#define GOOGLE_CHAR1_CONTROL        { 0x37, 0xb1, 0xc3, 0xd8, 0x7e, 0xd4, 0x21, 0x81, 0x13, 0x46, 0xb7, 0x9c, 0x43, 0xaf, 0x83, 0x76 }
#define GOOGLE_CHAR4_NAME           { 0x36, 0xb1, 0xc3, 0xd8, 0x7e, 0xd4, 0x21, 0x81, 0x13, 0x46, 0xb7, 0x9c, 0x43, 0xaf, 0x83, 0x76 }
#define GOOGLE_CHAR3_STATUS         { 0x38, 0xb1, 0xc3, 0xd8, 0x7e, 0xd4, 0x21, 0x81, 0x13, 0x46, 0xb7, 0x9c, 0x43, 0xaf, 0x83, 0x76 }
#define GOOGLE_CHAR5_HWSW_VER		{ 0x39, 0xb1, 0xc3, 0xd8, 0x7e, 0xd4, 0x21, 0x81, 0x13, 0x46, 0xb7, 0x9c, 0x43, 0xaf, 0x83, 0x76 }
#define GOOGLE_CHAR6_NOTIFY			{ 0x40, 0xb1, 0xc3, 0xd8, 0x7e, 0xd4, 0x21, 0x81, 0x13, 0x46, 0xb7, 0x9c, 0x43, 0xaf, 0x83, 0x76 }
#define GOOGLE_CHAR2_RX_UUID        { 0xba, 0x5c, 0x49, 0xd2, 0x04, 0xa3, 0x40, 0x71, 0xa0, 0xb5, 0x35, 0x85, 0x3e, 0xb0, 0x83, 0x07 }

/*
 * TYPEDEFS (类型定义)
 */

/*
 * GLOBAL VARIABLES (全局变量)
 */

extern uint8_t g_Google_Svc_ID;
extern uint8_t g_ntf_Google_char6_Enable;
extern const gatt_attribute_t google_profile_att_table[GOOGLE_IDX_NB];

/*
 * LOCAL VARIABLES (本地变量)
 */


/*
 * PUBLIC FUNCTIONS (全局函数)
 */
/*********************************************************************
 * @fn      Google_gatt_add_service
 *
 * @brief   Simple Profile add GATT service function.
 *			添加GATT service到ATT的数据库里面。
 *
 * @param   None.
 *
 *
 * @return  None.
 */
void Google_Gatt_Add_Service(void);

#ifdef __cplusplus
}
#endif


#endif







