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

#ifndef _sw_uart_h_
#define _sw_uart_h_

#include<avr/io.h>



#define EVEN 0
#define ODD 1

#define NO 0
#define YES 1
/*************************************** PORT to be used for Tx and Rx *********************************************/
#define UART_RXPIN   PIND
#define UART_RXDDR   DDRD
#define UART_RXBIT   PD2
#define UART_RXPORT  PORTD

#define UART_TXPORT  PORTD
#define UART_TXDDR   DDRD
#define UART_TXBIT   PD3
/********************************************************************************************************************/

/************************* Define Oscillator frequency and required baud rate for UART here *************************/
#define CPU_CLOCK_HZ 8000000
#define BAUD_RATE 9600
/********************************************************************************************************************/

/**************************** Define number of data bits and number of stop bits here *******************************/
#define NUM_OF_DATABITS 8
#define NUM_OF_STOPBITS 1
/********************************************************************************************************************/


/************************************ Make U2X2 1 to double Baud rate ***********************************************/
#define U2X2 0
/********************************************************************************************************************/

/**************************** Define if you want to use Parity bit in SW UART here *********************************/
/****************************** If using parity bit define whether EVEN or ODD Parity *******************************/
#define PARITY_BIT NO
#define PARITY ODD
/********************************************************************************************************************/


#define IN_BUF_SIZE		16

#define set_tx_pin_high()  UART_TXPORT |=  ( 1<< UART_TXBIT )
#define set_tx_pin_low()   UART_TXPORT &= ~( 1<<UART_TXBIT )

void sw_uart_init(void);
void sw_UART_Transmit_char(char ch);
char sw_UART_Receive_char( uint8_t numOfDevices );
uint8_t SendReceiveId(uint8_t id);
char sw_UART_Receive_Buffer(void);
int get_rx_pin_status(void);
void softuart_turn_rx_on( void );
void softuart_turn_rx_off( void );
void sw_uart_io_init(void);
void flush_input_buffer(void);
void frame_calc(unsigned char ch);


#endif
