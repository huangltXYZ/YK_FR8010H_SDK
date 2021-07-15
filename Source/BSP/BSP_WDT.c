#include "BSP.h"

#if (BSP_WDT_USE > 0u)

/*****************************************************************************
Function name :  BSP_WDT_Init
Description:	 ���Ź���ʼ��
Input:	         void
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_WDT_Init(void)
{
	SYS_UnlockReg();
    wdt_init();
    wdt_start();
    
    /*********ι��***************/
    BSP_WDT_Refresh();

}

/*****************************************************************************
Function name :  BSP_WDT_Refresh
Description:	 ���Ź�ι������
Input:	         void
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2��
1��   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_WDT_Refresh(void)
{
    SYS_UnlockReg();
    wdt_clear();

}

#endif

