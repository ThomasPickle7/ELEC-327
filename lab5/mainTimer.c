/*
    *   Thomas Pickell
    *   Lab 5: Timer
    *   Demo Link: https://drive.google.com/file/d/1eJGeQZbRZbEruSpYgE8sU4LxsXzv_DIf/view?usp=sharing
    *   *IMPORTANT: the cameras refresh rate makes the display appear to flicker in both modes, but it only does so in time-setting mode.
    * 
    *   Created on: Feb 25, 2024
*/

#include <msp430g2452.h>

// Global variables, explained as they appear in the code
volatile char value = 9;
volatile char pressed = 0;
// 0 = countdown mode, 1 = time-setting mode
volatile char mode = 0;
volatile char set_flag = 0;
volatile char stop_flag = 0;
volatile char inc_flag = 0;
volatile unsigned first_flag = 0;


volatile int counter = 0;
volatile int dec_counter = 0;

// Constants for timing
#define ONE_SEC 10000
#define REFRESH 40

// Character patterns for displaying numbers
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

// Variables for display update
char *current_character;
unsigned char current_column = 0;

// Function to refresh the display
void refresh(void) {
    if (current_column > 4)
        current_column = 0;
    P2OUT = 0x01 << current_column;
    P1OUT = ~current_character[current_column++];
}

// Function to update the display with a given value
void update_display(char value) {
    current_character = numbers[value];
}

void main(void){
    // Disable watchdog timer
    WDTCTL = WDTPW + WDTHOLD;

    // Set LFXT1 to VLOCLK
    BCSCTL3 |= LFXT1S_2;

    // Configure Timer A0
    TA0CTL = TASSEL_1 + MC_2 + TAIE;
    TA0CCR0 = ONE_SEC;
    TA0CCR1 = REFRESH;
    TA0CCTL0 = CCIE;
    TA0CCTL1 = CCIE;

    // Configure Ports
    P1DIR |= BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6;
    P2DIR |= BIT0|BIT1|BIT2|BIT3|BIT4;
    P1OUT = BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6;
    P2OUT = BIT0|BIT1|BIT2|BIT3|BIT4;
    P1REN |= BIT7;
    P1IE |= BIT7;
    P1IES |= BIT7;
    P1IFG &= 0;

    // Initialize character patterns for numbers
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

    // Set initial display character
    current_character = numbers[9];

    // Enable interrupts
    __enable_interrupt();

    while(1){
        

        // Check if in time setting mode and not stopped
        if((mode == 0) & (stop_flag == 0)){
            // Decrease the value by 1
            if(~(value >= 0)){
                if(value == 0){
                    value++;
                }
                value--;
            }else{
                value = 0;
            }
            // Wrap around if value exceeds 9
            if (value >= 10){
                value = 0;
            }
        }
        // Update the display with the current value
        update_display(value);

        // Enter low power mode 3
        __low_power_mode_3();
    }
}

// Timer A0 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0 (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A0 (void)
#else
#error Compiler not supported!
#endif
{
    // Increase TA0CCR0 by ONE_SEC
    TA0CCR0 += ONE_SEC;

    // Exit low power mode 3
    __bic_SR_register_on_exit(LPM3_bits);

    // Check if button is pressed, meaning the mode should be changed and sets flag accordingly
    if(pressed == 1){
        set_flag = 1;
    }
}

// Timer A1 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer0_A1 (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A1_VECTOR))) Timer_A1 (void)
#else
#error Compiler not supported!
#endif
{
    // Check if in time setting mode
    if(mode == 0){
        // Check if interrupt is from TA0CCR1
        if (TAIV == TA0IV_TACCR1){
            // Normal refresh rate
            TA0CCR1 += REFRESH;
            // Refresh the display
            refresh();
        }
    }else{
        // Check if interrupt is from TA0CCR1
        if (TAIV == TA0IV_TACCR1){
            // Flashing refresh rate for Extra Credit
            TA0CCR1 += 180;
            // Refresh the display
            refresh();
        }
    }

    // Check if in time setting mode and button is pressed
    if((mode == 1) & (set_flag == 1) & (pressed == 1)){
        // checks if the button has been pressed for 400 refreshes, or 'more than a second'
        if(counter < 400){
            //increments the timer if the threshold hasnt been reached
            counter++;
        }else{
            //if it has, the increment flag is raised and we know to increment the vimer
            inc_flag = 1;
            value++;
            // Wrap around if value exceeds 9
            if(value > 9){
                value = 0;
            }
            counter = 0;
        }
    }

    // Check if in time setting mode and button is pressed
    if((mode == 1) & (pressed == 1)){
        // Check if dec_counter is 0
        if(dec_counter == 0){
            dec_counter++;
        }
        // Check if the button has been pressed for only 2 cycles, IE a fast press
        else if (dec_counter > 2){
            dec_counter = 0;
            // Decrease value by 3 to compensate for the increase brought by pressing the button twice.
            value = value - 3;
        }
    }
    // Check if in time setting mode
    else if (mode == 1){
        // Check if dec_counter is greater than 0
        if(dec_counter > 0){
            dec_counter++;
        }
        // Check if dec_counter is greater than 50, meaning the button has not been pressed for a while
        if(dec_counter > 50){
            dec_counter = 0;
        }
    }
}

// Port 1 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(PORT1_VECTOR))) PORT1_ISR(void)
#else
#error Compiler not supported!
#endif
{
    // Check if it is the first interrupt
    if(first_flag == 1){
        // Toggle the edge select for button interrupt
        P1IES ^= BIT7;

        // Check if in time setting mode
        if(mode == 0){
            // Check if button is pressed
            if(pressed == 1){
                // Check if set_flag is set
                if(set_flag == 1){
                    // Switch to time setting mode
                    mode = 1;
                    set_flag = 0;
                    stop_flag = 0;
                }else{
                    // Toggle stop_flag
                    stop_flag ^= 0x01;
                }
            }
        }else{
            // Check if button is pressed
            if(pressed == 1){
                // Check if increment flag is set
                if(inc_flag == 1){
                    inc_flag = 0;
                    set_flag = 0;
                }else{
                    // Check if set_flag is set
                    if(set_flag == 1){
                        // Switch to normal mode
                        mode = 0;
                        set_flag = 0;
                    }else{
                        // Increase value by 1
                        value++;
                    }
                }
            }
        }

        // Toggle pressed flag
        if(pressed == 0){
            pressed = 1;
        }else{
            pressed = 0;
        }

        // Clear interrupt flag
        P1IFG &= ~BIT7;
    }else{
        // Increment first_flag
        first_flag++;
        // Clear interrupt flag
        P1IFG &= ~BIT7;
    }
}
