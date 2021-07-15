#ifndef __BSP_ZB25D_H__
#define __BSP_ZB25D_H__


#define ERASE_MAX_TIMES 			100000

#define WRITE_ENABLE				0x06
#define WRITE_DISABLE				0x04
#define READ_DATA					0x03
#define READ_DATA_FAST				0x0B
#define PAGE_PROGRAM				0x02
#define SECTOR_ERASE				0x20	// 4KB Sector Erase
#define BLOCK_ERASE_32K				0x52	// 32KB Block Erase
#define BLOCK_ERASE_64K				0xD8	// 64KB Block Erase
#define CHIP_ERASE_C7				0xC7	// C7h
#define CHIP_ERASE_60				0x60	// 60h
#define READ_STATUS_REGISTER_1		0x05	// Read Status Register -1
#define READ_STATUS_REGISTER_2		0x35	// Read Status Register -2
#define READ_STATUS_REGISTER_3		0x15	// Read Status Register -3

#define CMD_TIMEOUT_TIMES			100
#define PAGE_PROGRAM_TIMEOUT_TIMES	200
#define ERASE_4K_TIMEOUT_TIMES		180
#define ERASE_32K_TIMEOUT_TIMES		240
#define ERASE_64K_TIMEOUT_TIMES		320
#define CHIP_ERASE_TIMEOUT_TIMES	2800



void FlashReadDataBytes(uint8_t *pbuff, int addr, int len);
void FlashWriteProgram(uint8_t *pbuff, int addr, int len);

void FlashSectorErase4K(int addr);
void FlashBlockErase32K(int addr);
void FlashBlockErase64K(int addr);
void FlashChipErase(void);
uint8_t spi_RWBytes(unsigned char pSendBuffer);


#endif

