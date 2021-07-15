#include "BSP.h"


#if (BSP_SPI_USE > 0u)

#if (SPI_FLASH_INTERFACE_NUM == 0)
	static void SPI0_init(void);
#elif (SPI_FLASH_INTERFACE_NUM == 1)
	static void SPI1_init(void);
#endif
	

/*****************************************************************************
Function name :  BSP_SPI_Init
Description:	 SPI初始化
Input:	         ucChannel->SPI通道编号
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2、
1、   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_SPI_Init(U8 ucChannel)
{
    if(ucChannel >= SPI_CHNL_MAX) 
	{
	    return;
	} 
	
	if(SPI_CHNL_FLASH == ucChannel)
	{
		
	#if (SPI_FLASH_INTERFACE_NUM == 0)
        SPI0_init();
    #elif (SPI_FLASH_INTERFACE_NUM == 1)
        SPI1_init();
    #endif


	}
	
}



void BSP_SPI_TransmitReceive(U8 ucChannel, const U8 *pTxData, U8 *pRxData, U16 usLen)
{
    if((ucChannel >= SPI_CHNL_MAX) || ((PNULL == pTxData) && (PNULL == pRxData)))
	{
	    return;
	}

}


/*SPI读写一个字节*/
U8 BSP_SPI_ReadWriteByte(U8 ucChannel, U8 ucWriteData)
{
    U8 ucReadData;
    
    if(ucChannel >= SPI_CHNL_MAX) 
	{
	    return 0xFF;
	}    
 
    
 	return ucReadData;/*返回收到的数据*/	
}






#if (SPI_FLASH_INTERFACE_NUM == 1)

uint8_t spi_RWBytes(unsigned char pSendBuffer)
{
    while(SPI_IsTxFifoFull(SPI1));
    SPI_SendData(SPI1, pSendBuffer);

    while(SPI_IsRxFifoEmpty(SPI1));
    return SPI_ReceiveData(SPI1);
}

static void SPI1_init(void)
{
    SYS->P1_MFP |= SYS_MFP_P10_SPI1_MOSI;
    SYS->P2_MFP |= SYS_MFP_P22_SPI1_CLK ;
    SYS->P5_MFP |= SYS_MFP_P57_SPI1_MISO;

    GPIO_ENABLE_DIGITAL_PATH(P1,(1<<0));
    GPIO_ENABLE_DIGITAL_PATH(P2,(1<<2));
    GPIO_ENABLE_DIGITAL_PATH(P5,(1<<7));
    CLK_EnableModuleClock(SPI1_MODULE);

    SPI_InitTypeDef spiInitStruct;

    /* Init SPI1 Master*/
    spiInitStruct.SPI_baudRate = Spi_baudrate_div2;
    spiInitStruct.SPI_CPHA = Spi_clock_phase_1edge;
    spiInitStruct.SPI_CPOL = Spi_pol_low;
    spiInitStruct.SPI_dataFrameSize = Spi_data_frame_8b;
    spiInitStruct.SPI_transferMode = Spi_dir_full_duplex;
    spiInitStruct.SPI_role = Spi_role_master;

    SPI_Init(SPI1, &spiInitStruct);
    SPI_SetTxTrigger(SPI1,Spi_fifo_tx_thre_1_chars);
    SPI_SetRxTrigger(SPI1,Spi_fifo_rx_thre_1_chars);

    SPI_EnableSpi(SPI1);

    SYS->P2_MFP |= SYS_MFP_P23_GPIO;
    GPIO_SetMode(P2,BIT3,GPIO_MODE_OUTPUT);
    P23 = 1;

}

#elif (SPI_FLASH_INTERFACE_NUM == 0)
uint8_t spi_RWBytes(unsigned char pSendBuffer)
{
    while(SPI_IsTxFifoFull(SPI0));
    SPI_SendData(SPI0, pSendBuffer);

    while(SPI_IsRxFifoEmpty(SPI0));
    return SPI_ReceiveData(SPI0);
}

static void SPI0_init(void)
{
    SYS->P0_MFP |= SYS_MFP_P05_SPI0_MOSI | SYS_MFP_P06_SPI0_MISO | SYS_MFP_P07_SPI0_CLK;

    GPIO_ENABLE_DIGITAL_PATH(P0,(1<<5));
    GPIO_ENABLE_DIGITAL_PATH(P0,(1<<6));
    GPIO_ENABLE_DIGITAL_PATH(P0,(1<<7));
    CLK_EnableModuleClock(SPI0_MODULE);

    SPI_InitTypeDef spiInitStruct;

    /* Init SPI1 Master*/
    spiInitStruct.SPI_baudRate = Spi_baudrate_div2;
    spiInitStruct.SPI_CPHA = Spi_clock_phase_1edge;
    spiInitStruct.SPI_CPOL = Spi_pol_low;
    spiInitStruct.SPI_dataFrameSize = Spi_data_frame_8b;
    spiInitStruct.SPI_transferMode = Spi_dir_full_duplex;
    spiInitStruct.SPI_role = Spi_role_master;

    SPI_Init(SPI0, &spiInitStruct);
    SPI_SetTxTrigger(SPI0,Spi_fifo_tx_thre_1_chars);
    SPI_SetRxTrigger(SPI0,Spi_fifo_rx_thre_1_chars);

    SPI_EnableSpi(SPI0);

    SYS->P2_MFP |= SYS_MFP_P26_GPIO;
    GPIO_SetMode(P2,BIT6,GPIO_MODE_OUTPUT);
    P26 = 1;
}

#endif

#endif

