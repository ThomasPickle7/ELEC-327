#include <msp430.2553.h>

#include <stdlib.h>

#include <stdio.h>

#include <string.h>

#include <math.h>

// ADC in MSP430 by drselim
// Plese don't forget to give credits while sharing this code
// for the video description for the code:
// https://youtu.be/ZOqMVR4tNbI
int datatosend[] = {0b00000001, 0b00000011, 0b00000111, 0b00001111, 0b00011111, 0b00111111, 0b01111111, 0b11111111};

char vt_char[5];

char rm_char[5];

char mv_char[5];

char charmemval[] = "ADC10MEM Value: ";

char volt[] = "Voltage of the Potentiometer is: ";

char newline[] = " \r\n";

char dot[] = ".";

#define SERIAL BIT0 //   P1.0

#define SRCLK BIT4 //   P1.4

#define RCLK BIT5 //   P1.5

// ADC in MSP430 by drselim

// Plese don't forget to give credits while sharing this code

// for the video description for the code:

// https://youtu.be/ZOqMVR4tNbI

void SRCLK_Pulse(void);            // To create a clock pulse for Shift Reg
void RCLK_Pulse(void);             // To create a clock pulse for Storage Reg
void Send_Bit(unsigned int value); // For sending 1 or zero
void ser_output(char *str);
void main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
    // enables IO pins 2.0, 2.4, 2.5,and 2.3
    P2DIR |= (BIT0 + BIT3 + BIT4 + BIT5);

    P1SEL2 = BIT1 | BIT2;
    UCA0CTL1 |= UCSWRST + UCSSEL_2;
    UCA0BR0 = 52;
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS_0;
    UCA0CTL1 &= ~UCSWRST;

    volatile float voltage;

    ADC10CTL0 = SREF_0 | ADC10SHT_2 | ADC10ON;

    ADC10CTL1 = INCH_3 | SHS_0 | ADC10DIV_0 | ADC10SSEL_0 | CONSEQ_0;

    ADC10AE0 = BIT3; // P1.3 also above, INCH_3

    ADC10CTL0 |= ENC;

    // ADC in MSP430 by drselim

    // Plese don't forget to give credits while sharing this code

    // for the video description for the code:

    // https://youtu.be/ZOqMVR4tNbI

    while (1)
    {

        ADC10CTL0 |= ADC10SC;

        while (ADC10CTL1 & ADC10BUSY)
            ;

        int memval = ADC10MEM;

        voltage = ((ADC10MEM * 3.3) / 1023); // mapping values

        int volt_int = floor(voltage);

        int rmndr = floor((voltage - volt_int) * 1000); // ltoa only gets the integer values, this is a workaround to get the fractional part

        // if you get an error with ltoa function below, try: 'ltoa(memval, mv_char,10)'

        // check article:

        // https://software-dl.ti.com/ccs/esd/documents/sdto_cgt_handling_changes_in_ltoa.html

        // if-else blocks to determine which LEDDS  to enable based on input voltage from 1.3
        if (voltage >= .66)
            P2OUT |= BIT0;
        else
            P2OUT &= ~BIT0;
        if (voltage >= (.66 * 2))
            P2OUT |= BIT4;
        else
            P2OUT &= ~BIT4;
        if (voltage >= (.66 * 3))
            P2OUT |= BIT5;
        else
            P2OUT &= ~BIT5;
        if (voltage >= (.66 * 4))
            P2OUT |= BIT3;
        else
            P2OUT &= ~BIT3;
    }
}

void ser_output(char *str)
{

    while (*str != 0)
    {

        while (!(IFG2 & UCA0TXIFG))
            ;

        UCA0TXBUF = *str++;
    }
}

void SRCLK_Pulse(void)

{

    P1OUT |= SRCLK;

    P1OUT ^= SRCLK;
}

void RCLK_Pulse(void)

{

    P1OUT |= RCLK;

    P1OUT ^= RCLK;
}

void Send_Bit(unsigned int value)

{

    if (value != 0)
    {

        P1OUT |= SERIAL;
    }

    else
    {

        P1OUT &= ~SERIAL;
    }
}
