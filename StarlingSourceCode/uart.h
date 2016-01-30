/***************************************************************************************************
                                   ExploreEmbedded	
****************************************************************************************************
 * File:   uart.h
 * Version: 15.0
 * Author: ExploreEmbedded
 * Website: http://www.exploreembedded.com/wiki
 * Description: Contains the baudrate configurations and function prototypes for UART routines

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



/***************************************************************************************************
                             Revision History
****************************************************************************************************
15.0: Initial version 
***************************************************************************************************/
#ifndef _UART_H
#define _UART_H

#include <avr\io.h>
#include "stdutils.h"

/***************************************************************************************************
                             Baudrate configurations
***************************************************************************************************/
#define C_MinBaudRate_U32 2400
#define C_MaxBaudRate_U32 115200UL

#define M_GetBaudRateGeneratorValue(baudrate)  (((F_CPU -((baudrate) * 8L)) / ((baudrate) * 16UL)))
/**************************************************************************************************/


#define DisableHwUartComm() (UCSRB = 0x00)





/***************************************************************************************************
                             Function Prototypes
***************************************************************************************************/

void UART_Init(uint32_t var_baudRate_u32);
void UART_SetBaudRate(uint32_t var_baudRate_u32);
void UART_TxChar(char var_uartData_u8);
char UART_RxChar();


#endif
