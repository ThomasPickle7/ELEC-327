#include <msp430.h>

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;//stops watchdog timer
    P1DIR |= BIT0;//Enables the LED as an IO device
    P1REN |= BIT3;//Reads whatever input is at bit3
    P1IE |= BIT3;
    P1IES |= BIT3;
    __bis_SR_register(GIE);
    while(1)
     {
        __bis_SR_register(LPM4_bits);     // Enter LPM3

     }
}
//Button ISR3

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector= PORT1_VECTOR;
__interrupt void PORT1_ISR (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT1_VECTOR))) PORT1_ISR (void)
#else
#error Compiler not supported!
#endif
{
    int i = 0;
    while(i < 10000){
        i++;
    }
    P1OUT ^= BIT0;
//    P1IES ^= BIT3;
    P1IFG &= ~BIT3;
    __bic_SR_register_on_exit(LPM4_bits);
}
