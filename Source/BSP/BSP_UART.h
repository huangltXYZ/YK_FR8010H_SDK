#ifndef __BSP_UART_H
#define __BSP_UART_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Com port channel */
typedef enum 
{
    UART_CHNL_HCI = 0,
	UART_CHNL_MAX,		
    
}UART_CHNL_BSP;

/* 串口发送方式 */
typedef enum 
{
	UART_SEND_TYPE_POLL = 0,	
    UART_SEND_TYPE_IT,
    UART_SEND_TYPE_DMA,
	UART_CHNL_TYPE_MAX,	
    
}UART_SEND_TYPE_BSP;
 
void BSP_UART_Init(U8 ucChannel);
void BSP_UART_DeInit(U8 ucChannel);
void BSP_UART_Transmit(U8 SendType, U8 ucChannel, U8 *pucTxBuf, U16 usLen, U32 ulTimeout);

#ifdef __cplusplus
}
#endif
  
   
#endif

