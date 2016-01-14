#include <stdio.h>
#include <stdint.h>
#include "i2c_functions.h"
#include "conversion.h"
#include "mpudefines.h"
#include "mpu.h"

int mpu_reset_fifo(void){
    unsigned char data;

    //if (!(chip_cfg.sensors))
    //return -1;

    data = 0;
    i2c_write_address(MPU6050, INT_ENABLE, data, 1);
    i2c_write_address(MPU6050, FIFO_EN, data, 1);
    i2c_write_address(MPU6050, USER_CTRL, data, 1);

    if (1) { //assume truth TODO fix
        data = 0x04 | 0x08;
        i2c_write_address(MPU6050, USER_CTRL, data, 1);
        usleep(50000);
        data = 0x80 | 0x40;
        if (1) //chip_cfg.sensors & 0x01
            data |= 0x20;
        i2c_write_address(MPU6050, USER_CTRL, data, 1);
        if (1) //chip_cfg.int_enable
            data = 0x02;
        else
            data = 0;
        i2c_write_address(MPU6050, INT_ENABLE, data, 1);
        data = 0;
        i2c_write_address(MPU6050, FIFO_EN, data, 1);
    } else {
        data = 0x04;
        i2c_write_address(MPU6050, USER_CTRL, data, 1);
        if (0) //chip_cfg.bypass_mode || !(chip_cfg.sensors & 0x01)
            data = 0x40;
        else
            data = 0x04 | 0x20;
        i2c_write_address(MPU6050, USER_CTRL, data, 1);
        usleep(50000);
        if (1) //chip_cfg.int_enable
            data = 0x01;
        else
            data = 0;
        i2c_write_address(MPU6050, INT_ENABLE, data, 1);
        i2c_write_address(MPU6050, FIFO_EN, 1, 1); //chip_cfg.fifo_enable
    }
    return 0;
}

int mpu_set_fifo_rate(unsigned short rate){
    const unsigned char regs_end[12] = {0xFE, 0xF2, 0xAB,
                                        0xc4, 0xAA, 0xF1,
                                        0xDF, 0xDF, 0xBB,
                                        0xAF, 0xDF, 0xDF};
    unsigned short div;
    unsigned char tmp[8];

    if (rate > DMP_SAMPLE_RATE)
        return -1;
    div = DMP_SAMPLE_RATE / rate - 1;
    tmp[0] = (unsigned char)((div >> 8) & 0xFF);
    tmp[1] = (unsigned char)(div & 0xFF);
    if (i2c_write_mem(534, 2, tmp))
        return -1;
    if (i2c_write_mem(2753, 12, (unsigned char*)regs_end))
        return -1;
    return 0;
}

int mpu_set_lpf(unsigned short lpf)
{
    unsigned char data;
    //if (!(chip_cfg.sensors))
    //    return -1;

    if (lpf >= 188)
        data = 1;
    else if (lpf >= 98)
        data = 2;
    else if (lpf >= 42)
        data = 3;
    else if (lpf >= 20)
        data = 4;
    else if (lpf >= 10)
        data = 5;
    else
        data = 6;

    i2c_write_address(MPU6050, LPF, data, 1);
    return 0;
}

char mpu_read(){
    short intStatus;
    unsigned char tmp[2];
    unsigned char errCode;
    
    
    // getting int status
    /*if (!chip_cfg.sensors)
        return false;
    if (!chip_cfg.dmp_on)
        return false;*/
        
    i2c_read_multiple_addresses(MPU6050, DMP_INT_STATUS, tmp, 2);
    intStatus = (tmp[0] << 8) | tmp[1];
    
    // check int status
    if ((intStatus & 0x0102) != 0x0102){
        //Serial.println("data not ready");
        return 1; //data not ready
    }
    // read fifo
    if (errCode = mpu_read_fifo()){
        //Serial.print("fifo read failed with code: ");
        //Serial.println(errCode);
        return 1;
    }
    uint16_t smallInt = 0;
    i2c_read_multiple_addresses(MPU6050, 0x43, &smallInt, 2);
    const int negative = (smallInt & (1 << 15)) != 0;

    if (negative)
      rawaccel = smallInt | ~((1 << 16) - 1);
    else
      rawaccel = smallInt;
    rawaccel = rawaccel/131;

    
    return 0;
}

int mpu_read_fifo(){
    unsigned char fifo_data[32];
    int errCode;
    unsigned char more;

    // Get a packet
    unsigned char tmp[2];
    unsigned short fifo_count;
    i2c_read_multiple_addresses(MPU6050, FIFO_COUNT_H, tmp, 2);
    fifo_count = (tmp[0] << 8) | tmp[1];
    if (fifo_count < PACKET_LENGTH) {
        return -1;
    }
    if (fifo_count > (MAX_FIFO >> 1)) {
        // check overflow
        tmp[0] = i2c_read_address(MPU6050, INT_STATUS, 1);
        if (tmp[0] & 0x10) {
            mpu_reset_fifo();
            return -2;
        }
    }
    i2c_read_multiple_addresses(MPU6050, FIFO_RW, fifo_data, PACKET_LENGTH);

    rawQuat[0] = ((long)fifo_data[0] << 24) | ((long)fifo_data[1] << 16) |
        ((long)fifo_data[2] << 8) | fifo_data[3];
    rawQuat[1] = ((long)fifo_data[4] << 24) | ((long)fifo_data[5] << 16) |
        ((long)fifo_data[6] << 8) | fifo_data[7];
    rawQuat[2] = ((long)fifo_data[8] << 24) | ((long)fifo_data[9] << 16) |
        ((long)fifo_data[10] << 8) | fifo_data[11];
    rawQuat[3] = ((long)fifo_data[12] << 24) | ((long)fifo_data[13] << 16) |
        ((long)fifo_data[14] << 8) | fifo_data[15];
    
    return 0;
}

void mpu_calculate_angles_2(){
    // convert quaternion
    dmpQuat[QUATW] = (float)rawQuat[QUATW];
    dmpQuat[QUATX] = (float)rawQuat[QUATX];
    dmpQuat[QUATY] = (float)rawQuat[QUATY];
    dmpQuat[QUATZ] = (float)rawQuat[QUATZ];
    converse_quaternion_normalize(dmpQuat);
    converse_fixed_quat_to_euler(dmpQuat, dmpEuler);
}

void mpu_print_angles(float *vec){
    printf("%f, %f, %f\n", vec[VEC3_X] * RAD_TO_DEGREE, (vec[VEC3_Y]) * RAD_TO_DEGREE, vec[VEC3_Z] * RAD_TO_DEGREE);
}