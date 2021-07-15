
#include "BSP.h"

#if (BSP_ZB25D_USE > 0u)

//------------------------------------------------------------------------------
/// Flash Write Enable.
//------------------------------------------------------------------------------
void FlashWriteEnable()
{
    SPI_CS_0;
    spi_RWBytes(WRITE_ENABLE);
    SPI_CS_1;
}

//------------------------------------------------------------------------------
/// Read Flash Status Register.
/// \param pbuff  Register data buffer to read.
/// \param len  Length of the data buffer.
//------------------------------------------------------------------------------
uint8_t FlashReadStatusRegister1()
{
    uint8_t data = 0;

    SPI_CS_0;
    //send cmds
    spi_RWBytes(READ_STATUS_REGISTER_1);
    //read status
    data = spi_RWBytes(0xff);
    SPI_CS_1;

    return data;
}


//------------------------------------------------------------------------------
/// Check Erase is busy
/// \rettrun:when is erase ok
//------------------------------------------------------------------------------
void FlashCheckEraseIsBusy()
{
    while((FlashReadStatusRegister1() & 0x01) == 0x01);
}


//------------------------------------------------------------------------------
/// Read Datas Bytes from flash address
/// \param pbuff  read data to buffer.
/// \param addr  read datas from flash start address.
/// \param len  Length of the read bytes data.
//------------------------------------------------------------------------------
void FlashReadDataBytes(uint8_t *pbuff, int addr, int len)
{
    int i = 0;

    if(pbuff==0)
        return;

    SPI_CS_0;

    //send cmd
    spi_RWBytes(READ_DATA);

    //send addr
    spi_RWBytes((addr>>16)&0xff);
    spi_RWBytes((addr>>8)&0xff);
    spi_RWBytes(addr&0xff);

    for(i=0; i<len; i++)
    {
        pbuff[i] = spi_RWBytes(0xff);
    }

    SPI_CS_1;
}

//------------------------------------------------------------------------------
/// Write Flash Data
/// \param pbuff  Data buffer to send.
/// \param addr  Write data to flash start address.
/// \param len 	Data buffer len.
//------------------------------------------------------------------------------
void FlashWriteProgram(uint8_t *pbuff, int addr, int len)
{
    int i = 0, j = 0, num = 0, index = 0;

    if(pbuff==0)
        return;

    if(len<=0)
        return;

    num = (len/256);
    if(len%256)
    {
        num += 1;
    }

    for(j=0; j<num; j++)
    {
        if(j==num-1)
        {
            len -= 256*j;
            FlashWriteEnable();
            FlashCheckEraseIsBusy();

            index = 256-(addr+256*j)%256;

            SPI_CS_0;
            //send cmd
            spi_RWBytes(PAGE_PROGRAM);

            //send addr
            spi_RWBytes(((addr+256*j)>>16)&0xff);
            spi_RWBytes(((addr+256*j)>>8)&0xff);
            spi_RWBytes((addr+256*j)&0xff);

            for(i=0; i<len; i++)
            {
                if(index && i==index)
                {
                    break;
                }
                spi_RWBytes(pbuff[256*j+i]);
            }
            SPI_CS_1;

            FlashCheckEraseIsBusy();
            if(index && index<len)
            {
                FlashWriteEnable();
                FlashCheckEraseIsBusy();

                SPI_CS_0;
                //send cmd
                spi_RWBytes(PAGE_PROGRAM);

                //send addr
                spi_RWBytes(((addr+256*j+index)>>16)&0xff);
                spi_RWBytes(((addr+256*j+index)>>8)&0xff);
                spi_RWBytes((addr+256*j+index)&0xff);

                for(i=index; i<len; i++)
                {
                    spi_RWBytes(pbuff[256*j+i]);
                }
                SPI_CS_1;
                FlashCheckEraseIsBusy();
            }
        }
        else
        {
            FlashWriteEnable();
            FlashCheckEraseIsBusy();

            index = 256-addr%256;

            SPI_CS_0;
            //send cmd
            spi_RWBytes(PAGE_PROGRAM);

            //send addr
            spi_RWBytes((addr>>16)&0xff);
            spi_RWBytes((addr>>8)&0xff);
            spi_RWBytes(addr&0xff);

            for(i=0; i<256; i++)
            {
                if(index && i==index)
                {
                    break;
                }
                spi_RWBytes(pbuff[256*j+i]);
            }
            SPI_CS_1;

            FlashCheckEraseIsBusy();

            if(index && index<256)
            {
                FlashWriteEnable();
                FlashCheckEraseIsBusy();

                SPI_CS_0;
                //send cmd
                spi_RWBytes(PAGE_PROGRAM);

                //send addr
                spi_RWBytes(((addr+index)>>16)&0xff);
                spi_RWBytes(((addr+index)>>8)&0xff);
                spi_RWBytes((addr+index)&0xff);

                for(i=index; i<256; i++)
                {
                    spi_RWBytes(pbuff[256*j+i]);
                }
                SPI_CS_1;

                FlashCheckEraseIsBusy();
            }
        }
    }
}

//------------------------------------------------------------------------------
/// Sector Erase Flash.
/// \param addr  Erase flash start address.
//------------------------------------------------------------------------------
void FlashSectorErase4K(int addr)
{
    FlashWriteEnable();
    FlashCheckEraseIsBusy();

    SPI_CS_0;
    //send cmd
    spi_RWBytes(SECTOR_ERASE);

    //send addr
    spi_RWBytes((addr>>16)&0xff);
    spi_RWBytes((addr>>8)&0xff);
    spi_RWBytes(addr&0xff);
    SPI_CS_1;

    FlashCheckEraseIsBusy();
}


//------------------------------------------------------------------------------
/// Block Erase 32K Flash.
/// \param addr  Erase flash start address.
//------------------------------------------------------------------------------
void FlashBlockErase32K(int addr)
{
    FlashWriteEnable();
    FlashCheckEraseIsBusy();

    SPI_CS_0;
    //send cmd
    spi_RWBytes(BLOCK_ERASE_32K);

    //send addr
    spi_RWBytes((addr>>16)&0xff);
    spi_RWBytes((addr>>8)&0xff);
    spi_RWBytes(addr&0xff);
    SPI_CS_1;

    FlashCheckEraseIsBusy();
}

//------------------------------------------------------------------------------
/// Block Erase 64K Flash.
/// \param addr  Erase flash start address.
//------------------------------------------------------------------------------
void FlashBlockErase64K(int addr)
{
    FlashWriteEnable();
    FlashCheckEraseIsBusy();

    SPI_CS_0;
    //send cmd
    spi_RWBytes(BLOCK_ERASE_64K);

    //send addr
    spi_RWBytes((addr>>16)&0xff);
    spi_RWBytes((addr>>8)&0xff);
    spi_RWBytes(addr&0xff);
    SPI_CS_1;

    FlashCheckEraseIsBusy();
}

//------------------------------------------------------------------------------
/// Chip Erase Flash.
/// \param addr  Erase flash start address.
//------------------------------------------------------------------------------
void FlashChipErase()
{
    FlashWriteEnable();
    FlashCheckEraseIsBusy();

    SPI_CS_0;
    //send cmd
    spi_RWBytes(CHIP_ERASE_C7);
    SPI_CS_1;

    FlashCheckEraseIsBusy();
}


int SpiFlashTest()
{
    uint8_t buff_w[256] = {0};
    uint8_t buff_r[256] = {0};
    int j = 0;
    int mark = 7;
    uint32_t addr = 0;

    for(j = 0; j < 256; j++)
    {
        buff_w[j] = j;
    }
    switch(mark)
    {
        case 1:	//Erase 4K byte
            FlashSectorErase4K(addr);
            //printf("4K Sector Erase Ok\r\n");
            break;
        case 2:	//Erase 32K byte
            FlashBlockErase32K(addr);
            //printf("32K Sector Erase Ok\r\n");
            break;
        case 3:	//Erase 64K byte
            FlashBlockErase64K(addr);
            //printf("64K Sector Erase Ok\r\n");
            break;
        case 4:	//Chip Erase
            FlashChipErase();
            //printf("chip Erase Ok\r\n");
            break;
        case 5:	//Write one byte
            FlashWriteProgram(buff_w, addr, 256);
            //printf("0x%x Write:%x\r\n", addr, buff_w[0]);
            break;
        case 6:	//read one byte
            FlashReadDataBytes(buff_r, addr, 256);
            //printf("addr:0x%x, data:0x%x\r\n", addr, buff_r[0]);
            break;
        case 7:	//Raed 256 Bytes
            FlashChipErase();
            FlashWriteProgram(buff_w, addr, 256);
            FlashReadDataBytes(buff_r, addr, 256);
//		for(j=0; j<256; j++)
//        {
//			if(j%16==0&&j)
//				printf("\r\n");
//			printf("%2x ", buff_r[j]&0xff);
//		}
//        printf("\r\n");
            break;
    }

    return 0;
}

#endif
