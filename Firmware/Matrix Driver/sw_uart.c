/*****************************************
Filename  : sw_uart.h
Author    : Prashant Mehta
Date	  : 26-06-2009
 ******************************************/
/*                   
 *******************************************************************************************************************
 *  Header file for SOFTWARE UART using timer 0.									  *
 *  Timer 0 is set to 3 times the required baud rate because data sampling has to be done 3 times per bit	  *
 *  5,6,7,8 bit Tx and Rx possible by this s/w UART								  *
 *  1 or 2 Stop bits as pre-defined by user									  *
 *  Possible to use parity bit (even or odd) along with data frame						  *
 *  parity_error flag set if parity error									  *
 *  Use sw_uart_config.h to configure Software UART. Dont make any changes here.  				  *
 *  sw_uart_init()              -- Function to initialize Software UART						  *
 *  sw_UART_Transmit_char(char) -- Function for Transmitting character						  *
 *  sw_UART_Receive_char()      -- Function for Receiving character						  *
 *******************************************************************************************************************
 */

#include<avr/io.h>
#include<avr/interrupt.h>
#include"sw_uart.h"
#include"delay.h"
#include "uart.h"
#include "config.h"




/******************************************* Software UART variables  **********************************************/
/*volatile static unsigned char		inbuf[IN_BUF_SIZE];
volatile static unsigned char		qin = 0;
volatile static unsigned char		qout = 0;
 */

volatile unsigned static char 			flag_tx_ready;
volatile unsigned static char 			timer_tx_ctr;
volatile unsigned static char 			bits_left_in_tx;
volatile unsigned static char 			tx_num_of_bits;
volatile unsigned static short			internal_tx_buffer;
volatile unsigned static char			user_tx_buffer;
volatile static unsigned char           tx_num_of_ones=0;
volatile static unsigned int            frame;
/*volatile unsigned static char			num_of_databits;
volatile unsigned static char			parity_error;

volatile static unsigned char    		flag_rx_off;
volatile static unsigned char   		flag_rx_ready;

volatile static unsigned char			rx_num_of_ones=0;

volatile static unsigned int			rx_parity_bit;
volatile static unsigned char           NewChar = 0x31; */
/*********************************************************************************************************************/



/* Function to initialize uart PB7 as TX pin and PG3 as RX pin.
   Also initialize timer 0 for delay of 3 times the baud rate*/
void sw_uart_init(void)
{
    sw_uart_io_init();

    /*************************************************/

    flag_tx_ready = FALSE;
    tx_num_of_bits = NUM_OF_DATABITS+NUM_OF_STOPBITS+1+PARITY_BIT;
    set_tx_pin_high();
    TCNT2=0x0;
    TIMSK|=0x80;
    TCCR2=0x0a;                 // Prescalar 32
    OCR2=(CPU_CLOCK_HZ)/(BAUD_RATE)/8/3;				// start timer 0
    //  softuart_turn_rx_on();
}
/*********************************************************************************************************************/


/******************************************  Timer ISR on oveflow  ***********************************************/

ISR (TIMER2_COMP_vect)					// Interrupt on compare match
{
    char mask;
    // static unsigned char flag_rx_waiting_for_stop_bit = FALSE;
    // static unsigned char rx_mask;

    //  static char timer_rx_ctr;	// Counter for sampling at reciption
    //  static char bits_left_in_rx;
    //  static unsigned char internal_rx_buffer;

    //  char start_bit, flag_in;
    /***                 Transmitter section                 ***/
    if(flag_tx_ready)
    {

        if(--timer_tx_ctr<=0)
        {

            mask=internal_tx_buffer&1;
            internal_tx_buffer>>=1;
            if(mask)
            {
                set_tx_pin_high();

            }
            else
            {
                set_tx_pin_low();
            }
            timer_tx_ctr = 3;
            if ( --bits_left_in_tx<=0 )
            {
                flag_tx_ready = FALSE;
            }
        }
    }
}
/***********************************************************************************************************************/

/*********************************** Function to transmit a character on Tx pin ****************************************/
void sw_UART_Transmit_char(char ch)
{
    if( flag_tx_ready == FALSE)
    {
        user_tx_buffer = ch;
        timer_tx_ctr = 3;
        bits_left_in_tx = tx_num_of_bits;
        frame_calc(user_tx_buffer);
        internal_tx_buffer = (user_tx_buffer<<1)|frame;
        flag_tx_ready = TRUE;
    }
}
/*********************************************************************************************************************/


/***********************************************************************************************************************/







/************************************ Function to initialise s/w uart Tx and Rx ports ***********************************/
void sw_uart_io_init(void)
{
    // TX-Pin as output
    UART_TXDDR |=  ( 1 << UART_TXBIT );
    // RX-Pin as input
    UART_RXDDR &= ~( 1 << UART_RXBIT );
    // RX-pin pull-up
    UART_RXPORT |= (1  << UART_RXBIT );
}
/************************************************************************************************************************/

/****************************************** Function to set Tx pin(PB7) high ********************************************/

/***********************************************************************************************************************/


/***************************  Function for adding parity bit & stop bits into data frame *******************************/
void frame_calc(unsigned char ch)
{
    unsigned char temp=0,i;
    for(i=0;i<=NUM_OF_DATABITS;i++)
    {
        temp=ch&1;
        ch>>=1;
        if(temp==1)
            tx_num_of_ones++;
    }
    frame=0x200>>(8-NUM_OF_DATABITS); //OneStop Bit
}


/***********************************************************************************************************************/
