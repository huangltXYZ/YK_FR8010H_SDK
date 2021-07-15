#ifndef  __BSP_IIC_H__
#define  __BSP_IIC_H__

#ifdef __cplusplus
extern "C" {
#endif


typedef enum 
{ 
	IIC_fail=0, 
	IIC_true,
	
}IIC_bool;





void BSP_IIC_Init(void);
void BSP_IIC_Delay(U8 ucTick);
void BSP_IIC_Start(void);
void BSP_IIC_Stop(void);
void BSP_IIC_Ack(U8 ucAck);
U8 BSP_IIC_SendData(U8 ucData);
U8 BSP_IIC_RcvByte(void);


#ifdef __cplusplus
}
#endif

#endif

