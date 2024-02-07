#include <msp430.h>

int main(void)
{

    BCSCTL3 |= LFXT1S_2;                    // ACLK = VLO
//  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
//  WDTCTL = WDT_ADLY_16;               // Stop WDT
//  IE1 |= WDTIE;                             // Enable WDT interrupt


  //timer A1.1
  TA1CTL = TASSEL_1 + MC_1;// ACLK, upmode
  TA1CCR0 = 100;            // Set PWM frequency ~12 kHz / 20
  TA1CCR1 = 25;             // Count to 40
  TA1CCTL1 = OUTMOD_3;     // Set PWM mode

  //timer for A1.2
  TA1CCR2 = 75;             // Count to 40
  TA1CCTL2 = OUTMOD_3;     // Set PWM mode


  P2DIR = BIT1|BIT5; //enables P2.1 AND P2.5
  P2SEL = BIT1|BIT5; //SETS P2.1 and P2.5 to Clock mode


  while(1)
  {
    __bis_SR_register(LPM3_bits + GIE);     // Enter LPM3
  }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(WDT_VECTOR))) watchdog_timer (void)
#else
#error Compiler not supported!
#endif
{
  __bic_SR_register_on_exit(LPM3_bits);     // Clear LPM3 bits from 0(SR)
}
