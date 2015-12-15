#include <linux/i2c-dev.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "mpudefines.h"
#include "i2c_functions.h"

void i2c_write_address(unsigned char device_addr, unsigned char register_addr, unsigned char val, char stop){
    int file;
    char *filename = "/dev/i2c-1";
    if ((file = open(filename, O_RDWR)) < 0) {
        printf("Failed to open device\n");
        /* ERROR HANDLING: you can check errno to see what went wrong */
        //perror("Failed to open the i2c bus");
        //return 1;
    }
    int addr = device_addr;          // The I2C address of the MPU
    if (ioctl(file, I2C_SLAVE, addr) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        /* ERROR HANDLING; you can check errno to see what went wrong */
    }

    uint8_t buffer[2];

    buffer[0] = register_addr;
    buffer[1] = val;

    if (write(file, buffer, 2) != 2) {
        printf("Can not write data. Address %d.\n", device_addr);
    }
    close(file);
}

unsigned char i2c_read_address(unsigned char device_addr, unsigned char register_addr, char stop){
    int file;
    char *filename = "/dev/i2c-1";
    if ((file = open(filename, O_RDWR)) < 0) {
        printf("Failed to open device\n");
        /* ERROR HANDLING: you can check errno to see what went wrong */
        //perror("Failed to open the i2c bus");
        //return 1;
    }
    int addr = device_addr;          // The I2C address of the MPU
    if (ioctl(file, I2C_SLAVE, addr) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        /* ERROR HANDLING; you can check errno to see what went wrong */
    }

    uint8_t buffer[1];

    buffer[0] = register_addr;

    if (write(file, buffer, 1) != 1) {
        printf("Can not write data. Address %d.\n", device_addr);
    }

    close(file);

    if ((file = open(filename, O_RDWR)) < 0) {
        printf("Failed to open device\n");
        /* ERROR HANDLING: you can check errno to see what went wrong */
        //perror("Failed to open the i2c bus");
        //return 1;
    }
    addr = device_addr;          // The I2C address of the MPU + 1
    if (ioctl(file, I2C_SLAVE, addr) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        /* ERROR HANDLING; you can check errno to see what went wrong */
    }

    unsigned char p;
    if (read(file, &p, 1) != 1) {
        printf("Can not read data. Address %d.\n", device_addr);
    }
    close(file);
    return p;
}

char i2c_write_multiple_addresses(unsigned char device_addr, unsigned char start_addr, unsigned char val[], int amount_of_bytes){
    int file;
    char ret =0;
    char *filename = "/dev/i2c-1";
    if ((file = open(filename, O_RDWR)) < 0) {
        printf("Failed to open device\n");
        /* ERROR HANDLING: you can check errno to see what went wrong */
        //perror("Failed to open the i2c bus");
        //return 1;
    }
    int addr = device_addr;          // The I2C address of the MPU
    if (ioctl(file, I2C_SLAVE, addr) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        /* ERROR HANDLING; you can check errno to see what went wrong */
    }

    uint8_t buffer[1+amount_of_bytes];

    buffer[0] = start_addr;
    memcpy(buffer+1, val, amount_of_bytes);

    if (write(file, buffer, amount_of_bytes+1) != amount_of_bytes+1) {
        ret = 1;
        printf("Can not write data. Address %d.\n", device_addr);
    }

    /*unsigned char *p = &val[0];
    if (write(file, val, amount_of_bytes) != amount_of_bytes) {
        ret=1;
        printf("Can not write data. Address %d.\n", device_addr);
    }*/
    close(file);
    return ret;
}

void i2c_read_multiple_addresses(unsigned char device_addr, unsigned char start_addr, unsigned char val[], int amount_of_bytes){
    int file;
    char *filename = "/dev/i2c-1";
    if ((file = open(filename, O_RDWR)) < 0) {
        printf("Failed to open device\n");
        /* ERROR HANDLING: you can check errno to see what went wrong */
        //perror("Failed to open the i2c bus");
        //return 1;
    }
    int addr = device_addr;          // The I2C address of the MPU
    if (ioctl(file, I2C_SLAVE, addr) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        /* ERROR HANDLING; you can check errno to see what went wrong */
    }

    uint8_t buffer[1];

    buffer[0] = start_addr;

    if (write(file, buffer, 1) != 1) {
        printf("Can not write data. Address %d.\n", device_addr);
    }

    close(file);

    if ((file = open(filename, O_RDWR)) < 0) {
        printf("Failed to open device\n");
        /* ERROR HANDLING: you can check errno to see what went wrong */
        //perror("Failed to open the i2c bus");
        //return 1;
    }
    addr = device_addr;          // The I2C address of the MPU + 1
    if (ioctl(file, I2C_SLAVE, addr) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        /* ERROR HANDLING; you can check errno to see what went wrong */
    }

    unsigned char *p = &val[0];
    if (read(file, val, amount_of_bytes) != amount_of_bytes) {
        printf("Can not read data. Address %d.\n", device_addr);
    }
    //*p = read_byte(1);
    close(file);
}

int i2c_write_mem(unsigned short mem_addr, unsigned short length, uint8_t *data)
{
    unsigned char tmp[2];

    if (!data)
        return -1;
    //if (!chip_cfg.sensors)
    //    return -2;

    tmp[0] = (unsigned char)(mem_addr >> 8);
    tmp[1] = (unsigned char)(mem_addr & 0xFF);

    /* Check bank boundaries. */
    if (tmp[1] + length > BANK_SIZE)
        return -3;
    if (i2c_write_multiple_addresses(MPU6050, BANK_SEL, tmp, 2))
        return 1;
    if (i2c_write_multiple_addresses(MPU6050, MEM_RW, data, length))
        return 1;
    return 0;
}

int i2c_read_mem(unsigned short mem_addr, unsigned short length, uint8_t *data)
{
    unsigned char tmp[2];

    if (!data)
        return -1;
    //if (!chip_cfg.sensors)
    //    return -2; TODO fix this

    tmp[0] = (unsigned char)(mem_addr >> 8);
    tmp[1] = (unsigned char)(mem_addr & 0xFF);

    /* Check bank boundaries. */
    if (tmp[1] + length > BANK_SIZE)
        return -3;
    i2c_write_multiple_addresses(MPU6050, BANK_SEL, tmp, 2);
    i2c_read_multiple_addresses(MPU6050, MEM_RW, data, length);
    return 0;
}