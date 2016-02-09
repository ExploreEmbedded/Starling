#ifndef _CONFIG_H_
#define _CONFIG_H_


#define TWI_TIMEOUT_VALUE     10
#define SCROLL_DELAY_MS       1
#define SLAVE     0
#define MASTER    1


#define MAX_DEVICES      10
#define MSG_SIZE         240
#define FONT_SIZE        8

/*
typedef enum
{
    E_Init,
    E_StartArbitration,
    E_StopArbitration,
    E_MasterDisp,
    E_SlaveDisp
}states_t;
*/

#define E_Init                0x00
#define E_StartArbitration    0x01
#define E_StopArbitration     0x02
#define E_MasterDisp          0x03
#define E_SlaveDisp           0x04

#define NextDevice_SendData(x)        sw_UART_Transmit_char(x)
#define NextDevice_Receivedata(x)      sw_UART_Receive_char(x)

#define PrevDevice_SendData(x)        UART_TxChar(x)



#endif
