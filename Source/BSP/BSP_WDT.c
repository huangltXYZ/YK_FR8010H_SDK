#include "BSP.h"

#if (BSP_WDT_USE > 0u)

/*****************************************************************************
Function name :  BSP_WDT_Init
Description:	 ¿´ÃÅ¹·³õÊ¼»¯
Input:	         void
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2¡¢
1¡¢   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_WDT_Init(void)
{
	SYS_UnlockReg();
    wdt_init();
    wdt_start();
    
    /*********Î¹¹·***************/
    BSP_WDT_Refresh();

}

/*****************************************************************************
Function name :  BSP_WDT_Refresh
Description:	 ¿´ÃÅ¹·Î¹¹·º¯Êý
Input:	         void
Return:          void
Notes:
-----------------------------------------------------------------------------
Modification History:
<No.> <version>	  <time>	   <author>  <contents>
2¡¢
1¡¢   1.00 		  ####-##-##   hlt     build this module
******************************************************************************/
void BSP_WDT_Refresh(void)
{
    SYS_UnlockReg();
    wdt_clear();

}

#endif

