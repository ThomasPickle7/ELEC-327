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

    P1DIR |= BIT7;  // configure P1 as output

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

    current_character = numbers[9];

    int value = 9;
    //boolean to toggle mode, true = 'time-setting mode', false = 'counting-down mode'
    bool toggle_mode = true;
    //boolean to toggle increment, true = 'increment', false = 'decrement'
    bool increment = false;

    __enable_interrupt();   // equivalent to __bis_SR_register(GIE)

    while(1)
    {
        //checks to see if in time-setting mode
        if(~time_setting_mode){
            //checks to see if function is at 0, doesnt decrement if so
            if(value == 0){
                value = 0;
            }else{
                value = value - 1;
            }
        }else{
            //checks to see if increment or decrement is true
            if(increment){
                //checks to see if function is at 9, doesnt increment if so
                if(value == 9){
                    value = 9;
                }else{
                value = value + 1;
                }
            }else{
                //checks to see if function is at 0, doesnt decrement if so
                if(value == 0){
                    value = 0;
                }else{
                    value = value - 1;
                }
            }
        }
    
        __low_power_mode_3(); // equivalent to __bis_SR_register(LPM3_bits);     // Enter LPM3
        update_display(value);

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

// Define button status and the button variable
int BUTTON_STATUS_NOTPRESSED;
int BUTTON_STATUS_HOLDING;
int BUTTON_STATUS_RELEASED;
int BUTTON_STATUS_NOTPRESSED;

// Define button state values (if inverted then swap the values, assume input at bit 1)
BUTTON_STATE_NOTPRESSED = 0;
BUTTON_STATE_PRESSED = BIT7;

// Setup timer hardware for 1 ms ticks
TA0CCR0 = 125;
TA0CCTL0 = CCIE;
TA0CTL = TASSEL_2 | ID_3 | MC_1 | TACLR;

// Process the button events for every timer tick
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A0(void) // Better name would be Timer_A0_2_ISR
{
  // Local variable for counting ticks when button is pressed
  // Assume nobody will ever hold the button for 2^32 ticks long!
  static uint32_t counter = 0;
  
  // Constant for measuring how long (ms) the button must be held
  // Adjust this value to feel right for a single button press
  const uint32_t BUTTON_PRESSED_THRES = 50;
  
  // Check status of button, assume P2.1 input for the button
  // Note once the button enters release status, other code must change back to not pressed
  switch (button)
  {
    //if the button is gheld for longer than 50ms, it is considered to be holding
    case BUTTON_STATUS_NOTPRESSED:
      // Increment if pressed, else reset the count
      // This will filter out the noise when starting to press
      if ((P2IN & BIT7) == BUTTON_STATE_PRESSED) counter += 1;
      else counter = 0;
      if (counter > BUTTON_PRESSED_THRES) button = BUTTON_STATUS_HOLDING;
      break;
    

    case BUTTON_STATUS_HOLDING:
      // Decrement if not pressed, else set the count to threshold
      // This will filter out the noise when starting to release
      if ((P2IN & BIT1) == BUTTON_STATE_NOTPRESSED) counter = 0;
      else counter = BUTTON_PRESSED_THRES;
      if (counter == 0) button = BUTTON_STATUS_RELEASED;
      break;
  }
}
// Your other code to detect when the button has been pressed and release
if (button == BUTTON_STATUS_RELEASED)
{
    toggle_mode = ~toggle_mode;
    // Must reset after detecting button has been release
    button = BUTTON_STATUS_NOTPRESSED;
}