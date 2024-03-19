#include <msp430.h>

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

    TA0CCTL0 = CCIE; // TA0CCR0 interrupt enabled
    TA0CCR0 = 31250;
    TA0CTL = TASSEL_1 + MC_1 + ID_1; // SMCLK, up-mode, divide clock by 8

    P1DIR |= 0x01; // Set P1.0 to output direction

    char *test[11] = {".", ".", ".", " ", "-", "-", "-", " ", ".", ".", "."}; // SOS Signal
    /*char *test[9] = {"-", " ",
                       ".", "-", " ",
                       ".", "-", "-", "."};     Initials Signal */
    __bis_SR_register(GIE); // Enable interrupt
    while (1)
    {
        int i;
        for (i = 0; i < sizeof(test); i++)
        { // iterates through all dots and dashes in the array
            if (test[i] == ".")
            { // case for a dot, turns the LED off for 1 unit.
                P1OUT = 0x01;
                __bis_SR_register(LPM0_bits);
            }
            else if (test[i] == "-")
            { // case for a dash, keeps the LED on for 3 units.
                P1OUT = 0x01;
                __bis_SR_register(LPM0_bits);
                __bis_SR_register(LPM0_bits);
                __bis_SR_register(LPM0_bits);
            }
            else if (test[i] == " ")
            { // for a space, wait 2 units
                P1OUT = 0x00;
                __bis_SR_register(LPM0_bits);
                __bis_SR_register(LPM0_bits);
            }
            P1OUT = 0x00;
            __bis_SR_register(LPM0_bits);
        }
        P1OUT = 0x00; // at the end of the word, the LED turns off
        volatile int j;
        for (j = 0; j < 6; j++)
        { // loop repeats to keep LED off for 7 units.
            __bis_SR_register(LPM0_bits);
        }
    }
}

// Timer A0 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(TIMER0_A0_VECTOR))) Timer_A(void)
#else
#error Compiler not supported!
#endif
{
    __bic_SR_register_on_exit(LPM0_bits);
}
