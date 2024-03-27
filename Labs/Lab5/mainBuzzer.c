/*
    *   Thomas Pickell  
    *   Lab 5: Buzzer
    *   Demo Link: https://drive.google.com/file/d/1eEek4p670AQ3eFGAcpzW-2MUltQBU8s8/view?usp=sharing
    *   Created on: Feb 25, 2024
*/
#include "msp430g2553.h"

// Periods for "Twinkle Twinkle Little Star"
int periods_twinkle[] = {1000000/261.63,
                         1000000/261.63,
                         1000000/392.00,
                         1000000/392.00,
                         1000000/440.00,
                         1000000/440.00,
                         1000000/392.00};
// Durations of notes for "Twinkle Twinkle Little Star"
int durations_twinkle[] = {1, 1, 1, 1, 1, 1, 2};
int length_twinkle = sizeof(periods_twinkle)/sizeof(periods_twinkle[0]);

// Periods for "Mary Had a Little Lamb"
int periods_mary[] = {1000000/329.63,
                      1000000/293.66,
                      1000000/261.63,
                      1000000/293.66,
                      1000000/329.63,
                      1000000/329.63,
                      1000000/329.63};
// Durations of notes for "Mary Had a Little Lamb"
int durations_mary[] = {1, 1, 1, 1, 1, 1, 2};
int length_mary = sizeof(periods_mary)/sizeof(periods_mary[0]);

// Variable to keep track of which song should be played
int mary = 0;
//variable to see if the button is pressed mid-song
volatile int mem = 0;
//variable to ensure delay only happens if the button isnt pressed mid-song
volatile int del_flag = 0;

// Function to play a sound given an array of notes and durations
void PlaySound(int *notes, int* durations, int length){
    int i, j;
    //stores the state of the memory flag
    const int song_mem = mem;
    //iterates through every note in the notes list
    for(i = 0; i < length; i++){
        //checks to see if the memory flag has changed, if it has, it breaks the loop
        if (song_mem != mem){
            break;
        }
        //sets the timer to the note and duration
        TA1CCR0 = notes[i];
        //divides the note by 2 to get the duty cycle
        TA1CCR2 = notes[i]>>1;
        //delays for the duration of the note
        for (j=0; j < durations[i]; j++){
            __delay_cycles(400000);
        }
        //resets the timer to 1 to stop the sound
        TA1CCR0 = 1;
        TA1CCR2 = 1;
        //checks to see if the loop has reached the end, and sets the del_flag to 1 if it has
        if(i == length - 1){
            del_flag = 1;
        }
    }
    //if the del_flag is set, the song is rested for 1.5 seconds
    if(del_flag == 1){
        __delay_cycles(1500000);
        del_flag = 0;
    }
}

void main(void){
// Disable the watchdog timer
WDTCTL = WDTPW + WDTHOLD;

// Set the low-frequency crystal oscillator as the clock source
BCSCTL3 |= LFXT1S_2;

// Set the basic clock system control register 1 to 1MHz
BCSCTL1 = CALBC1_1MHZ;

// Set the digitcal clock oscillator control register to 1MHz
DCOCTL = CALDCO_1MHZ;

// Set P2.5 and P2.1 as output pins
P2DIR |= BIT5;
P2DIR |= BIT1;

// Set P2.5 as the timer output
P2SEL |= BIT5;

// Set P1.0 as an output pin
P1DIR |= BIT0;

// Enable the pull-up resistor for P1.3
P1REN |= BIT3;

// Enable interrupt for P1.3
P1IE |= BIT3;

// Set interrupt edge select for P1.3 to falling edge
P1IES |= BIT3;

// Set the timer A1 capture/compare register 0 and 2 to 0
TA1CCR0 = 0;
TA1CCR2 = 0;

// Set the timer A1 capture/compare control register 2 to output mode 7 (reset/set)
TA1CCTL2 = OUTMOD_7;

// Set the timer A1 control register to use the SMCLK as the clock source and count up to CCR0
TA1CTL = TASSEL_2 + MC_1;


  __bis_SR_register(CPUOFF + GIE);
  while (1){
            //plays whichever song should be played based on the mary flag
            if(mary == 0){
                PlaySound(periods_mary, durations_mary, length_mary);
            }
            else{
                PlaySound(periods_twinkle, durations_twinkle, length_twinkle);
            }
            //resets the timer to 1 to stop the sound
            TA1CCR0 = 1;
            TA1CCR2 = 1;
  }
}

// Watchdog Timer interrupt handler
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
{

}

// Port 1 interrupt handler
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
    // Checks to see which song should be played
    if(mary == 1){
        mary = 0;
    }
    else{
        mary = 1;
    }
    // Toggles the memory flag, so that the loop can be stopped if the button is pressed mid-song
    if(mem == 0){
        mem = 1;
    }
    else{
        mem = 0;
    }
    // Toggles the LED, to indicate which song will play if you want to know between songs
    P1OUT ^= BIT0;
    // Clears the interrupt flag
    P1IFG &= ~BIT3;
    __bic_SR_register_on_exit(LPM4_bits);
}