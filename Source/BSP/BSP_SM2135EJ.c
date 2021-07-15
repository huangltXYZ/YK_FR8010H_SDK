#include "BSP.h"

#if (BSP_SM2135EJ_USE > 0u)

/*****************************************************************************
Function name :  BSP_SM2135EJ_Init
Description:	 SM2135EJ��ʼ��
Input:	         void
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_SM2135EJ_Init(void)
{
	SM2135EJ_MAX_CURRENT MaxCurrent;

    memset(&MaxCurrent, 0x00, sizeof(SM2135EJ_MAX_CURRENT));
        
#if (HW_RGB_ENABLE == 1) 
    MaxCurrent.RGB = HW_MAX_CURRENT_RGB;
#endif

    MaxCurrent.WY = HW_MAX_CURRENT_WY;

    BSP_SM2135EJ_SetMaxCurrent(MaxCurrent);
    BSP_SM2135EJ_SetWorkMode(SM2135EJ_WORK_MODE_NORMAL);

//    BSP_SM2135EJ_SetRGBLevel(255,0,0);
//    while(1);
}

/*****************************************************************************
Function name :  BSP_SM2135EJ_SetMaxCurrent
Description:	 ����������
Input:	         MaxCurrent->RGB��WY��������
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_SM2135EJ_SetMaxCurrent(SM2135EJ_MAX_CURRENT MaxCurrent)
{
    /*�����Ϸ����ж�*/
    if((MaxCurrent.RGB >= SM2135EJ_RGB_MAX_CURRENT_MAX)  || (MaxCurrent.WY >= SM2135EJ_WY_MAX_CURRENT_MAX))
    {
        return;
    }
    
    BSP_IIC_Start();

    BSP_IIC_SendData(SM2135EJ_ADDR_CMD_SET_MAX_CURRENT);
    BSP_IIC_SendData(MaxCurrent.WY | (MaxCurrent.RGB << 4));

    BSP_IIC_Stop();
}

/*****************************************************************************
Function name :  BSP_SM2135EJ_SetWorkMode
Description:	 ���ù���ģʽ ���� or ��������
Input:	         WorkMode->����ģʽ
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_SM2135EJ_SetWorkMode(SM2135EJ_WORK_MODE WorkMode)
{
    /*�����Ϸ����ж�*/
    if((WorkMode != SM2135EJ_WORK_MODE_STANDBY)  && (WorkMode != SM2135EJ_WORK_MODE_NORMAL))
    {
        return;
    }
    
    BSP_IIC_Start();

    BSP_IIC_SendData(SM2135EJ_ADDR_CMD_SET_WORK_MODE);
    BSP_IIC_SendData(WorkMode);

    BSP_IIC_Stop();
}

/*****************************************************************************
Function name :  BSP_SM2135EJ_SetWYLevel
Description:	 ����WYͨ���ĻҶȵȼ�
Input:	         WLevel->Wͨ���ĻҶȵȼ�
                 YLevel->Yͨ���ĻҶȵȼ�
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_SM2135EJ_SetWYLevel(U8 WLevel, U8 YLevel)
{
    /*�����Ϸ����ж�*/
	WLevel = (WLevel > HW_SM2135EJ_WY_LEVEL_MAX) ? (WLevel = HW_SM2135EJ_WY_LEVEL_MAX) : WLevel;
	YLevel = (YLevel > HW_SM2135EJ_WY_LEVEL_MAX) ? (YLevel = HW_SM2135EJ_WY_LEVEL_MAX) : YLevel;
	
	/*WY��·�ĺͲ��ܳ���Ӳ��֧�ֵ��������*/
	if(WLevel + YLevel > HW_SM2135EJ_WY_LEVEL_MAX)
	{
	    return;
	}

    BSP_IIC_Start();

    BSP_IIC_SendData(SM2135EJ_ADDR_CMD_SET_W_LEVEL);
    BSP_IIC_SendData(WLevel);
    BSP_IIC_SendData(YLevel);
	
    BSP_IIC_Stop();
}

#if (HW_RGB_ENABLE == 1)
/*****************************************************************************
Function name :  BSP_SM2135EJ_SetRGBLevel
Description:	 ����RGBͨ���ĻҶȵȼ�
Input:	         RLevel->Rͨ���ĻҶȵȼ�
                 GLevel->Gͨ���ĻҶȵȼ�
                 BLevel->Bͨ���ĻҶȵȼ�
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_SM2135EJ_SetRGBLevel(U8 RLevel, U8 GLevel, U8 BLevel)
{
    /*�����Ϸ����ж�*/
    RLevel = (RLevel > HW_SM2135EJ_RGB_LEVEL_MAX) ? (RLevel = HW_SM2135EJ_RGB_LEVEL_MAX) : RLevel;
	GLevel = (GLevel > HW_SM2135EJ_RGB_LEVEL_MAX) ? (GLevel = HW_SM2135EJ_RGB_LEVEL_MAX) : GLevel;
	BLevel = (BLevel > HW_SM2135EJ_RGB_LEVEL_MAX) ? (BLevel = HW_SM2135EJ_RGB_LEVEL_MAX) : BLevel;
	
    BSP_IIC_Start();

    BSP_IIC_SendData(SM2135EJ_ADDR_CMD_SET_R_LEVEL);
	
//    BSP_IIC_SendData(RLevel);
//    BSP_IIC_SendData(GLevel);
//    BSP_IIC_SendData(BLevel);
	/*ע�����Ӳ��R��B�ӷ��ˣ����������������*/
	BSP_IIC_SendData(BLevel);
    BSP_IIC_SendData(GLevel);
    BSP_IIC_SendData(RLevel);
	

    BSP_IIC_Stop();
}
#endif

#endif
