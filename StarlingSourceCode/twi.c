#include <avr\io.h>
#include "delay.h"
#include "config.h"
#include "twi.h"

extern volatile uint8_t TimeTick;

void TWI_init_master(void) // Function to initialize master
{
    TWBR=0x01; // Bit rate
    TWSR=(0<<TWPS1)|(0<<TWPS0); // Setting prescalar bits
    // SCL freq= F_CPU/(16+2(TWBR).4^TWPS)
}

void TWI_init_slave(uint8_t slaveId) // Function to initilaize slave
{
    TWAR=slaveId; // Fill slave address to TWAR
}


uint8_t TWI_start(void)
{
    uint8_t timeout,returnStatus = 1;
    // Clear TWI interrupt flag, Put start condition on SDA, Enable TWI
    timeout = TimeTick + TWI_TIMEOUT_VALUE;
    TWCR= (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    while(!(TWCR & (1<<TWINT))) // Wait till start condition is transmitted
    {
        if(timeout == TimeTick)
        {
            returnStatus = 0;
            break;
        }
    }
    while(timeout != TimeTick);

    timeout = TimeTick + TWI_TIMEOUT_VALUE;

    while((TWSR & 0xF8)!= 0x08) // Check for the acknowledgement
    {
        {
            if(timeout == TimeTick)
            {
                returnStatus = 0;
                break;
            }
        }
    }
    while(timeout != TimeTick);
    return returnStatus;
}



uint8_t TWI_write_address(unsigned char data)
{
    uint8_t timeout,returnStatus = 1;
    timeout = TimeTick + TWI_TIMEOUT_VALUE;
    TWDR=data; // Address and write instruction
    TWCR=(1<<TWINT)|(1<<TWEN);    // Clear TWI interrupt flag,Enable TWI
    while (!(TWCR & (1<<TWINT))) // Wait till complete TWDR byte transmitted
    {
        if(timeout == TimeTick)
        {
            returnStatus = 0;
            break;
        }
    }

    while(timeout != TimeTick);


    timeout = TimeTick + TWI_TIMEOUT_VALUE;

    while((TWSR & 0xF8)!= 0x18)  // Check for the acknoledgement
    {
        if(timeout == TimeTick)
        {
            returnStatus = 0;
            break;
        }
    }

    while(timeout != TimeTick);


    return returnStatus;
}



void TWI_write_data(unsigned char data)
{
    uint8_t timeout = TimeTick + TWI_TIMEOUT_VALUE;
    TWDR=data; // put data in TWDR
    TWCR=(1<<TWINT)|(1<<TWEN);    // Clear TWI interrupt flag,Enable TWI
    while (!(TWCR & (1<<TWINT))) // Wait till complete TWDR byte transmitted
    {
        if(timeout == TimeTick)
            break;
    }
    while((TWSR & 0xF8) != 0x28) // Check for the acknoledgement
    {
        if(timeout == TimeTick)
            break;
    }
}






char TWI_read_slave(void)
{
    char recv_data;
    // Clear TWI interrupt flag,Get acknowlegement, Enable TWI
    TWCR= (1<<TWINT)|(1<<TWEA)|(1<<TWEN);
    while (!(TWCR & (1<<TWINT))); // Wait for TWINT flag
    while((TWSR & 0xF8)!=0x80); // Wait for acknowledgement
    recv_data=TWDR; // Get value from TWDR
    return (recv_data); // send the receive value on PORTB
}

void TWI_match_read_slave(void) //Function to match the slave address and slave dirction bit(read)
{
    while((TWSR & 0xF8)!= 0x60)  // Loop till correct acknoledgement have been received
    {
        // Get acknowlegement, Enable TWI, Clear TWI interrupt flag
        TWCR=(1<<TWEA)|(1<<TWEN)|(1<<TWINT);
        while (!(TWCR & (1<<TWINT)));  // Wait for TWINT flag
    }
}


void TWI_stop(void)
{
    uint8_t timeout = TWI_TIMEOUT_VALUE;
    // Clear TWI interrupt flag, Put stop condition on SDA, Enable TWI
    TWCR= (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
    while(!(TWCR & (1<<TWSTO)))  // Wait till stop condition is transmitted
    {
        timeout--;
        DELAY_us(10);
        if(timeout == 0)
            break;
    }
}

void TWI_Receive_Frame(uint8_t *str)
{
    int i;
    TWI_match_read_slave(); //Function to match the slave address and slave dirction bit(read)
    while(1)
    {
        str[i] = TWI_read_slave(); // Function to read data
        i++;
        if( (i==8))
        {
            str[i] = 0x00;
            break;
        }
    }
}
