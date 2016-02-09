#ifndef _TWI_H_
#define _TWI_H_


uint8_t TWI_start(void);
void TWI_repeated_start(void);
void TWI_init_master(void);
uint8_t TWI_write_address(unsigned char);
void TWI_read_address(unsigned char);
void TWI_write_data(unsigned char);
void TWI_read_data(void);
void TWI_stop(void);
void TWI_init_slave(uint8_t slaveId);
char TWI_read_slave(void);
void TWI_match_read_slave(void);
void TWI_Receive_Frame(uint8_t *str);



#endif
