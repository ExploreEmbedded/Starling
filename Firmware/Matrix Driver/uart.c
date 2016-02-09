/***************************************************************************************************
                                   ExploreEmbedded	
****************************************************************************************************
 * File:   uart.c (AVR controllers)
 * Version: 15.0
 * Author: ExploreEmbedded
 * Website: http://www.exploreembedded.com/wiki
 * Description: File contains the Library routines for UART

The libraries have been tested on ExploreEmbedded development boards. We strongly believe that the 
library works on any of development boards for respective controllers. However, ExploreEmbedded 
disclaims any kind of hardware failure resulting out of usage of libraries, directly or indirectly.
Files may be subject to change without prior notice. The revision history contains the information 
related to updates. 


GNU GENERAL PUBLIC LICENSE: 
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. 

Errors and omissions should be reported to codelibraries@exploreembedded.com
 **************************************************************************************************/


/**************************************************************************************************
                             Revision History
****************************************************************************************************
15.0: Initial version 
***************************************************************************************************/
#include <stdarg.h>
#include "uart.h"
#include "delay.h"




/***************************************************************************************************
                         void UART_Init(uint32_t var_baudRate_u32)
****************************************************************************************************
 * I/P Arguments: uint32_t : Baudrate to be configured.
 * Return value	: none

 * description  :This function is used to initialize the UART at specified baud rate.
                 If the requested baud rate is not within the supported range then
                 the default baud rate of 9600 is set.


		    Refer uart.h file for Supported(range) baud rates.		
***************************************************************************************************/
void UART_Init(uint32_t var_baudRate_u32)
{
	UCSRB= (1<<RXEN) | (1<<TXEN)| (1<<RXCIE);                  // Enable Receiver and Transmitter
	UCSRC= (1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0);   // Asynchronous mode 8-bit data and 1-stop bit
	UCSRA= 0x00;                                   // Clear the UASRT status register
	UART_SetBaudRate(var_baudRate_u32);
}






/***************************************************************************************************
                         void UART_SetBaudRate(uint32_t var_baudRate_u32)
 ***************************************************************************************************
 * I/P Arguments: uint32_t : var_baudRate_u32 to be configured.
 * Return value	: none

 * description  :This function is used to Set/Change the baudrate on the fly.
                 If the requested baud rate is not within the supported range then
                 the default baudrate of 9600 is set.

		    Refer uart.h file for Supported range of baud rates.
***************************************************************************************************/
void UART_SetBaudRate(uint32_t var_baudRate_u32)
{
	uint16_t RegValue;

	if((var_baudRate_u32 >= C_MinBaudRate_U32) && (var_baudRate_u32<=C_MaxBaudRate_U32))
	{
		/* Check if the requested baudate is within range,
	     If yes then calculate the value to be loaded into baud rate generator. */
		RegValue = M_GetBaudRateGeneratorValue(var_baudRate_u32);
	}
	else
	{
		/*	 Invalid baudrate requested, hence set it to default baudrate of 9600 */
		RegValue = M_GetBaudRateGeneratorValue(9600);
	}

	UBRRL = util_ExtractByte0to8(RegValue);
	UBRRH = util_ExtractByte8to16(RegValue);
}





/***************************************************************************************************
                              char UART_RxChar()
 ***************************************************************************************************
 * I/P Arguments: none.
 * Return value	: char: Ascii value of the character received

 * description :This function is used to receive a char from UART module.
                It waits till a char is received and returns it after reception.
***************************************************************************************************/
char UART_RxChar()
{
	while(util_IsBitCleared(UCSRA,RXC));  // Wait till the data is received
	return(UDR);                          // return the received char
}








/***************************************************************************************************
                         void UART_TxChar(char var_uartData_u8)
****************************************************************************************************
 * I/P Arguments: char--> Ascii value of the character to be transmitted.
 * Return value	: none.

 * description  :This function is used to transmit a char through UART module.
***************************************************************************************************/
void UART_TxChar(char var_uartData_u8)
{
	while(util_IsBitCleared(UCSRA,UDRE)); // Wait till Transmitter(UDR) register becomes Empty
	UDR =var_uartData_u8;
	DELAY_ms(1);// Load the data to be transmitted
}







