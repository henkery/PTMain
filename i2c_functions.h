#ifndef I2C_FUNC_H
#define I2C_FUNC_H

void i2c_write_address(unsigned char device_addr, unsigned char register_addr, unsigned char val, char stop);
char i2c_write_multiple_addresses(unsigned char device_addr, unsigned char start_addr, unsigned char val[], int amount_of_bytes);
int i2c_write_mem(unsigned short mem_addr, unsigned short length, uint8_t *data);
unsigned char i2c_read_address(unsigned char device_addr, unsigned char register_addr, char stop);

#endif