#include <avr\io.h>
#include<avr/pgmspace.h>
#include<avr/interrupt.h>
#include <string.h>
#include "delay.h"
#include"sw_uart.h"
#include "uart.h"
#include "config.h"
#include "twi.h"
#include "adc.h"



//#define SIMULATION 0x00  //0xff for Simulation, 0x00 for real Time
#define MaxSegments 15
#define MaxColPerSegment 8

uint8_t NumberOfSegments = MaxSegments;

//#define EnableBlankSpaces

#ifdef SMALLDISPLAY
#define SIMULATION 0xff
#else
#define SIMULATION 0x00
#endif

//uint8_t pattern[16] = {0xc3,0x91,0x24,0x20,0x20,0x24,0x91,0xc3};
uint8_t pattern[16] = {0x3c,0x6e,0xdb,0xdf,0xdf,0xdb,0x6e,0x3c};    //pattern for animation
uint8_t colSelection[8] = {1,2,4,8,16,32,64,128}; /* Select One column at atime */


volatile uint8_t MyDeviceId = 0x30,NextDeviceId = 0x32,IdConfirmed=0,sendNewIdCount = 0;
volatile uint16_t debounceCount = 0;
volatile uint8_t TimeTick = 0;
uint8_t deviceType = MASTER;
uint8_t state = E_Init;
extern const uint8_t Font[][8];

volatile uint8_t NewMsgLength = 0;
volatile uint8_t NewMsgReceived = 0;
uint8_t isrBuffer[MSG_SIZE];
volatile uint8_t inputBuffer[MSG_SIZE]={"hello"};
uint8_t msgBuffer[MSG_SIZE] = "ABCDEFGHIJ";
uint8_t FrameBuffer[MAX_DEVICES*FONT_SIZE];

volatile uint8_t isrColIndex;
uint8_t MsgLength = 10;



void BuildFrame(uint8_t *msgPtr);
void GetFont(uint8_t ch,uint8_t *font);
void StartDisplayTimer();
void ScrollString(char *ptr_msgPointer);
void DisplayString(char *ptr_msgPointer);
void TWI_ScrollFrame(void);




#define DELAY_TIME 75

int main()
{
    int16_t length=0,adc_value1,adc_value2;
    uint8_t i;
    DELAY_ms(100);
    ADC_Init();
    TIMSK = 0x00;
    DDRB= 0xff;
    DDRC = 0xff;
    DDRD = 0xff;
    PORTC = 0xff;



    while(1)
    {

        switch(state)
        {
        case E_Init:
            DELAY_ms(200);
            ADC_Init();
            adc_value1 = ADC_GetAdcValue(6);
            adc_value2 = ADC_GetAdcValue(7);

           // UART_Init(BAUD_RATE);
          //  sw_uart_init();
            StartDisplayTimer();

            sei();
          //  GetFont('1',pattern);

            while(1);
            MsgLength = strlen(inputBuffer);


            if( (adc_value1 >= 600) && (adc_value2 >= 600) )
            {
                deviceType = MASTER;
            }
            else
            {
                deviceType = SLAVE;
            }

            state = E_StartArbitration;
            break;

        case E_StartArbitration:

            GetFont(((MyDeviceId-0x30)/2)+0x30,pattern);
            if(deviceType == MASTER)
            {
                DELAY_ms(500);

                TCCR2 = 0x00;
                TIMSK = 0x01;
                TWI_init_master();
                state = E_MasterDisp;
            }
            else
            {
                if(IdConfirmed == 1)
                {
                    DELAY_ms(250);

                    TCCR2 = 0x00;
                    TIMSK = 0x01;
                    DisableHwUartComm();
                    TWI_init_slave(MyDeviceId);
                    state = E_SlaveDisp;
                }
            }
            break;


        case E_MasterDisp:

            if(NewMsgReceived!=0)
            {

                MsgLength = NewMsgLength;
                NewMsgReceived = 0;
                NewMsgLength = 0;
            }

            strcpy(msgBuffer,inputBuffer);

            if(MsgLength > NumberOfSegments)
            {
                ScrollString(msgBuffer);
            }
            else
            {

                DisplayString(msgBuffer);
            }

            break;

        case E_SlaveDisp:

            TWI_Receive_Frame(FrameBuffer);
            for(i=0;i<8;i++)
                pattern[i] = FrameBuffer[i];

            break;
        }
    }
}



void BuildFrame(uint8_t *msgPtr)
{
    uint8_t i,endOfFrame=0;

    for(i=0;i<(MAX_DEVICES);i++)
    {

        if(endOfFrame == 0)
        {
            if(msgPtr[i] == 0)
            {
                endOfFrame = 1;
            }
        }


        if(endOfFrame == 0)
        {
            GetFont(msgPtr[i],(FrameBuffer+(i*8)));
        }
        else
        {
            GetFont(' ',(FrameBuffer+(i*8)));
        }
    }
}



void DisplayString(char *ptr_msgPointer)
{
    int i,j;
    uint8_t activeStatus = 1;
    NumberOfSegments = 1;
    BuildFrame(ptr_msgPointer);

    for(i=0;activeStatus;i++)
    {
        if(i==0)
        {
            for(j=0;j<8;j++)
                pattern[j] = FrameBuffer[j];
        }
        else
        {
            TWI_start(); // Function to send start condition

            activeStatus = TWI_write_address(i*2+0x30); // Function to write address and data direction bit(write) on SDA
            if(activeStatus)
            {
                NumberOfSegments++;
                for(j=0;j<8;j++)
                {
                    TWI_write_data(FrameBuffer[i*8+j]);      // Function to write data in slave
                    //   UART_TxChar(' ');
                    // UART_TxHexNumber(i,2);      // Function to write data in slave
                }
            }
            TWI_stop(); // Function to send stop condition
        }
    }
}







void ScrollString(char *ptr_msgPointer)
{
    unsigned char i,len;

    len = NumberOfSegments+ MsgLength;
    ptr_msgPointer[len+1] = 0;
    ptr_msgPointer[len] = ' ';
    for(i=len-1;i>(NumberOfSegments-1);i--)
    {
        ptr_msgPointer[i] = ptr_msgPointer[i-(NumberOfSegments)];
    }
    for(i=0;i<(NumberOfSegments);i++)
    {
        ptr_msgPointer[i] = ' ';
    }

    for(i=0;(ptr_msgPointer[i]) && (NewMsgReceived==0);i++)
    {
        /* Loop to display the complete string, each time N chars are displayed and
                pointer is incremented to point to next char */

        BuildFrame(ptr_msgPointer+i);

        TWI_ScrollFrame();
        /*   if(NewMsgReceived == 1)
            {
                BuildFrame(ptr_msgPointer+i+1);
                TWI_HorizontalScrollFrame(0);
            } */
    }
}


void TWI_ScrollFrame(void)
{
    uint8_t segindex=0,i,j;
    uint8_t str_Ptr[8];
    uint8_t activeStatus = 1;


    while((segindex<8) && (NewMsgReceived==0))
    {
        activeStatus = 1;
        NumberOfSegments = 1;
        for(i=0; (activeStatus) && (NewMsgReceived==0);i++)
        {


            for(j=0;j<MaxColPerSegment;j++)
            {

                str_Ptr[j] = FrameBuffer[(i*8)+j+segindex];
            }



            if(i==0)
            {
                for(j=0;j<MaxColPerSegment;j++)
                {
                    pattern[j] = str_Ptr[j];
                }
            }
            else
            {

                activeStatus = TWI_start(); // Function to send start condition

                if(activeStatus)
                {

                    activeStatus = TWI_write_address(i*2+0x30); // Function to write address and data direction bit(write) on SDA

                    if(activeStatus)
                    {

                        NumberOfSegments++;
                        for(j=0;j<8;j++)
                        {
                            TWI_write_data(str_Ptr[j]);      // Function to write data in slave
                        }
                    }
                }

                TWI_stop(); // Function to send stop condition
            }
        }

        for(i=NumberOfSegments;i<MAX_DEVICES;i++)
            DELAY_ms(SCROLL_DELAY_MS);
        segindex++;
    }
}










void GetFont(uint8_t ch,uint8_t *font)
{
    uint8_t i=0;
    uint8_t *ptr;
    ch = ch-0x20;
    ptr =(uint8_t*)Font[ch];

    for(i=0;i<8;i++)
    {
        font[i] = pgm_read_byte(ptr+i);
    }
}


void StartDisplayTimer()
{
    TCCR0 = 0X01;
    TCNT0 = 253;
    TIMSK|=0x01 ;
}

ISR (TIMER0_OVF_vect)
{
    char val = (colSelection[isrColIndex]);

    PORTB = 0x00;

    PORTC = (PORTC & 0xF0)| (val & 0x0F);   // Higher four Rows/Cols are connected to PC4-PC7
    PORTD = (PORTD & 0x0F) | (val & 0xF0);  // Lower four Rows/Cols are connected to PD0-PD3

    PORTB = pattern[isrColIndex];

    isrColIndex++;
    if(isrColIndex == 8)
    {
        isrColIndex = 0x00;

        if(state == E_StartArbitration)
        {
            if(sendNewIdCount == 10)
            {
                NextDevice_SendData(NextDeviceId);
                sendNewIdCount = 0;
            }
            else
            {
                sendNewIdCount++;
            }
        }
    }
    TimeTick++;
}



void handleUartData()
{
    char ch;
    static uint8_t inputBufferIndex = 0;
    ch = UDR;
    if(deviceType == MASTER)
    {

        if((ch == '\n') || (ch == '\r'))
        {
            if(inputBufferIndex > 0)
            {
                for(NewMsgLength=0;NewMsgLength<inputBufferIndex;NewMsgLength++)
                {
                    inputBuffer[NewMsgLength]= isrBuffer[NewMsgLength];
                }
                inputBuffer[NewMsgLength] = 0;
                NewMsgReceived = 1;
                inputBufferIndex= 0x00;
            }
        }
        else if(inputBufferIndex<(MSG_SIZE-1))
        {
             UDR = ch;
            isrBuffer[inputBufferIndex] = ch;
            inputBufferIndex++;
        }
    }
    else
    {
        if(IdConfirmed == 0)
        {
            if((ch>0x30) && (ch<0x50))
            {
                if(MyDeviceId == ch)
                {
                    if(debounceCount>=100)
                    {
                        IdConfirmed = 1;
                    }
                    else
                    {
                        debounceCount++;
                    }
                }
                else
                {
                    MyDeviceId =  ch;
                    NextDeviceId =  ch + 2; /* Device Ids start form 0x30,0x32---0x42 */
                }
            }
        }
    }
}


ISR (USART_RXC_vect)
{
    handleUartData();
}
