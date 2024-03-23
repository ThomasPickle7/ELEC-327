#include <msp430.h> 
#include <stdint.h>
#include <stdbool.h>
#include "rgb_interface.h"
#include "simon_random.h"
/**
 * main.c
 */
void init_wdt(void){
    BCSCTL3 |= LFXT1S_2;     // ACLK = VLO
    WDTCTL = WDT_ADLY_1000;    // WDT 16ms (~43.3ms since clk 12khz), ACLK, interval timer
    IE1 |= WDTIE;            // Enable WDT interrupt
}
void init_buttons() {
    P2DIR &= ~(BIT0 + BIT2 + BIT3 + BIT4); // set to input
    P2REN = BIT0 + BIT2 + BIT3 + BIT4; // enable pullup/down resistors
    P2OUT = BIT0 + BIT2 + BIT3 + BIT4; // set resistors to pull up

    /* Uncomment the following code if you want to use interrupts to detect button presses */
    P2IES = BIT0 + BIT2 + BIT3 + BIT4; // listen for high to low transitions
    P2IFG &=  ~(BIT0 + BIT2 + BIT3 + BIT4); // clear any pending interrupts
    P2IE = BIT0 + BIT2 + BIT3 + BIT4; // enable interrupts for these pins
    
}

int womp_womp[] = {
    1000000/440.00,
    1000000/415.30,
    1000000/392.00,
    1000000/369.99,
    0
}
int womp_womp_periods[] = {1, 1, 1, 2, 1}
#define RED_BUZZ = 1000000/440.00
#define GREEN_BUZZ = 1000000/493.88
#define BLUE_BUZZ = 1000000/523.25
#define YELLOW_BUZZ = 1000000/587.33
#define OFF_BUZZ = 1


int red_on = 0;
int green_on = 0;
int blue_on = 0;
int yellow_on = 0;


void play_sound(uint8_t *LED){
    if(LED == red){
        TA1CCR0 = RED_BUZZ;
        TA1CCR2 = RED_BUZZ/2;
    }
    else if(LED == green){
        TA1CCR0 = GREEN_BUZZ;
        TA1CCR2 = GREEN_BUZZ/2;
    }
    else if(LED == blue){
        TA1CCR0 = BLUE_BUZZ;
        TA1CCR2 = BLUE_BUZZ/2;
    }
    else if(LED == yellow){
        TA1CCR0 = YELLOW_BUZZ;
        TA1CCR2 = YELLOW_BUZZ/2;
    }
    else{
        TA1CCR0 = OFF_BUZZ;
        TA1CCR2 = OFF_BUZZ/2;
    }

}
void stop_sound(){
    TA1CCR0 = 0;
    TA1CCR2 = 0;
}




/* ---------------------------------------------------------------- */
/* Code for generating and testing the Simon values */

#define INITIAL_BUTTON 0

unsigned int next_button() {
    return rand();
}
unsigned int check_button(unsigned int button) {

    if (button == rand())
        return 1;
    else
        return 0;
}

void reset_button_sequence(unsigned int initial_value) {
    srand(initial_value);
    return;
}
/* ---------------------------------------------------------------- */

uint8_t red[] = {0xF0, 0, 0, 10};
uint8_t blue[] = {0xF0, 0, 10, 0};
uint8_t green[] = {0xF0, 10, 0, 0};
uint8_t yellow[] = {0xF0, 0, 10, 10};
uint8_t off[] = {0xE0, 0, 0, 0};



int timer_wakeup_flag = 0;
int button_wakeup_flag = 0;
int main(void)
{
    enum state_enum{PresentingSequence, Lost, Win} state; // enum to describe state of system

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    DCOCTL = 0;                 // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_16MHZ;     // Set range
    DCOCTL = CALDCO_16MHZ;      // Set DCO step + modulation */
    BCSCTL3 |= LFXT1S_2;        // ACLK = VLO - This is also called in the init_wdt() function
    init_wdt();
    rgb_init_spi();
    init_buttons();

    /* To make a functional Simon, you'll need to seed the random number generation system
       randomly! */
    int initial_random_seed = 10;
    reset_button_sequence(initial_random_seed); // TODO: Add code to make the initial seed random!


    _enable_interrupts();

    int leds_on = 0;
    uint8_t *LED1, *LED2, *LED3, *LED4;
    int sequence_counter = 0;
    int sequence_length = 10;  // GRADING: This variable should be changed to test different sequence lengths
    int button;

    while (1) {

        /* This template is a potentially a place to start. What it provides is a
           foundation of a main loop which gets woken up either by a timer (the WDT)
           or by a button press (assuming you enable the button interrupts).
           But it needs more code:

          - SOUND!!!

          - Win state

          - WaitingForUserInput state, including processing button inputs
            (see check_button() function!)

          - If you use the button interrupts to wake up, then processing button
            input for maximum score will require changing the IES (edge-select)
            register to detect the press and release as separate events.
            + Alternatively, if you choose to use polling, you need to handle timing
            for things like sequence presentation, timeout detection, etc. This is
            probably easier than using interrupts, but if you naively use the TA1R
            or TA0R registers (the counters for TA0 or TA1), they may conflict
            with other uses of those resources for sound or serial communication.
            + A middle ground is to use the fastest WDT wakeup and poll the button
            states periodically (you can safely assume that button presses
            will last at least 40-50 ms). Of course, this would still require handling
            timeouts somehow.

          - Animations for win/lose

          - Processing if a timeout has occured to trigger the Lost state

          - Ideally, you will abstract a bunch more so that the code will be more readable!
        */


        if (timer_wakeup_flag) {
            timer_wakeup_flag = 0;
        }

        if (state == Lost) {
            // TODO: Make this dynamic + with sound (more interesting) to achieve a better grade!
            int i = 0;
            if (i == 5){
                i = 0;
            }
            play_sound(womp_womp[i]);
            rgb_set_LEDs((i>=0)? red:off , (i>=1)? red:off , (i>=2)? red:off , (i >= 3)? red:off);
            i++;
        }

        else if (state == PresentingSequence) {
            if (sequence_counter == 0) {
                reset_button_sequence(initial_random_seed);
            }
            button = next_button();
            sequence_counter = sequence_counter + 1;
            if (sequence_counter >= sequence_length) { // TODO: This is actually the condition for winning!
                state = Win; // TODO: Change to "Win" (need to add an enum), and add processing code for winning
            }
            else {
                state = PresentingSequence;
            }

            LED1 = off;
            LED2 = off;
            LED3 = off;
            LED4 = off;
            switch(button) {
            case 0:
                LED1 = blue;
                play_sound(LED1);
                break;
            case 1:
                LED2 = red;
                play_sound(LED2);
                break;
            case 2:
                LED3 = green;
                play_sound(LED3);
                break;
            case 3:
                LED4 = yellow;
                play_sound(LED4);
                break;
            }
            rgb_set_LEDs(LED1, LED2, LED3, LED4);
        }
        __bis_SR_register(LPM3_bits + GIE);
    }
    else if (state == Win) {

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
    timer_wakeup_flag = 1;
    __bic_SR_register_on_exit(LPM3_bits); // exit LPM3 when returning to program (clear LPM3 bits)
}

// Watchdog Timer interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT2_VECTOR
__interrupt void button(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT2_VECTOR))) button (void)
#else
#error Compiler not supported!
#endif
{
    button_wakeup_flag = 1;
    red_on = 0;
    green_on = 0;
    blue_on = 0;
    yellow_on = 0;
    P2IFG &= ~(BIT0 + BIT2 + BIT3 + BIT4);
    if(P2IFG & BIT0){
        red_on = 1;
    }
    if(P2IFG & BIT2){
        green_on = 1;
    }
    if(P2IFG & BIT3){
        yellow_on = 1;
    }
    if(P2IFG & BIT4){
        blue_on = 1;
    }

    __bic_SR_register_on_exit(LPM3_bits); // exit LPM3 when returning to program (clear LPM3 bits)
}
