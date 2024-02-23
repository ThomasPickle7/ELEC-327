#include <msp430g2452.h>

// TODO!!!!
#define ONE_SEC 30000    // You'll need to set this to achieve 1 second interrupts!
#define REFRESH 100     // You'll need to set this to for refresh!

// For pixels to be turned on, the column value has
//   to be 1, and the row has to be zero. So a blank
//   display looks like this:
char blank_screen[5] = { 0x7F, 0x7F, 0x7F, 0x7F, 0x7F};


char zero[5] = { 0x3E, 0x51, 0x49, 0x45, 0x3E };

char one[5] = { 0x00, 0x01, 0x7F, 0x21, 0x00 };

char two[5] = { 0x31, 0x49, 0x45, 0x43, 0x21 };

char three[5] = { 0x36, 0x49, 0x49, 0x41, 0x22 };

char four[5] = { 0x04, 0x7F, 0x24, 0x14, 0x0C };

char five[5] = { 0x4E, 0x51, 0x51, 0x51, 0x72 };

char six[5] = { 0x26, 0x49, 0x49, 0x49, 0x3E };

char seven[5] = { 0x60, 0x50, 0x48, 0x47, 0x40 };

char eight[5] = { 0x36, 0x49, 0x49, 0x49, 0x36 };

char nine[5] = { 0x3E, 0x49, 0x49, 0x49, 0x32 };

char *numbers[10];

char *current_character;

int current_column = 0;

// TODO!!!!
void refresh(void) {
    // only 5 columns exist! indexing starts at 0
    if (current_column > 4)
        current_column = 0;
    // Iterate through columns
    P2OUT = 0x01 << current_column;
    P1OUT = ~current_character[current_column++];
    // NOTE - outputs are inverted. We invert here to make it easier.
}

void update_display(int value) {
    current_character = numbers[value];
}


void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;    // Stop WDT
    BCSCTL3 |= LFXT1S_2;         // ACLK = VLO

    // Configure Timer A0 to give us interrupts being driven by ACLK
    TA0CTL = TASSEL_1 + MC_2 + TAIE;  // ACLK, upmode, counter interrupt enable

    TA0CCR0 = ONE_SEC;  // Register 0 counter value to trigger interrupt
    TA0CCR1 = REFRESH;  // Register 1 counter value to trigger interrupt

    TA0CCTL0 = CCIE;    // CCR0 interrupt enabled
    TA0CCTL1 = CCIE;    // CCR1 interrupt enabled

    P1DIR |= 0x7F;  // configure P1 as output
    P2DIR |= 0x1F;  // configure P2 as output

    P1OUT = 0x7F;   // Rows will default to high
    P2OUT = 0x1F;   // Columns will default to high

    numbers[0] = zero;
    numbers[1] = one;
    numbers[2] = two;
    numbers[3] = three;
    numbers[4] = four;
    numbers[5] = five;
    numbers[6] = six;
    numbers[7] = seven;
    numbers[8] = eight;
    numbers[9] = nine;

    current_character = numbers[0];

    int value = 0;

    __enable_interrupt();   // equivalent to __bis_SR_register(GIE)

    while(1)
    {
        // This loop should execute once per second as controlled by TA0
            update_display(value);
            value = value + 1;
            if (value == 10)
                value = 0;

        __low_power_mode_3(); // equivalent to __bis_SR_register(LPM3_bits);     // Enter LPM3

    }
}

// Timer 0 interrupt service routine for register 0 (count)
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0 (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A0 (void)
#else
#error Compiler not supported!
#endif
{
    TA0CCR0 += ONE_SEC;                       // Increment register 0 value for next interrupt
    __bic_SR_register_on_exit(LPM3_bits);     // Clear LPM3 bits from 0(SR)
}

// Timer 0 interrupt service routine for register 1 (refresh)
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer0_A1 (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A1_VECTOR))) Timer_A1 (void)
#else
#error Compiler not supported!
#endif
{
    if (TAIV == TA0IV_TACCR1){
        TA0CCR1 += REFRESH;     // Increment register 1 value for next interrupt
        refresh();              // Refresh
    }
}




