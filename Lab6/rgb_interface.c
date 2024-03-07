/*
 * rgb_interface.c
 *
 *  Created on: Mar 12, 2021
 *      Author: ckemere
 */


#include "rgb_interface.h"
#include <stdint.h>


uint8_t start_frame[] = {0,0,0,0};
uint8_t end_frame[] = {0xFF,0xFF,0xFF,0xFF};
//an alphabet of RGB values
uint8_t a[] = {0xF0, 2, 0, 25};
uint8_t b[] = {0xF0, 6, 0, 25};
uint8_t c[] = {0xF0, 11, 0, 25};
uint8_t d[] = {0xF0, 16, 0, 25};
uint8_t e[] = {0xF0, 21, 0, 25};
uint8_t f[] = {0xF0, 25, 0, 25};
uint8_t g[] = {0xF0, 25, 0, 20};
uint8_t h[] = {0xF0, 25, 0, 15};
uint8_t i[] = {0xF0, 25, 0, 10};
uint8_t j[] = {0xF0, 25, 0, 6};
uint8_t k[] = {0xF0, 25, 0, 1};
uint8_t l[] = {0xF0, 25, 3, 0};
uint8_t m[] = {0xF0, 25, 8, 0};
uint8_t n[] = {0xF0, 25, 13, 0};
uint8_t o[] = {0xF0, 25, 17, 0};
uint8_t p[] = {0xF0, 25, 22, 0};
uint8_t q[] = {0xF0, 25, 22, 0};
uint8_t r[] = {0xF0, 24, 25, 0};
uint8_t s[] = {0xF0, 20, 25, 0};
uint8_t t[] = {0xF0, 16, 25, 0};
uint8_t u[] = {0xF0, 12, 25, 0};
uint8_t v[] = {0xF0, 7, 25, 0};
uint8_t w[] = {0xF0, 3, 25, 0};
uint8_t x[] = {0xF0, 0, 25, 0};
uint8_t y[] = {0xF0, 0, 25, 4};
uint8_t z[] = {0xF0, 0, 25, 8};
uint8_t aa[] = {0xF0, 0, 25, 12};
uint8_t ab[] = {0xF0, 0, 25, 16};
uint8_t ac[] = {0xF0, 0, 25, 21};
uint8_t ad[] = {0xF0, 0, 25, 25};
uint8_t ae[] = {0xF0, 0, 22, 25};
uint8_t af[] = {0xF0, 0, 18, 25};





uint8_t off[] = {0xE0, 0, 0, 0};

//Temperature values are compiled into a list.
//off is added every 4th entry to visualize colors moving.
uint8_t *alphabet[] = {&off, &a, &b, &c, &off, &d, &e, &f, &off, &g, &h, &i,
                       &off, &j, &k, &l, &off, &m, &n, &o, &off, &p, &q, &r,
                       &off, &s, &t, &u, &off, &v, &w, &x, &off, &y, &z, &aa,
                       &off, &ab, &ac, &ad, &off, &ae, &af, &a};

void rgb_init_spi(void){
    //COPI on p1.2, SCLK on p1.4
    P1SEL = BIT2 + BIT4;
    P1SEL2 = BIT2 + BIT4;

    UCA0CTL1=UCSWRST; //disable serial interface
    UCA0CTL0 |= UCCKPH + UCMSB + UCMST + UCSYNC;    // data cap at 1st clk edge, MSB first, master mode, synchronous
    UCA0CTL1 |= UCSSEL_2;                           // select SMCLK
    UCA0BR0 = 0;                                    //set frequency
    UCA0BR1 = 0;                                    //
    UCA0CTL1 &= ~UCSWRST;           // Initialize USCI state machine

}


//writes a 32 bit frame to the spi buffer
void rgb_send_frame(const uint8_t *frame1, bool wait_for_completion){
    int byte1;
    for (byte1=0;byte1<4;byte1++){//send 32 bit frame in 8 bit chunks
        UCA0TXBUF=frame1[byte1];
        while (!(IFG2 & UCA0TXIFG));  // USCI_A0 TX buffer ready?
    }
    if (wait_for_completion)
        while (!(IFG2 & UCA0RXIFG));  // USCI_A0 RX buffer ready? (indicates transfer complete)
}

void rgb_send_start() {
    rgb_send_frame(start_frame, false);
}

void rgb_send_end() {
    rgb_send_frame(end_frame, true);
}
void set_temperature(int LED1_temp, int LED2_temp, int LED3_temp, int LED4_temp){
            //simply call the send frame with the appropriate indices
            rgb_send_start();
            rgb_send_frame(alphabet[LED1_temp], false);
            rgb_send_frame(alphabet[LED2_temp], false);
            rgb_send_frame(alphabet[LED3_temp], false);
            rgb_send_frame(alphabet[LED4_temp], false);
            rgb_send_frame(&off, false);
            rgb_send_end();


}

