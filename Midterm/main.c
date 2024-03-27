#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include "rgb_interface.h"
/*CONSTANTS*/
uint8_t red[] = {0xF0, 0, 0, 10};
uint8_t blue[] = {0xF0, 10, 0, 0};
uint8_t green[] = {0xF0, 0, 10, 0};
uint8_t yellow[] = {0xF0, 0, 10, 10};
uint8_t off[] = {0xE0, 0, 0, 0};
int button_on = 0;
int timer_but = 0;
int timer_seq = 0;
int timer_present = 0;

#define ROUNDS = 10
int cur_round = 1;

int button_wakeup_flag = 0;
int wdt_wakeup_flag = 0;

// The sequence of frequencies to play for the win state
unsigned int win[] = {100000/440.00,
    100000/493.88,
    100000/523.25,
    100000/587.33,
    100000/659.25,
    100000/698.46,
    100000/783.99,
    100000/880.00,
    100000/987.77,
    100000/1046.50,
    100000/1174.66
    };
// The sequence of frequencies to play for the lose state
unsigned int womp_womp[] = {
    1000000/440.00,
    1000000/415.30,
    1000000/392.00,
    1000000/369.99
    };
// The sequence of periods to play for the lose state
int womp_womp_periods[] = {1, 1, 1, 2, 1};
//The periods for the respective colored-button presses.
int RED_BUZZ = 1000000/440.00;
int GREEN_BUZZ = 1000000/493.88;
int BLUE_BUZZ = 1000000/523.25;
int YELLOW_BUZZ = 1000000/587.33;

int OFF_BUZZ = 1;





/*FUNCTIONS*/
void init(){

    P2REN |= BIT4|BIT2|BIT3|BIT0;//Reads whatever input is at bit3
    P2IE |= BIT4|BIT2|BIT3|BIT0;
    P2IES |= BIT4|BIT2|BIT3|BIT0;
    P2DIR |= BIT5;
    P2SEL |= BIT5;
    TA1CCR0 = 1;
    TA1CCR2 = 1;
    // Set the timer A1 capture/compare control register 2 to output mode 7 (reset/set)
    TA1CCTL2 = OUTMOD_7;

    // Set the timer A1 control register to use the SMCLK as the clock source and count up to CCR0
    TA1CTL = TASSEL_2 + MC_1;


    // Set the watchdog timer interval to 250ms
    WDTCTL = WDT_ADLY_250;

    // Enable the watchdog timer interrupt
    IE1 |= WDTIE;
    rgb_init_spi();
    rgb_set_LEDs(off, off, off, off);


}

unsigned int check_button(unsigned int right, unsigned int select){
    return (select==right)?1:0;
}

void buzz(int buzz){
    TA1CCR0 = buzz;
    TA1CCR2 = buzz/2;
}



void main(void)


{
    enum state_enum{WaitingForUserInput, PresentingSequence, Lost, Win} state; // enum to describe state of system
    init();
    int sequence[5];
    int i = 0;
    for(i = 0; i < sizeof(sequence); i++){
        sequence[i] = (i % 4) + 1;
    }
    buzz(1);
    state = PresentingSequence;
    __bis_SR_register(GIE);
    while(1)
     {
        if(state == PresentingSequence){

            int a = 0;
            while(a < cur_round){
                if(timer_seq == 15){

                    timer_seq = 0;
                    switch (sequence[a])
                    {
                       case 0:
                           buzz(RED_BUZZ);
                           rgb_set_LEDs(red, off, off, red);
                            break;
                       case 1:
                            buzz(RED_BUZZ);
                            rgb_set_LEDs(red, off, off, off);
                            break;
                       case 2:
                            buzz(GREEN_BUZZ);
                            rgb_set_LEDs(off, green, off, off);
                            break;
                       case 3:
                            buzz(YELLOW_BUZZ);
                            rgb_set_LEDs(off, off, yellow, off);
                            break;
                       case 4:
                            buzz(BLUE_BUZZ);
                            rgb_set_LEDs(off, off, off, blue);
                            break;
                       default:
                            rgb_set_LEDs(blue, off, off, blue);
                            break;
                     }
                    a++;

                }
            }
            __delay_cycles(400000);
            state = WaitingForUserInput;
        }
        if(state == Lost){
            int k = 0;
            while(1){
                if(timer_present == 20){
                    timer_present = 0;
                    if(k == 4){
                        k = 0;
                    }
                    k++;
                    wdt_wakeup_flag = 0;
                    buzz(womp_womp[k]);
                    rgb_set_LEDs((k > 0)? yellow : off, (k > 1)?yellow : off, (k > 2)?yellow : off, (k > 3)?yellow : off);
                }

                if(button_wakeup_flag == 1){
                    button_wakeup_flag = 0;
                    state = WaitingForUserInput;
                    cur_round = 0;
                    break;

                }
            }

        }
        if(state == Win){
            rgb_set_LEDs(blue, blue, blue, blue);
        }
        if(state == WaitingForUserInput){
            rgb_set_LEDs(off,off,off,off);
            int j = 0;
            for(j = 0; j < cur_round; j++){
                    __bis_SR_register(LPM3_bits);
                    if (button_on == sequence[j]){
                    switch (button_on)
                        {
                           case 0:
                               buzz(RED_BUZZ);
                               rgb_set_LEDs(red, off, off, red);
                                break;
                           case 1:
                                buzz(RED_BUZZ);
                                rgb_set_LEDs(red, off, off, off);
                                break;
                           case 2:
                                buzz(GREEN_BUZZ);
                                rgb_set_LEDs(off, green, off, off);
                                break;
                           case 3:
                                buzz(YELLOW_BUZZ);
                                rgb_set_LEDs(off, off, yellow, off);
                                break;
                           case 4:
                                buzz(BLUE_BUZZ);
                                rgb_set_LEDs(off, off, off, blue);
                                break;
                           default:
                                rgb_set_LEDs(blue, off, off, blue);
                                break;
                        }
                    }else{
                        state = Lost;
                        button_wakeup_flag = 0;
                        break;

                    }

            }

            if(cur_round > (sizeof(sequence)/ sizeof(sequence[0]))){
                state = Win;
            }else if (state != Lost){
                state = PresentingSequence;
                cur_round++;
            }



        }
     }

}
//Button ISR3

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector= PORT2_VECTOR;
__interrupt void PORT2_ISR (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT2_VECTOR))) PORT2_ISR (void)
#else
#error Compiler not supported!
#endif
{
    //DR. YOUNG SAID THIS REDUCES DEBOUNCING, SO I PUT IT :)))
    int i = 0;
    while(i < 10000){
        i++;
    }

    button_wakeup_flag = 1;

    if((P2IFG & BIT0) && (timer_but == 4)){
        button_on = 1;
        timer_but = 0;
    }
    else if((P2IFG & BIT2) && (timer_but == 4)){
        button_on = 2;
        timer_but = 0;
    }
    else if((P2IFG & BIT3) && (timer_but == 4)){
        button_on = 3;
        timer_but = 0;
    }
    else if((P2IFG & BIT4) && (timer_but == 4)){
        button_on = 4;
        timer_but = 0;
    }else{
        button_on = 0;
    }
    P2IFG &= ~(BIT0+BIT2+BIT3+BIT4);
    __bic_SR_register_on_exit(LPM4_bits);
}
#pragma vector=WDT_VECTOR;
__interrupt void watchdog_timer (void){
    if(timer_present < 20){
        timer_present++;
    }
    if(timer_seq < 15){
        timer_seq++;
    }

    if(timer_but < 4){
    timer_but++;
    }


}
