#ifndef I2C_H
#define I2C_H








void i2c_set_port(uint32_t port);


uint8_t i2c_read();
void i2c_write(uint8_t value);


void i2c_read_multiple(void* buffer,uint32_t length);
void i2c_write_multiple(void* buffer,uint32_t length);


/*
void i2c_read_port(uint8_t *buffer, uint32_t length, uint32_t port);
void i2c_write_port(uint8_t *buffer, uint32_t length, uint32_t port);
*/








#endif