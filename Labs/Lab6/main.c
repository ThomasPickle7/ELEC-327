#include <msp430.h> 
#include <stdint.h>
#include <stdbool.h>
#include "rgb_interface.h"

/**
 * main.c
 */

void init_wdt(void){
    BCSCTL3 |= LFXT1S_2;     // ACLK = VLO
    WDTCTL = WDT_ADLY_250;    // WDT 250ms (~675ms since clk 12khz), ACLK, interval timer
    IE1 |= WDTIE;            // Enable WDT interrupt
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    init_wdt();
    rgb_init_spi();

    _enable_interrupts();
    int i = 0;
    while (1) {
        //iterates through all 32 colors plus the off messages every 4th index for a totla of 44
        set_temperature(i % 44, (i + 1) % 44, (i + 2) % 44 , (i + 3) % 44);
        LPM3;
        i++;


    }

    return 0;
}

// Watchdog Timer interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(WDT_VECTOR))) watchdog_timer (void)
#else
#error Compiler not supported!
#endif
{
    __bic_SR_register_on_exit(LPM3_bits); // exit LPM3 when returning to program (clear LPM3 bits)
}
