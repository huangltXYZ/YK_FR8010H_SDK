#include "BSP.h"

#if (BSP_UART_USE > 0u)


/*****************************************************************************
Function name :  BSP_UART_Init
Description:	 ���ڳ�ʼ��
Input:	         ucChannel->����ͨ�����
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_UART_Init(U8 ucChannel)
{
	if(ucChannel >= UART_CHNL_MAX)
	{
	    return;
	}
	if(UART_CHNL_HCI == ucChannel)
	{
//        system_set_port_pull(GPIO_PA2, false);
        //system_set_port_mux(GPIO_PORT_A, GPIO_BIT_2, PORTA2_FUNC_UART1_RXD);
        system_set_port_mux(GPIO_PORT_A, GPIO_BIT_3, PORTA3_FUNC_UART1_TXD);
        uart_init(UART1, BAUD_RATE_921600); 
	
	}

}

/*****************************************************************************
Function name :  BSP_UART_DeInit
Description:	 �������ʼ��
Input:	         ucChannel->����ͨ�����
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_UART_DeInit(U8 ucChannel)
{

	if(ucChannel >= UART_CHNL_MAX)
	{
	    return;
	}
	if(UART_CHNL_HCI == ucChannel)
	{
		
    #if (ENABLE_UART0 && (XN297_MODE_EN == 0))    
        /* Init UART0 for printf */
        UART_DeInit (UART0);
    #else

    #endif
	
	}
	

}

/*****************************************************************************
Function name :  BSP_UART_Transmit
Description:	 ���ڷ��ͺ���
Input:	         SendType->���ͷ�ʽ����ѯ/�ж�/DMA
                 ucChannel->ͨ����
                 pucTxBuf->�����͵�����Buff
                 usLen->�����͵����ݳ���
                 ulTimeout->��ѯ��ʽ����ʱ�ĳ�ʱʱ��
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_UART_Transmit(U8 SendType, U8 ucChannel, U8 *pucTxBuf, U16 usLen, U32 ulTimeout)
{
	
	
    if(SendType >= UART_CHNL_TYPE_MAX) 
	{
	    return;
	}
    
	if(ucChannel >= UART_CHNL_MAX) 
	{
	    return;
	}
    
    switch(SendType)
    {
        case UART_SEND_TYPE_POLL:
        {
        
        }break;
        
        case UART_SEND_TYPE_IT:
        {

        }break;  
        
        case UART_SEND_TYPE_DMA:
        {
           
        }break;

        default:
            break;        
    }
}

#endif
