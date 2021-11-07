#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"
#include <arduino.h>

bool timer_triggered;

void timer_init(freq f)
{
    timer_triggered = false;

    _PROTECTED_WRITE((CLKCTRL.OSC32KCTRLA), RTC_RUNSTDBY_bm);
    RTC.CLKSEL = RTC_CLKSEL_INT1K_gc;
    RTC.PITINTCTRL = RTC_PI_bm;
    while (RTC.PITSTATUS & RTC_CTRLBUSY_bm);

    /* RTC clock 1024Hz, divider 1024 => 1 Hz */

    switch(f)
    {
        case freq1:
            RTC.PITCTRLA = RTC_PERIOD_CYC1024_gc | RTC_PITEN_bm;
            break;

        case freq32:
            RTC.PITCTRLA = RTC_PERIOD_CYC32_gc | RTC_PITEN_bm;
            break;

        case freq64:
            RTC.PITCTRLA = RTC_PERIOD_CYC16_gc | RTC_PITEN_bm;
            break;

        case freq128:
            RTC.PITCTRLA = RTC_PERIOD_CYC8_gc | RTC_PITEN_bm;
            break;
    }
}

void timer_handled(void)
{
    timer_triggered = false;
}

bool is_timer_triggered(void)
{
    return timer_triggered;
}

ISR(RTC_PIT_vect)
{
    timer_triggered = true;
    RTC.PITINTFLAGS = RTC_PI_bm;
}