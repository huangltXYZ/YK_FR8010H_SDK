
#include "YK_APP.h"

#define __RAM_FUNC __attribute__((section("ram_code")))



__RAM_FUNC void timer0_isr_ram(void)
{
    timer_clear_interrupt(TIMER0);
	
    YK_APP_TimeProc(1);
}


__RAM_FUNC void timer1_isr_ram(void)
{
    timer_clear_interrupt(TIMER1);
	
    app_light_timer_cb(PNULL);
	
}

