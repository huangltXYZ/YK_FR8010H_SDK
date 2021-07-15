
#include "YK_APP.h"

/******************************* TC_OTA GATT Profile defination *******************************/

/*Simple GATT Profile Service UUID*/ 
#define TC_OTA_SVC_UUID    (0xFD00)
#define TC_OTA_DATA_UUID   (0xFD01)
#define TC_OTA_CTRL_UUID   (0xFD02)


/*TC_OTA GATT Profile Service UUID: 0x1800*/
const uint8_t TC_OTA_Svc_uuid[] = UUID16_ARR(TC_OTA_SVC_UUID);

/******************************* DATA Characteristic defination *******************************/
#define TC_OTA_DATA_CHAR_VALUE_LEN    (512U)
uint8_t TC_OTA_Data_Char_Value[TC_OTA_DATA_CHAR_VALUE_LEN] = {0};

/*Char User Description*/ 
#define TC_OTA_DATA_CHAR_DESC_LEN   14
const uint8_t TC_OTA_Data_Char_Desc[TC_OTA_DATA_CHAR_DESC_LEN] = "OTA Data Char";

/******************************* CTRL Characteristic defination *******************************/
/*CCCD*/ 
#define TC_OTA_CTRL_CHAR_CCC_LEN   2
uint8_t TC_OTA_Ctrl_Char_ccc[TC_OTA_CTRL_CHAR_CCC_LEN] = {0};

/*DATA*/
#define TC_OTA_CTRL_CHAR_VALUE_LEN  100
uint8_t TC_OTA_Ctrl_Char_Value[TC_OTA_CTRL_CHAR_VALUE_LEN] = {0};

/*Char User Description*/ 
#define TC_OTA_CTRL_CHAR_DESC_LEN   14
const uint8_t TC_OTA_Ctrl_Char_Desc[TC_OTA_CTRL_CHAR_DESC_LEN] = "OTA Ctrl Char";
/*
 * TYPEDEFS (���Ͷ���)
 */

/*
 * GLOBAL VARIABLES (ȫ�ֱ���)
 */
uint8_t g_TC_OTA_Svc_ID = 0xff;
uint8_t g_ntf_TC_OTA_Ctrl_Enable = 0;

/*
 * LOCAL VARIABLES (���ر���)
 */
static gatt_service_t TC_OTA_Profile_Svc;




/*********************************************************************
 * Profile Attributes - Table
 * ÿһ���һ��attribute�Ķ��塣
 * ��һ��attributeΪService �ĵĶ��塣
 * ÿһ������ֵ(characteristic)�Ķ��壬�����ٰ�������attribute�Ķ��壻
 * 1. ����ֵ����(Characteristic Declaration)
 * 2. ����ֵ��ֵ(Characteristic value)
 * 3. ����ֵ������(Characteristic description)
 * �����notification ����indication �Ĺ��ܣ��������ĸ�attribute�Ķ��壬����ǰ�涨���������������һ������ֵ�ͻ�������(client characteristic configuration)��
 *
 */
const gatt_attribute_t TC_OTA_Profile_Att_Tbl[TC_OTA_IDX_NB] =
{
    /*TC OTA Server*/ 
    [TC_OTA_IDX_SERVICE] =   
	{
        { UUID_SIZE_2, UUID16_ARR(GATT_PRIMARY_SERVICE_UUID) },     /* UUID */
        GATT_PROP_READ,                                             /* Permissions */
        UUID_SIZE_2,                                                /* Max size of the value */     /* Service UUID size in service declaration */
        (uint8_t*)TC_OTA_Svc_uuid,                                  /* Value of the attribute */    /* Service UUID value in service declaration */
    },

    /* DATA Char */
    [TC_OTA_IDX_DATA_DECLARATION] =   
	{
        { UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID) },           /* UUID */
        GATT_PROP_READ,                                             /* Permissions */
        0,                                                          /* Max size of the value */
        NULL,                                                       /* Value of the attribute */
    },
    // Characteristic 1 Value
    [TC_OTA_IDX_DATA_VALUE] =   
	{
        { UUID_SIZE_2, UUID16_ARR(TC_OTA_DATA_UUID) },              /* UUID */
        GATT_PROP_WRITE_REQ ,                  						/* Permissions */
        TC_OTA_DATA_CHAR_VALUE_LEN,                                 /* Max size of the value */
        NULL,                                                       /* Value of the attribute */    /* Can assign a buffer here, or can be assigned in the application by user */
    },

    // Characteristic 1 User Description
    [TC_OTA_IDX_DATA_USER_DESCRIPTION] =   
	{
        { UUID_SIZE_2, UUID16_ARR(GATT_CHAR_USER_DESC_UUID) },      /* UUID */
        GATT_PROP_READ,                                             /* Permissions */
        TC_OTA_DATA_CHAR_DESC_LEN,                                  /* Max size of the value */
        (uint8_t *)TC_OTA_Data_Char_Desc,                            /* Value of the attribute */
    },

    /* Ctrl Char */
    [TC_OTA_IDX_CTRL_DECLARATION] =   
	{
        { UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID) },           /* UUID */
        GATT_PROP_READ,                                             /* Permissions */
        0,                                                          /* Max size of the value */
        NULL,                                                       /* Value of the attribute */
    },
    // Characteristic 6 Value
    [TC_OTA_IDX_CTRL_VALUE] =   
	{
        { UUID_SIZE_2, UUID16_ARR(TC_OTA_CTRL_UUID) },      		/* UUID */
        GATT_PROP_NOTI | GATT_PROP_WRITE_REQ,             			/* Permissions */
        TC_OTA_CTRL_CHAR_VALUE_LEN,                                 /* Max size of the value */
        NULL,                                                       /* Value of the attribute */    /* Can assign a buffer here, or can be assigned in the application by user */
    },
    // Characteristic 6 client characteristic configuration
    [TC_OTA_IDX_CTRL_CFG] =   
	{
        { UUID_SIZE_2, UUID16_ARR(GATT_CLIENT_CHAR_CFG_UUID) },     /* UUID */
        GATT_PROP_READ | GATT_PROP_WRITE,                           /* Permissions */
        TC_OTA_CTRL_CHAR_CCC_LEN,                                   /* Max size of the value */
        NULL,                                                       /* Value of the attribute */    /* Can assign a buffer here, or can be assigned in the application by user */
    },
    // Characteristic 6 User Description
    [TC_OTA_IDX_CTRL_USER_DESCRIPTION] =   
	{
        { UUID_SIZE_2, UUID16_ARR(GATT_CHAR_USER_DESC_UUID) },      /* UUID */
        GATT_PROP_READ,                                             /* Permissions */
        TC_OTA_CTRL_CHAR_DESC_LEN,                                  /* Max size of the value */
        (uint8_t *)TC_OTA_Ctrl_Char_Desc,                           /* Value of the attribute */
    },
};

/*********************************************************************
 * @fn      TC_OTA_gatt_msg_handler
 *
 * @brief   Simple Profile callback funtion for GATT messages. GATT read/write
 *			operations are handeled here.
 *
 * @param   p_msg       - GATT messages from GATT layer.
 *
 * @return  uint16_t    - Length of handled message.
 */
static uint16_t TC_OTA_gatt_msg_handler(gatt_msg_t *p_msg)
{
    switch(p_msg->msg_evt)
    {
        case GATTC_MSG_READ_REQ:
        {
            /*********************************************************************
             * @brief   Simple Profile user application handles read request in this callback.
             *			Ӧ�ò�������ص��������洦���������
             *
             * @param   p_msg->param.msg.p_msg_data  - the pointer to read buffer. NOTE: It's just a pointer from lower layer, please create the buffer in application layer.
             *					  ָ�����������ָ�롣 ��ע����ֻ��һ��ָ�룬����Ӧ�ó����з��仺����. Ϊ�������, ���Ϊָ���ָ��.
             *          p_msg->param.msg.msg_len     - the pointer to the length of read buffer. Application to assign it.
             *                    ���������ĳ��ȣ��û�Ӧ�ó���ȥ������ֵ.
             *          p_msg->att_idx - index of the attribute value in it's attribute table.
             *					  Attribute��ƫ����.
             *
             * @return  ������ĳ���.
             */
			if(p_msg->att_idx == TC_OTA_IDX_DATA_VALUE)
            {
				co_printf("read TC_OTA_IDX_DATA_VALUE\r\n");
                memcpy(p_msg->param.msg.p_msg_data, TC_OTA_Ctrl_Char_Value, TC_OTA_CTRL_CHAR_VALUE_LEN);
				
                return TC_OTA_CTRL_CHAR_VALUE_LEN;
            }
			else if(p_msg->att_idx == TC_OTA_IDX_CTRL_VALUE)
			{
				co_printf("read TC_OTA_IDX_CTRL_VALUE\r\n");
                memcpy(p_msg->param.msg.p_msg_data, TC_OTA_Ctrl_Char_Value, TC_OTA_CTRL_CHAR_VALUE_LEN);
				
                return TC_OTA_CTRL_CHAR_VALUE_LEN;
			}
        }
        break;

        case GATTC_MSG_WRITE_REQ:
        {
            /*********************************************************************
             * @brief   Simple Profile user application handles write request in this callback.
             *			Ӧ�ò�������ص��������洦��д������
             *
             * @param   p_msg->param.msg.p_msg_data   - the buffer for write
             *			              д����������.
             *
             *          p_msg->param.msg.msg_len      - the length of write buffer.
             *                        д�������ĳ���.
             *          att_idx     - index of the attribute value in it's attribute table.
             *					      Attribute��ƫ����.
             *
             * @return  д����ĳ���.
             */
			if(p_msg->att_idx == TC_OTA_IDX_DATA_VALUE)
            {
				co_printf("write TC_OTA_IDX_DATA_VALUE\r\n");
				
				gst_AppTCOTATB.ImageUpdate = TRUE;
				gst_AppTCOTATB.ucLen = p_msg->param.msg.msg_len <= APP_TC_OTA_RX_BUFF_LEN ? p_msg->param.msg.msg_len:APP_TC_OTA_RX_BUFF_LEN;
				
				memcpy(gst_AppTCOTATB.Buff, p_msg->param.msg.p_msg_data, gst_AppTCOTATB.ucLen);	
				YK_APP_Set_Evt(AppTaskTCB[APP_TASK_TC_OTA].Handler, EVT_TC_OTA_RECV_FRAME);
                return gst_AppTCOTATB.ucLen;
            }
			else if(p_msg->att_idx == TC_OTA_IDX_CTRL_VALUE)
			{
				co_printf("write TC_OTA_IDX_CTRL_VALUE\r\n");
				show_reg(p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len,1);
				gst_AppTCOTATB.ConIdx = p_msg->conn_idx;
                gst_AppTCOTATB.AttIdx = p_msg->att_idx;                
				gst_AppTCOTATB.ImageUpdate = FALSE;
				gst_AppTCOTATB.ucLen = p_msg->param.msg.msg_len <= APP_TC_OTA_RX_BUFF_LEN ? p_msg->param.msg.msg_len:APP_TC_OTA_RX_BUFF_LEN;

                memcpy(gst_AppTCOTATB.Buff, p_msg->param.msg.p_msg_data, gst_AppTCOTATB.ucLen);	
				YK_APP_Set_Evt(AppTaskTCB[APP_TASK_TC_OTA].Handler, EVT_TC_OTA_RECV_FRAME);
                return gst_AppTCOTATB.ucLen;
                
			}
			else if (p_msg->att_idx == TC_OTA_IDX_CTRL_CFG)
            {
                co_printf("TC_OTA_NOTIFIY ENABLE");
                show_reg(p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len,1);
                if(p_msg->param.msg.p_msg_data[0] & 0x1)
				{
				    g_ntf_TC_OTA_Ctrl_Enable = 1;
				}
                else
				{
					g_ntf_TC_OTA_Ctrl_Enable = 0;
				}
                memcpy(TC_OTA_Ctrl_Char_ccc, p_msg->param.msg.p_msg_data, TC_OTA_CTRL_CHAR_CCC_LEN);
            }
        }
        break;
        case GATTC_MSG_LINK_CREATE:
		{
			g_ntf_TC_OTA_Ctrl_Enable = 0;
		    co_printf("link_created\r\n");
			
		}break;
		
        case GATTC_MSG_LINK_LOST:
		{
			g_ntf_TC_OTA_Ctrl_Enable = 0;
		    co_printf("link_lost\r\n");
			
		}break;
		
        default:
            break;
    }
    return p_msg->param.msg.msg_len;
}

/*********************************************************************
 * @fn      TC_OTA_gatt_add_service
 *
 * @brief   TC_OTA Profile add GATT service function.
 *			���GATT service��ATT�����ݿ����档
 *
 * @param   None.
 *
 *
 * @return  None.
 */
void TC_OTA_gatt_add_service(void)
{
	if(g_TC_OTA_Svc_ID==0xff)
	{
		TC_OTA_Profile_Svc.p_att_tb = TC_OTA_Profile_Att_Tbl;
		TC_OTA_Profile_Svc.att_nb = TC_OTA_IDX_NB;
		TC_OTA_Profile_Svc.gatt_msg_handler = TC_OTA_gatt_msg_handler;
		g_TC_OTA_Svc_ID = gatt_add_service(&TC_OTA_Profile_Svc);	
	}
}



