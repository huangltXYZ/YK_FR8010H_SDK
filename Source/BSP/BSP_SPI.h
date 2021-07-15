#ifndef __BSP_SPI_H__
#define __BSP_SPI_H__

#ifdef __cplusplus
 extern "C" {
#endif
 
#define SPI_FLASH_INTERFACE_NUM 0

#if (SPI_FLASH_INTERFACE_NUM == 0)

#define SPI_CS_1 (P26 = 1)
#define SPI_CS_0 (P26 = 0)
void SPI0_init(void);
#elif (SPI_FLASH_INTERFACE_NUM == 1)

#define SPI_CS_1 (P23 = 1)
#define SPI_CS_0 (P23 = 0)
void SPI1_init(void);
#endif


/*SPI¿‡–Õ*/
typedef struct
{
    U32 CLKPhase;
    U32 CLKPolarity;
    U32 DataSize;
    U32 FirstBit;
    
}SPI_TYPE;

/* SPI channel */
typedef enum 
{
	SPI_CHNL_FLASH = 0,

	SPI_CHNL_MAX,	
    
}ENUM_SPI_CHNL_BSP;




void BSP_SPI_Init(U8 ucChannel);
void BSP_SPI_TransmitReceive(U8 ucChannel, const U8 *pTxData, U8 *pRxData, U16 usLen);
U8 BSP_SPI_ReadWriteByte(U8 ucChannel, U8 ucWriteData);

uint8_t spi_RWBytes(unsigned char pSendBuffer);


#ifdef __cplusplus
}
#endif 

#endif


