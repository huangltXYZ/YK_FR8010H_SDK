#include "BSP.h"

#if (BSP_IIC_USE > 0u)
	
	
/******* ���IIC�ܽ����� **********/
#define SDA_IN() 	         gpio_set_dir(GPIO_PORT_A, GPIO_BIT_4, GPIO_DIR_IN);
#define SDA_OUT()            gpio_set_dir(GPIO_PORT_A, GPIO_BIT_4, GPIO_DIR_OUT);
#define SDA_READ()           gpio_get_pin_value(GPIO_PORT_A, GPIO_BIT_4)

#define SCL_OUT()            gpio_set_dir(GPIO_PORT_A, GPIO_BIT_7, GPIO_DIR_OUT);

#define CHECK_BUSY()         (gpio_get_pin_value(GPIO_PORT_A, GPIO_BIT_4) & gpio_get_pin_value(GPIO_PORT_A, GPIO_BIT_7))

#define ACK 		         0x00
#define NACK 		         0x01

#define SDA_L()              gpio_set_pin_value(GPIO_PORT_A, GPIO_BIT_4, 0)
#define SDA_H()		         gpio_set_pin_value(GPIO_PORT_A, GPIO_BIT_4, 1)

#define SCL_L() 	         gpio_set_pin_value(GPIO_PORT_A, GPIO_BIT_7, 0)
#define SCL_H()              gpio_set_pin_value(GPIO_PORT_A, GPIO_BIT_7, 1)

	
/*****************************************************************************
Function name :  BSP_IIC_Init
Description:	 IIC��ʼ��
Input:	         void
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_IIC_Init(void)
{
	/*Ӳ����PA7��PA2�̽ӣ����������PA2���óɸ�������*/
	system_set_port_mux(GPIO_PORT_A, GPIO_BIT_2, PORTA2_FUNC_A2);
	gpio_set_dir(GPIO_PORT_A, GPIO_BIT_2, GPIO_DIR_IN);
    system_set_port_pull(GPIO_PA2, false);
	
	
	system_set_port_mux(GPIO_PORT_A, GPIO_BIT_4, PORTA4_FUNC_A4);
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_7, PORTA7_FUNC_A7);
	
	system_set_port_pull(GPIO_PA4, false);/*��ΧӲ������*/
	system_set_port_pull(GPIO_PA7, false);
	
	SDA_OUT();
    SCL_OUT();
    SDA_H();
    SCL_H();
}

/*****************************************************************************
Function name :  BSP_IIC_Delay
Description:	 IIC ��ʱ����������IIC����
Input:	         void
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_IIC_Delay(U8 ucTick)
{
    U16 us_unit;
    U16 i;
    for(us_unit = 0; us_unit < ucTick; us_unit++)
    {
        for(i = 0;i < 2; i++);
    } 
}

/*****************************************************************************
Function name :  BSP_IIC_Start
Description:	 IIC ��ʼ�ź�
Input:	         void
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_IIC_Start(void)
{
    /*SCL���ó�����������õ�*/
	SCL_L();   
	SCL_OUT();
	SCL_L();   
	BSP_IIC_Delay(1);
	
	/*SDA���ó�����������ø�*/
	SDA_H();
	SDA_OUT();
	SDA_H();
	BSP_IIC_Delay(1);
	
	SCL_H();   
	BSP_IIC_Delay(1);
	
	SDA_L();
	BSP_IIC_Delay(1);
	
	SCL_L();   
	BSP_IIC_Delay(1);
	BSP_IIC_Delay(1);
	
}

/*****************************************************************************
Function name :  BSP_IIC_Stop
Description:	 IIC ֹͣ�ź�
Input:	         void
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_IIC_Stop(void)
{
    SCL_L();
	BSP_IIC_Delay(1);
	BSP_IIC_Delay(1);
	
    SDA_L();
	BSP_IIC_Delay(1);
	
    SCL_H();
    BSP_IIC_Delay(1);
	
    SDA_H();
    BSP_IIC_Delay(1);
}

/*****************************************************************************
Function name :  BSP_IIC_Ack
Description:	 IIC ����ACK
Input:	         ucAck->ACK�ĵ�ƽ
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_IIC_Ack(U8 ucAck)
{
    SCL_L();
    BSP_IIC_Delay(1);

    if(1 == ucAck)
    {
        SDA_H();
    }
    else
    {
        SDA_L();
    }
    BSP_IIC_Delay(1);

    SCL_H();
    BSP_IIC_Delay(1);

    SCL_L();
    BSP_IIC_Delay(1);
}

/*****************************************************************************
Function name :  BSP_IIC_SendData
Description:	 IIC ��������
Input:	         ucData->�����͵�����
Return:          1->NACK 0->ACK
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
U8 BSP_IIC_SendData(U8 ucData)
{
    U8 BitCnt;
	U8 ucAck;
	
	
    for(BitCnt =0; BitCnt < 8; BitCnt++)
    {
		SCL_L(); 
		BSP_IIC_Delay(1);
		BSP_IIC_Delay(1);
        if(ucData  & 0x80)    
		{
		    SDA_H();
		}
        else    
		{
		    SDA_L();
		}
		ucData <<= 1;

        SCL_H();
        BSP_IIC_Delay(1);
		BSP_IIC_Delay(1);
    }
    SCL_L();
    BSP_IIC_Delay(1);
	BSP_IIC_Delay(1);
	
    SDA_IN();  //waiting for ack from slave
    SCL_H();
    BSP_IIC_Delay(1);
	BSP_IIC_Delay(1);
    ucAck = ((SDA_READ()) ? 1 : 0);
    SCL_L();
    BSP_IIC_Delay(1);
	BSP_IIC_Delay(1);
    SDA_L();
    SDA_OUT();

    return( ucAck );	
}


/*****************************************************************************
Function name :  BSP_IIC_RcvByte
Description:	 IIC ��������
Input:	         void
Return:          ���յ�������
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
U8 BSP_IIC_RcvByte(void)
{
    U8 BitCnt, ucData = 0x00;

    SDA_IN();

    SCL_L();
	
    BSP_IIC_Delay(1);
	BSP_IIC_Delay(1);
	
    for(BitCnt = 0; BitCnt < 8; BitCnt++)
    {
        SCL_H();
        BSP_IIC_Delay(1);
	    BSP_IIC_Delay(1);
        ucData <<= 0x01;
        if(SDA_READ())
        {
            ucData++;
        }
        SCL_L();
        BSP_IIC_Delay(1);
	    BSP_IIC_Delay(1);
    }

    SDA_OUT();

    return( ucData );
}

#endif
