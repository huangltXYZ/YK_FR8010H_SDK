
#include "YK_APP.h"

/******************************* Google GATT Profile defination *******************************/
/*Google GATT Profile Service UUID: 0x1800*/
const uint8_t Google_svc_uuid[] = UUID16_ARR(GOOGLE_SVC_UUID);

/******************************* Characteristic 1 defination *******************************/
#define GOOGLE_CHAR1_VALUE_LEN  20
uint8_t Google_char1_value[GOOGLE_CHAR1_VALUE_LEN] = {0};
// Characteristic 1 User Description
#define GOOGLE_CHAR1_DESC_LEN   17
const uint8_t Google_char1_desc[GOOGLE_CHAR1_DESC_LEN] = "Characteristic 1";

/******************************* Characteristic 2 defination *******************************/
#define GOOGLE_CHAR2_VALUE_LEN  20
uint8_t Google_char2_value[GOOGLE_CHAR2_VALUE_LEN] = {0};
// Characteristic 2 User Description
#define GOOGLE_CHAR2_DESC_LEN   17
const uint8_t Google_char2_desc[GOOGLE_CHAR2_DESC_LEN] = "Characteristic 2";

/******************************* Characteristic 3 defination *******************************/
#define GOOGLE_CHAR3_VALUE_LEN  30
uint8_t Google_char3_value[GOOGLE_CHAR3_VALUE_LEN] = {0};
// Characteristic 3 User Description
#define GOOGLE_CHAR3_DESC_LEN   17
const uint8_t Google_char3_desc[GOOGLE_CHAR3_DESC_LEN] = "Characteristic 3";

/******************************* Characteristic 4 defination *******************************/
#define GOOGLE_CHAR4_VALUE_LEN  40
uint8_t Google_char4_value[GOOGLE_CHAR4_VALUE_LEN] = {0};
// Characteristic 4 User Description
#define GOOGLE_CHAR4_DESC_LEN   17
const uint8_t Google_char4_desc[GOOGLE_CHAR4_DESC_LEN] = "Characteristic 4";

/******************************* Characteristic 5 defination *******************************/
uint8_t Google_char5_uuid[UUID_SIZE_2] =
{
    LO_UINT16(GOOGLE_CHAR5_UUID), HI_UINT16(GOOGLE_CHAR5_UUID)
};
// Characteristic 5 data
#define GOOGLE_CHAR5_VALUE_LEN  50
uint8_t Google_char5_value[GOOGLE_CHAR5_VALUE_LEN] = {0};
// Characteristic 5 User Description
#define GOOGLE_CHAR5_DESC_LEN   17
const uint8_t Google_char5_desc[GOOGLE_CHAR5_DESC_LEN] = "Characteristic 5";

/******************************* Characteristic 6 defination *******************************/
// Characteristic 6 UUID: 0xFFF5
uint8_t Google_char6_uuid[UUID_SIZE_2] =
{
    LO_UINT16(GOOGLE_CHAR5_UUID), HI_UINT16(GOOGLE_CHAR6_UUID)
};

// Characteristic 6 client characteristic configuration
#define GOOGLE_CHAR6_CCC_LEN   2
uint8_t Google_char6_ccc[GOOGLE_CHAR6_CCC_LEN] = {0};

// Characteristic 6 data
#define GOOGLE_CHAR6_VALUE_LEN  50
uint8_t Google_char6_value[GOOGLE_CHAR6_VALUE_LEN] = {0};
// Characteristic 6 User Description
#define GOOGLE_CHAR6_DESC_LEN   17
const uint8_t Google_char6_desc[GOOGLE_CHAR6_DESC_LEN] = "Characteristic 5";
/*
 * TYPEDEFS (类型定义)
 */

/*
 * GLOBAL VARIABLES (全局变量)
 */
uint8_t g_Google_Svc_ID = 0;
uint8_t g_ntf_Google_char6_Enable = 0;

/*
 * LOCAL VARIABLES (本地变量)
 */
static gatt_service_t Google_Profile_Svc;

/*********************************************************************
 * Profile Attributes - Table
 * 每一项都是一个attribute的定义。
 * 第一个attribute为Service 的的定义。
 * 每一个特征值(characteristic)的定义，都至少包含三个attribute的定义；
 * 1. 特征值声明(Characteristic Declaration)
 * 2. 特征值的值(Characteristic value)
 * 3. 特征值描述符(Characteristic description)
 * 如果有notification 或者indication 的功能，则会包含四个attribute的定义，除了前面定义的三个，还会有一个特征值客户端配置(client characteristic configuration)。
 *
 */
static const gatt_attribute_t s_Google_Profile_Att_Tbl[GOOGLE_IDX_NB] =
{
    // Simple gatt Service Declaration
    [GOOGLE_IDX_SERVICE] =   
	{
        { UUID_SIZE_2, UUID16_ARR(GATT_PRIMARY_SERVICE_UUID) },     /* UUID */
        GATT_PROP_READ,                                             /* Permissions */
        UUID_SIZE_2,                                                /* Max size of the value */     /* Service UUID size in service declaration */
        (uint8_t*)Google_svc_uuid,                                      /* Value of the attribute */    /* Service UUID value in service declaration */
    },

    // Characteristic 1 Declaration
    [GOOGLE_IDX_CHAR1_DECLARATION] =   
	{
        { UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID) },           /* UUID */
        GATT_PROP_READ,                                             /* Permissions */
        0,                                                          /* Max size of the value */
        NULL,                                                       /* Value of the attribute */
    },
    // Characteristic 1 Value
    [GOOGLE_IDX_CHAR1_VALUE] =   
	{
        { UUID_SIZE_16, GOOGLE_CHAR1_CONTROL },                			/* UUID */
        GATT_PROP_WRITE_CMD | GATT_PROP_WRITE_REQ,                           /* Permissions */
        GOOGLE_CHAR1_VALUE_LEN,                                         /* Max size of the value */
        NULL,                                                       /* Value of the attribute */    /* Can assign a buffer here, or can be assigned in the application by user */
    },

    // Characteristic 1 User Description
    [GOOGLE_IDX_CHAR1_USER_DESCRIPTION] =   
	{
        { UUID_SIZE_2, UUID16_ARR(GATT_CHAR_USER_DESC_UUID) },      /* UUID */
        GATT_PROP_READ,                                             /* Permissions */
        GOOGLE_CHAR1_DESC_LEN,                                          /* Max size of the value */
        (uint8_t *)Google_char1_desc,                                   /* Value of the attribute */
    },

    // Characteristic 2 Declaration
    [GOOGLE_IDX_CHAR2_DECLARATION] =   
	{
        { UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID) },           /* UUID */
        GATT_PROP_READ,                                             /* Permissions */
        0,                                                          /* Max size of the value */
        NULL,                                                       /* Value of the attribute */
    },
    // Characteristic 2 Value
    [GOOGLE_IDX_CHAR2_VALUE] =   
	{
        { UUID_SIZE_16, GOOGLE_CHAR2_CAPABILITY },                 /* UUID */
        GATT_PROP_READ,                                             /* Permissions */
        GOOGLE_CHAR2_VALUE_LEN,                                         /* Max size of the value */
        NULL,                                                       /* Value of the attribute */	/* Can assign a buffer here, or can be assigned in the application by user */
    },
    // Characteristic 2 User Description
    [GOOGLE_IDX_CHAR2_USER_DESCRIPTION] =   
	{
        { UUID_SIZE_2, UUID16_ARR(GATT_CHAR_USER_DESC_UUID) },       /* UUID */
        GATT_PROP_READ,                                             /* Permissions */
        GOOGLE_CHAR2_DESC_LEN,                                          /* Max size of the value */
        (uint8_t *)Google_char2_desc,                                   /* Value of the attribute */
    },


    // Characteristic 3 Declaration
    [GOOGLE_IDX_CHAR3_DECLARATION] =   
	{
        { UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID) },           /* UUID */
        GATT_PROP_READ,                                             /* Permissions */
        0,                                                          /* Max size of the value */
        NULL,                                                       /* Value of the attribute */
    },
    // Characteristic 3 Value
    [GOOGLE_IDX_CHAR3_VALUE] =   
	{
        { UUID_SIZE_16, GOOGLE_CHAR3_STATUS },                 /* UUID */
        GATT_PROP_READ,                                            /* Permissions */
        GOOGLE_CHAR3_VALUE_LEN,                                         /* Max size of the value */
        NULL,                                                       /* Value of the attribute */    /* Can assign a buffer here, or can be assigned in the application by user */
    },
    // Characteristic 3 User Description
    [GOOGLE_IDX_CHAR3_USER_DESCRIPTION] =   
	{
        { UUID_SIZE_2, UUID16_ARR(GATT_CHAR_USER_DESC_UUID) },      /* UUID */
        GATT_PROP_READ,                                             /* Permissions */
        GOOGLE_CHAR3_DESC_LEN,                                          /* Max size of the value */
        (uint8_t *)Google_char3_desc,                                   /* Value of the attribute */
    },


    // Characteristic 4 Declaration
    [GOOGLE_IDX_CHAR4_DECLARATION] =   
	{
        { UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID) },           /* UUID */
        GATT_PROP_READ,                                             /* Permissions */
        0,                                                          /* Max size of the value */
        NULL,                                                       /* Value of the attribute */
    },
    // Characteristic 4 Value
    [GOOGLE_IDX_CHAR4_VALUE] =   
	{
        { UUID_SIZE_16, GOOGLE_CHAR4_NAME },                 			/* UUID */
        GATT_PROP_READ,                          					/* Permissions */
        GOOGLE_CHAR4_VALUE_LEN,                                         /* Max size of the value */
        NULL,                                                       /* Value of the attribute */    /* Can assign a buffer here, or can be assigned in the application by user */
    },

    // Characteristic 4 User Description
    [GOOGLE_IDX_CHAR4_USER_DESCRIPTION] =   
	{
        { UUID_SIZE_2, UUID16_ARR(GATT_CHAR_USER_DESC_UUID) },      /* UUID */
        GATT_PROP_READ,                                             /* Permissions */
        GOOGLE_CHAR4_DESC_LEN,                                          /* Max size of the value */
        (uint8_t *)Google_char4_desc,                                   /* Value of the attribute */
    },


    // Characteristic 5 Declaration
    [GOOGLE_IDX_CHAR5_DECLARATION] =   
	{
        { UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID) },           /* UUID */
        GATT_PROP_READ,                                             /* Permissions */
        0,                                                          /* Max size of the value */
        NULL,                                                       /* Value of the attribute */
    },
    // Characteristic 5 Value
    [GOOGLE_IDX_CHAR5_VALUE] =   
	{
        { UUID_SIZE_16, GOOGLE_CHAR5_HWSW_VER },                 /* UUID */
        GATT_PROP_READ,             /* Permissions */
        GOOGLE_CHAR5_VALUE_LEN,                                         /* Max size of the value */
        NULL,                                                       /* Value of the attribute */    /* Can assign a buffer here, or can be assigned in the application by user */
    },
    // Characteristic 5 User Description
    [GOOGLE_IDX_CHAR5_USER_DESCRIPTION] =   
	{
        { UUID_SIZE_2, UUID16_ARR(GATT_CHAR_USER_DESC_UUID) },      /* UUID */
        GATT_PROP_READ,                                             /* Permissions */
        GOOGLE_CHAR5_DESC_LEN,                                          /* Max size of the value */
        (uint8_t *)Google_char5_desc,                                   /* Value of the attribute */
    },

    // Characteristic 5 Declaration
    [GOOGLE_IDX_CHAR6_DECLARATION] =   
	{
        { UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID) },           /* UUID */
        GATT_PROP_READ,                                             /* Permissions */
        0,                                                          /* Max size of the value */
        NULL,                                                       /* Value of the attribute */
    },
    // Characteristic 6 Value
    [GOOGLE_IDX_CHAR6_VALUE] =   
	{
        { UUID_SIZE_16, GOOGLE_CHAR6_NOTIFY },                			/* UUID */
        GATT_PROP_NOTI,             								/* Permissions */
        GOOGLE_CHAR6_VALUE_LEN,                                         /* Max size of the value */
        NULL,                                                       /* Value of the attribute */    /* Can assign a buffer here, or can be assigned in the application by user */
    },
    // Characteristic 6 client characteristic configuration
    [GOOGLE_IDX_CHAR6_CFG] =   
	{
        { UUID_SIZE_2, UUID16_ARR(GATT_CLIENT_CHAR_CFG_UUID) },     /* UUID */
        GATT_PROP_READ | GATT_PROP_WRITE,                           /* Permissions */
        GOOGLE_CHAR6_CCC_LEN,                                           /* Max size of the value */
        NULL,                                                       /* Value of the attribute */    /* Can assign a buffer here, or can be assigned in the application by user */
    },
    // Characteristic 6 User Description
    [GOOGLE_IDX_CHAR6_USER_DESCRIPTION] =   
	{
        { UUID_SIZE_2, UUID16_ARR(GATT_CHAR_USER_DESC_UUID) },      /* UUID */
        GATT_PROP_READ,                                             /* Permissions */
        GOOGLE_CHAR6_DESC_LEN,                                          /* Max size of the value */
        (uint8_t *)Google_char6_desc,                                   /* Value of the attribute */
    },
};

void ntf_data(uint8_t con_idx,uint8_t att_idx,uint8_t *data,uint16_t len)
{
    gatt_ntf_t ntf_att;

    ntf_att.att_idx  = att_idx;
    ntf_att.conidx   = con_idx;
    ntf_att.svc_id   = g_Google_Svc_ID;
    ntf_att.data_len = len;
    ntf_att.p_data   = data;

    gatt_notification(ntf_att);
}
/*********************************************************************
 * @fn      Google_gatt_msg_handler
 *
 * @brief   Simple Profile callback funtion for GATT messages. GATT read/write
 *			operations are handeled here.
 *
 * @param   p_msg       - GATT messages from GATT layer.
 *
 * @return  uint16_t    - Length of handled message.
 */
static uint16_t Google_gatt_msg_handler(gatt_msg_t *p_msg)
{
    uint8_t ucReadLen = 0;
    switch(p_msg->msg_evt)
    {
        case GATTC_MSG_READ_REQ:
        {
            /*********************************************************************
             * @brief   Simple Profile user application handles read request in this callback.
             *			应用层在这个回调函数里面处理读的请求。
             *
             * @param   p_msg->param.msg.p_msg_data  - the pointer to read buffer. NOTE: It's just a pointer from lower layer, please create the buffer in application layer.
             *					  指向读缓冲区的指针。 请注意这只是一个指针，请在应用程序中分配缓冲区. 为输出函数, 因此为指针的指针.
             *          p_msg->param.msg.msg_len     - the pointer to the length of read buffer. Application to assign it.
             *                    读缓冲区的长度，用户应用程序去给它赋值.
             *          p_msg->att_idx - index of the attribute value in it's attribute table.
             *					  Attribute的偏移量.
             *
             * @return  读请求的长度.
             */

            if(p_msg->att_idx == GOOGLE_IDX_CHAR1_VALUE)
            {
                memcpy(p_msg->param.msg.p_msg_data, "CHAR1_VALUE", strlen("CHAR1_VALUE"));
				
                return strlen("CHAR1_VALUE");
            }
            else if(p_msg->att_idx == GOOGLE_IDX_CHAR2_VALUE)/*读取设备信息/能力*/
            {
                p_msg->param.msg.p_msg_data[0]=gst_AppGoogleTB.pDeviceTraits->OnOff;
                p_msg->param.msg.p_msg_data[1]=gst_AppGoogleTB.pDeviceTraits->Brightness;
                p_msg->param.msg.p_msg_data[2]=gst_AppGoogleTB.pDeviceTraits->Temperature;
                p_msg->param.msg.p_msg_data[3]=gst_AppGoogleTB.pDeviceTraits->RGB;
                p_msg->param.msg.p_msg_data[4]=(U8)((gst_AppGoogleTB.pDeviceTraits->TemperatureMin & 0xFF00) >> 8);
                p_msg->param.msg.p_msg_data[5]=(U8)((gst_AppGoogleTB.pDeviceTraits->TemperatureMin & 0x00FF) >> 0);
                p_msg->param.msg.p_msg_data[6]=(U8)((gst_AppGoogleTB.pDeviceTraits->TemperatureMax & 0xFF00) >> 8);
                p_msg->param.msg.p_msg_data[7]=(U8)((gst_AppGoogleTB.pDeviceTraits->TemperatureMax & 0x00FF) >> 0);
                co_printf("\r\nRead Device Info:");
                return 8;
            }

            else if(p_msg->att_idx == GOOGLE_IDX_CHAR3_VALUE)/*Read device state*/
            {
                if(PROV_TYPE_WITH_GOOGLE == APP_Platfrom_Get_Provision_Status())
                {
                    return 0;
                }

                ucReadLen = sizeof(gst_AppGoogleTB.DeviceStatus);
                memcpy(p_msg->param.msg.p_msg_data, &gst_AppGoogleTB.DeviceStatus, ucReadLen);


                co_printf("Read Device Status:%d\r\n",ucReadLen);
                co_printf("Onoff:%d\r\n",gst_AppGoogleTB.DeviceStatus.OnOff);
                co_printf("Lightness:%d\r\n",gst_AppGoogleTB.DeviceStatus.Lightness);

                return ucReadLen;
            }
            else if(p_msg->att_idx == GOOGLE_IDX_CHAR4_VALUE)/*Read device name*/
            {
                ucReadLen = gst_AppGoogleTB.DeviceNameLen;
                memcpy(p_msg->param.msg.p_msg_data, gst_AppGoogleTB.pDeviceName, ucReadLen);
                co_printf("\r\nRead Device name:%d",ucReadLen);
				
                return ucReadLen;
            }
            else if(p_msg->att_idx == GOOGLE_IDX_CHAR5_VALUE)/*Read device version*/
            {
                ucReadLen = sizeof(gst_AppGoogleTB.DeviceVersion);
                memcpy(p_msg->param.msg.p_msg_data, gst_AppGoogleTB.DeviceVersion, ucReadLen);
                co_printf("\r\nRead Device version:%d",ucReadLen);
				
                return ucReadLen;
            }
			
			else if(p_msg->att_idx == GOOGLE_IDX_CHAR6_CFG)/*Read CCCD*/
			{
				co_printf("\r\nRead CCCD:%d", Google_char6_ccc[0]);
				
				memcpy(p_msg->param.msg.p_msg_data, Google_char6_ccc, GOOGLE_CHAR6_CCC_LEN);
				p_msg->param.msg.msg_len = GOOGLE_CHAR6_CCC_LEN;
				
				return GOOGLE_CHAR6_CCC_LEN;
			}
        }
        break;

        case GATTC_MSG_WRITE_REQ:
        {
            /*********************************************************************
             * @brief   Simple Profile user application handles write request in this callback.
             *			应用层在这个回调函数里面处理写的请求。
             *
             * @param   p_msg->param.msg.p_msg_data   - the buffer for write
             *			              写操作的数据.
             *
             *          p_msg->param.msg.msg_len      - the length of write buffer.
             *                        写缓冲区的长度.
             *          att_idx     - index of the attribute value in it's attribute table.
             *					      Attribute的偏移量.
             *
             * @return  写请求的长度.
             */
            if (p_msg->att_idx == GOOGLE_IDX_CHAR1_VALUE)
            {
                co_printf("char1_recv:");
                show_reg(p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len,1);
                gst_AppGoogleTB.ConIdx = p_msg->conn_idx;
                gst_AppGoogleTB.AttIdx = p_msg->att_idx;
                gst_AppGoogleTB.ucRxLen = p_msg->param.msg.msg_len;
                memcpy(gst_AppGoogleTB.RxBuff, p_msg->param.msg.p_msg_data, gst_AppGoogleTB.ucRxLen);

                YK_APP_Set_Evt(AppTaskTCB[APP_TASK_GOOGLE].Handler, EVT_Google_RECV_FRAME);
            }
            else if (p_msg->att_idx == GOOGLE_IDX_CHAR2_VALUE)
            {
                co_printf("char2_recv:");
                show_reg(p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len,1);
            }
            else if (p_msg->att_idx == GOOGLE_IDX_CHAR3_VALUE)
            {
                co_printf("char3_recv:");
                show_reg(p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len,1);
            }
            else if (p_msg->att_idx == GOOGLE_IDX_CHAR5_VALUE)
            {
                co_printf("char5_recv:");
                show_reg(p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len,1);
            }
            else if (p_msg->att_idx == GOOGLE_IDX_CHAR6_CFG)
            {
                co_printf("char6_ntf_enable:");
                show_reg(p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len,1);
                if(p_msg->param.msg.p_msg_data[0] & 0x1)
				{
				    g_ntf_Google_char6_Enable = 1;
				}
				else
				{
					g_ntf_Google_char6_Enable = 0;
				}
                    
                memcpy(Google_char6_ccc, p_msg->param.msg.p_msg_data, GOOGLE_CHAR6_CCC_LEN);
            }
        }
        break;
        case GATTC_MSG_LINK_CREATE:
		{
			g_ntf_Google_char6_Enable = 0;
			
		    co_printf("link_created\r\n");
			
		}break;
		
        case GATTC_MSG_LINK_LOST:
		{
		    co_printf("link_lost\r\n");
            g_ntf_Google_char6_Enable = 0;
			
		}break;
		
        default:
            break;
    }
    return p_msg->param.msg.msg_len;
}

/*********************************************************************
 * @fn      Google_gatt_add_service
 *
 * @brief   Google Profile add GATT service function.
 *			添加GATT service到ATT的数据库里面。
 *
 * @param   None.
 *
 *
 * @return  None.
 */
void Google_Gatt_Add_Service(void)
{
    Google_Profile_Svc.p_att_tb         = s_Google_Profile_Att_Tbl;
    Google_Profile_Svc.att_nb           = GOOGLE_IDX_NB;
    Google_Profile_Svc.gatt_msg_handler = Google_gatt_msg_handler;

    g_Google_Svc_ID = gatt_add_service(&Google_Profile_Svc);
}



