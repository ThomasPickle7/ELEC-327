#include <msp430.h>
#include <stdint.h>
#include <stdio.h>

#include "rgb_interface.h"
#include "simon_random.h"
/*
 * Simon (With bonus Piano Tiles!)
 * Written By Thomas Pickell
 * Demo Videos:
 * Simon:
 *
 * Piano Tiles:
 *
 *
 * GRADERS: To switch between Simon and Piano Tiles, change the value of game_toggle to 0 or 1 (line 88)
 *  To change the number of rounds in Simon, change the value of ROUNDS (line 84)
 * To change the difficulty of the game, change the value of DIFFICULTY (line 82)
 * */

/*CONSTANTS*/

// Enum to describe the state of the system
enum state_enum
{
    WaitingForUserInput,
    PresentingSequence,
    Lost,
    Win
} state;

// colors for the LEDs
const uint8_t red[] = {0xF0, 0, 0, 25};
const uint8_t blue[] = {0xF0, 25, 0, 0};
const uint8_t green[] = {0xF0, 0, 25, 0};
const uint8_t yellow[] = {0xF0, 0, 25, 25};
const uint8_t off[] = {0xE0, 0, 0, 0};

// The periods for the respective colored-button presses.
const unsigned int RED_BUZZ = 1000000 / 440.00;
const unsigned int GREEN_BUZZ = 1000000 / 493.88;
const unsigned int BLUE_BUZZ = 1000000 / 523.25;
const unsigned int YELLOW_BUZZ = 1000000 / 587.33;

const unsigned int OFF_BUZZ = 1;
// The sequence of frequencies to play for the win state
const unsigned int win[] = {
    1000000 / 369.99,
    1000000 / 293.66,
    1000000 / 329.63,
    1000000 / 220.00,
    1000000 / 220.00,
    1000000 / 329.63,
    1000000 / 369.99,
    1000000 / 293.66};
// The sequence of frequencies to play for the lose state
const unsigned int womp_womp[] = {
    1000000 / 440.00,
    1000000 / 415.30,
    1000000 / 392.00,
    1000000 / 369.99};
/*VARIABLES*/
// Indicates which button was pressed
// 0 = no button, 1 = red, 2 = green, 3 = yellow, 4 = blue
volatile char button_on = 0;

// a timer to seperate button presses
volatile char timer_but = 0;
volatile char timer;
// a timer used to present the sequences (includes any non-user input-caused sequences))
volatile char timer_present = 0;
int timeout = 0;

// The current round of the game
volatile char cur_round = 1;

// Flags to indicate if the system was woken up by a button press or the watchdog timer
char button_wakeup_flag = 0;
char wdt_wakeup_flag = 0;

// ** EXTRA CREDIT **
const char DIFFICULTY = 1;
// Determines the difficulty of the game, higher indicates less time to give inputs
const char ROUNDS = 10;
// Determines the number of rounds of either game to play

// ** EXTRA CREDIT **
const char game_toggle = 0;
// used to determine which game to play, 0 = Simon, 1 = Piano Tiles
// for playable/challenging Simon, i recommend ROUNDS = 10, DIFFICULTY = 1
// for playable/challenging piano tiles, I recommend ROUNDS = 30, DIFFICULTY = 8
char mv_char[];
/*FUNCTIONS*/
void init()
{
    // Set the button pins to be read from
    P2REN |= BIT4 | BIT2 | BIT3 | BIT0;
    // Enables interrupts for the button pins
    P2IE |= BIT4 | BIT2 | BIT3 | BIT0;
    // Set the button pins to trigger interrupts on the falling edge
    // P2IES |= BIT4 | BIT2 | BIT3 | BIT0;
    // Sets up the buzzer pin
    P2DIR |= BIT5;
    P2SEL |= BIT5;
    // Set the timer A1 capture/compare control register 0 to 0
    TA1CCR0 = 0;
    TA1CCR2 = 0;
    // Set the timer A1 capture/compare control register 2 to output mode 7 (reset/set)
    TA1CCTL2 = OUTMOD_7;

    // Set the timer A1 control register to use the SMCLK as the clock source and count up to CCR0
    TA1CTL = TASSEL_2 + MC_1;

    // Set the watchdog timer interval to 250ms
    WDTCTL = WDT_ADLY_250;

    // Enable the watchdog timer interrupt
    IE1 |= WDTIE;

    // Sets up the RGB LEDs and turns them off
    rgb_init_spi();
    rgb_set_LEDs(off, off, off, off);
    // INITIALIZE UART
    UCA0CTL1 |= UCSWRST + UCSSEL_2;
    // Set the baud rate to 19200
    UCA0BR0 = 52;
    UCA0BR1 = 0;

    // Set the modulation to 0
    UCA0MCTL = UCBRS_0;
    // Set the control register to use the SMCLK and release the reset
    UCA0CTL1 &= ~UCSWRST;
}

void buzz(int buzz)
{
    // Set the timer A1 capture/compare control register 0 to the frequency of the buzz
    TA1CCR0 = buzz;
    TA1CCR2 = buzz >> 1;
    // Wait for the buzz to finish
    __delay_cycles(400000);
    // Turn off the buzzer
    TA1CCR0 = 0;
    TA1CCR2 = 0;
}

void button_light(char button)
{
    // Set the LEDs and buzz the buzzer based on the button pressed
    switch (button)
    {
    case 0:
        // case for no button pressed
        rgb_set_LEDs(red, off, off, red);
        break;
    case 1:
        // case for red button pressed
        rgb_set_LEDs(red, off, off, off);
        break;
    case 2:
        // case for green button pressed
        rgb_set_LEDs(off, green, off, off);
        break;
    case 3:
        // case for yellow button pressed
        rgb_set_LEDs(off, off, yellow, off);
        break;
    case 4:
        // case for blue button pressed
        rgb_set_LEDs(off, off, off, blue);
        break;
    default:
        rgb_set_LEDs(blue, off, off, blue);
        break;
    }
}
void button_buzz(char button)
{
    // Set the LEDs and buzz the buzzer based on the button pressed
    switch (button)
    {
    case 0:
        // case for no button pressed
        buzz(RED_BUZZ);
        break;
    case 1:
        // case for red button pressed
        buzz(RED_BUZZ);
        break;
    case 2:
        // case for green button pressed
        buzz(GREEN_BUZZ);
        break;
    case 3:
        // case for yellow button pressed
        buzz(YELLOW_BUZZ);
        break;
    case 4:
        // case for blue button pressed
        buzz(BLUE_BUZZ);
        break;
    default:

        break;
    }
}

void ser_output(char *str)
{
    // iterates through the string and sends each character
    while (*str != 0)
    {

        while (!(IFG2 & UCA0TXIFG))
            ;

        UCA0TXBUF = *str++;
    }
}

void reverse(char s[])
{

    int i, j;

    char c;
    // reverses the string
    for (i = 0, j = sizeof(s) - 1; i < j; i++, j--)
    {

        c = s[i];

        s[i] = s[j];

        s[j] = c;
    }
}
void itoa(int n, char s[])
{

    int i, sign;
    // converts an integer to a string
    if ((sign = n) < 0) /* record sign */

        n = -n; /* make n positive */

    i = 0;
    // iterates through the number and adds each digit to the string
    do
    { /* generate digits in reverse order */

        s[i++] = n % 10 + '0'; /* get next digit */

    } while ((n /= 10) > 0); /* delete it */

    if (sign < 0)

        s[i++] = '-';

    s[i] = '\0';

    reverse(s);
}

void main(void)
{
    // Initialize the system
    init();
    // The sequence of buttons to press
    char sequence[ROUNDS]; // GRADERS: change this value to alter the number of rounds

    char i = 0;
    // Fill the sequence with random button presses

    // Set the initial state to play the Win sequence
    state = Lost;
    // Enable global interrupts
    __bis_SR_register(GIE);
    // main loop, effectively where the game starts

    while (1)
    {

        if (state == PresentingSequence)
        {
            if (game_toggle == 0)
            {
                char a = 0;
                // iterates through every round up to the current one
                while (a < cur_round)
                {
                    rgb_set_LEDs(off, off, off, off);
                    // if the timer is at 20, it will play the next button press in the sequence
                    if (timer_present == 20)
                    {
                        timer_present = 0;
                        button_light(sequence[a]);
                        button_buzz(sequence[a]);
                        rgb_set_LEDs(off, off, off, off);
                        a++;
                    }
                }
            }
            // waits for the user to input the sequence

            state = WaitingForUserInput;
        }
        if (state == Lost)
        {
            char k = 0;
            // resets the button wakeup flag
            button_wakeup_flag = 0;
            while (1)
            {
                if (timer_present == 20)
                {
                    // if the timer is at 20, it will play the womp womp sound
                    timer_present = 0;
                    if (k == 4)
                    {
                        k = 0;
                    }
                    k++;

                    rgb_set_LEDs((k <= 0) ? green : red, (k <= 1) ? green : red, (k <= 2) ? green : red, (k <= 3) ? green : red);
                    buzz(womp_womp[k]);
                }
                if (button_wakeup_flag == 1)
                {
                    srand(timer);
                    // Fill the sequence with random button presses
                    for (i = 0; i < sizeof(sequence); i++)
                    {
                        sequence[i] = rand() + 1;
                    }
                    state = WaitingForUserInput;
                    cur_round = 0;
                    break;
                }
            }
        }
        if (state == Win)
        {
            char b = 0;
            button_wakeup_flag = 0;
            // iterates through the win sequence
            while (1)
            {
                // if the timer is at 20, it will play the next button press in the win sequence
                if (timer_present == 20)
                {
                    timer_present = 0;
                    if (b == 8)
                    {
                        b = 0;
                    }
                    b++;
                    // plays the next button press in the win sequence
                    rgb_set_LEDs(((b % 4) == 0) ? yellow : off, ((b % 4) == 2) ? yellow : off, ((b % 4) == 3) ? yellow : off, ((b % 4) == 1) ? yellow : off);
                    // plays the next button press in the win sequence
                    buzz(win[b]);
                }
                // if the button is pressed, it will reset the sequence and start the game
                if (button_wakeup_flag == 1)
                {
                    // re-seeds the random number generator based on the timer
                    // creates a truly random sequence
                    srand(timer);
                    // Fill the sequence with random button presses
                    for (i = 0; i < sizeof(sequence); i++)
                    {
                        sequence[i] = rand() + 1;
                    }
                    // resets the button wakeup flag
                    button_wakeup_flag = 0;
                    state = WaitingForUserInput;
                    cur_round = 0;
                    break;
                }
            }
        }

        if (state == WaitingForUserInput)
        {

            rgb_set_LEDs(off, off, off, off);
            buzz(OFF_BUZZ);
            char j = 0;
            // checks to see if the game is in Simon mode
            if (game_toggle == 0)
            {
                // iterates up to the current round
                for (j = 0; j < cur_round; j++)
                {
                    timeout = 0;
                    // waits for the user to press a button
                    wdt_wakeup_flag = 1;
                    LPM3;
                    // if the user takes too long, they lose
                    if (wdt_wakeup_flag == 0)
                    {
                        state = Lost;
                        break;
                    }
                    // sends the score to the UART
                    itoa((int)button_on, mv_char);
                    ser_output("SCORE");
                    ser_output(mv_char);
                    ser_output("\r\s");
                    // if the user presses the correct button, it will light the button and buzz
                    if (button_on == sequence[j])
                    {
                        continue;
                    }
                    // if the user presses the wrong button, they lose
                    else
                    {
                        state = Lost;
                        break;
                    }

                    rgb_set_LEDs(off, off, off, off);
                    buzz(OFF_BUZZ);
                }
                // if the user has completed this round, it will check to see if they have won
                if (cur_round == (sizeof(sequence) / sizeof(sequence[0])) && (state != Lost))
                {
                    state = Win;
                }
                // if the user hasnt lost or won, it will present the next sequence
                else if (state != Lost)
                {
                    state = PresentingSequence;
                    cur_round++;
                }
            }
            // ** EXTRA CREDIT **
            // presumes the game is in Piano Tiles mode
            else
            {
                // iterates through the sequence
                for (j = 0; j < ROUNDS; j++)
                {
                    // lights the button and waits for the user to press it
                    button_light(sequence[j]);
                    timeout = 0;
                    wdt_wakeup_flag = 1;
                    LPM3;
                    // if the user takes too long, they lose
                    if (wdt_wakeup_flag == 0)
                    {
                        state = Lost;
                        break;
                    }
                    // if the user presses the correct button, it will light the button and buzz
                    if (button_on == sequence[j])
                    {
                        button_buzz(button_on);
                    }
                    // if the user presses the wrong button, they lose
                    else
                    {
                        state = Lost;
                        break;
                    }
                }
                // if the user completes the sequence, they win
                if (state != Lost)
                {
                    state = Win;
                }
            }
        }
    }
}
// Button ISR3
#pragma vector = PORT2_VECTOR;
__interrupt void PORT2_ISR(void)
{
    // DR. YOUNG SAID THIS REDUCES DEBOUNCING, SO I PUT IT :)))
    int i = 0;
    while (i < 10000)
    {
        i++;
    }
    // sets the general button wakeup flag
    button_wakeup_flag = 1;
    // sets the button pressed based on the button pressed
    // red button
    if ((P2IFG & BIT0) && (timer_but == 2))
    {
        button_light(1);
        button_buzz(1);
        button_on = 1;
        timer_but = 0;
    }
    // green button
    else if ((P2IFG & BIT2) && (timer_but == 2))
    {
        button_light(2);
        button_buzz(2);
        button_on = 2;
        timer_but = 0;
    }
    // yellow button
    else if ((P2IFG & BIT3) && (timer_but == 2))
    {
        button_light(3);
        button_buzz(3);
        button_on = 3;
        timer_but = 0;
    }
    // blue button
    else if ((P2IFG & BIT4) && (timer_but == 2))
    {
        button_light(4);
        button_buzz(4);
        button_on = 4;
        timer_but = 0;
    }
    // no button
    else
    {
        button_on = 0;
    }
    rgb_set_LEDs(off, off, off, off);
    P2IFG &= ~(BIT0 + BIT2 + BIT3 + BIT4);
    __bic_SR_register_on_exit(LPM4_bits);
}
#pragma vector = WDT_VECTOR;
__interrupt void watchdog_timer(void)
{
    // increments the timer
    // this timer is used for all presentation modes
    if (timer_present < 20)
    {
        timer_present++;
    }
    // increments the timer
    // this timer is used to time out the user if they take too long
    if (wdt_wakeup_flag == 1)
    {
        timeout++;
        if (timeout > 100 / DIFFICULTY)
        {

            wdt_wakeup_flag = 0;
            __bic_SR_register_on_exit(LPM3_bits);
        }
    }
    // increments the timer
    // this timer is used to debounce the buttons
    if (timer_but < 2)
    {
        timer_but++;
    }
    // this timer is used to seed the RNG
    timer++;
}
