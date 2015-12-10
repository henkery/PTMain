//This is the sensors module of the PT control system.
#include <stdint.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include "header.h"
#include "sensors.h"
#include "mpudefines.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int run;
long rawQuat[4];

signed char gyro_orientation[9] = { 1, 0, 0,
                                    0, 1, 0,
                                    0, 0, 1};

void *sns_sensor_loop(void* vd_data)
{
    mn_core_data *data = (mn_core_data *)vd_data;
    write_address(MPU6050, 0x6B, 0x00, 0);
    //sns_mpu_init(100, 42);
    sns_mpu_newinit();

    while (data->run) {
        uint8_t buffer[14];
        readByteBuffer(0x68, 0x3B, buffer, 14);
    }
        /*if (mpu_read())
        {
            printf("Sensor is not ready\n");
        }
        else
        {
            printf("data %li, %li, %li, %li,\n", rawQuat[0], rawQuat[1], rawQuat[2], rawQuat[3]);
        }
    }
    return 0; */
}

int sns_sensor_run(pthread_t *thread, mn_core_data *data) {
    pthread_create(thread, NULL, sns_sensor_loop, data);
    return 0;
}

char sns_mpu_newinit()
{
    writeBits(0x68, 0x6B, 0x01, 3, 0);
    writeBits(0x68, 0x1B, 0x00, 2, 3);
    writeBits(0x68, 0x1C, 0x00, 2, 3);
    writeBit(0x68, 0x6B, 1, 6);
}


char sns_mpu_init(int mpuRate, int lpf){
    int result;
    
    // Check validity of data
    /*if ((mpuRate > 1000) || (mpuRate < 1)){
        return -1;f
    }*/
    // Reset device 
    write_address(MPU6050, PWR_MGMT_1, 0x80, 1);
    usleep(100000);
    // Turn on device 
    write_address(MPU6050, PWR_MGMT_1, 0x00, 1);
    // Set MPU init values
    /*chip_cfg.sensors = 0x78;
    chip_cfg.sample_rate = 0xFFFF;
    chip_cfg.active_low_int = 1;
    chip_cfg.latched_int = 0;
    chip_cfg.int_motion_only = 0;
    chip_cfg.lp_accel_mode = 0;
    chip_cfg.dmp_on = 0;
    chip_cfg.dmp_loaded = 0;
    chip_cfg.dmp_sample_rate = 0;*/
    // Set gyro fsr (2000 dps)

    write_address(MPU6050, GYRO_CFG, 0x18, 1);
    
    // Set acc fsr (2g)
    write_address(MPU6050, ACCEL_CFG, 0x00, 1);
    
    // Set clock source (PLL) (sensors already enabled by default)
    write_address(MPU6050, PWR_MGMT_1, 0x01, 1);
    usleep(50000);
    
    // Configure fifo
    //configure_fifo(0x78);
    
    // Load firmware
    //if (load_firmware(DMP_CODE_SIZE, &dmp_memory, 0x0400, DMP_SAMPLE_RATE))
    //    return -1;
        
    // Setting orientation
    if (dmp_set_orientation(orientation_matrix_to_scalar(gyro_orientation)))
        return -1;
    
    // Enable features (enable all features to enable DMP calibration)
    dmp_enable_features(0x040 | 0x010 | DMP_FEATURE_SEND_CAL_GYRO | 0x020);
    
    // Setting update rate and Low-pass Filter
    set_fifo_rate(mpuRate);
    set_lpf(lpf);
    
    // Enable DMP
    if (set_dmp_state(1) != 0){
        //Serial.println("enable dmp failed");
        return 1;
    }
    
    
    //set_int_enable(1);
    return 1;
}

void write_address(unsigned char device_addr, unsigned char register_addr, unsigned char val, char stop){
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

void write_multiple_addresses(unsigned char device_addr, unsigned char start_addr, unsigned char val[], int amount_of_bytes){
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

    unsigned char *p = &val[0];
    if (write(file, val, amount_of_bytes) != amount_of_bytes) {
        printf("Can not write data. Address %d.\n", device_addr);
    }
    close(file);
}

void read_multiple_addresses(unsigned char device_addr, unsigned char start_addr, unsigned char val[], int amount_of_bytes){
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



int dmp_set_orientation(unsigned short orient){
    unsigned char gyro_regs[3], accel_regs[3];
    const unsigned char gyro_axes[3]  = {0x4C, 0xCD, 0x6C};
    const unsigned char accel_axes[3] = {0x0C, 0xC9, 0x2C};
    const unsigned char gyro_sign[3]  = {0x36, 0x56, 0x76};
    const unsigned char accel_sign[3] = {0x26, 0x46, 0x66};

    gyro_regs[0] = gyro_axes[orient & 3];
    gyro_regs[1] = gyro_axes[(orient >> 3) & 3];
    gyro_regs[2] = gyro_axes[(orient >> 6) & 3];
    accel_regs[0] = accel_axes[orient & 3];
    accel_regs[1] = accel_axes[(orient >> 3) & 3];
    accel_regs[2] = accel_axes[(orient >> 6) & 3];

    /* Chip-to-body, axes only. */
    if (write_mem(FCFG_1, 3, gyro_regs))
        return -1;
    if (write_mem(FCFG_2, 3, accel_regs))
        return -1;

    memcpy(gyro_regs, gyro_sign, 3);
    memcpy(accel_regs, accel_sign, 3);
    if (orient & 4) {
        gyro_regs[0] |= 1;
        accel_regs[0] |= 1;
    }
    if (orient & 0x20) {
        gyro_regs[1] |= 1;
        accel_regs[1] |= 1;
    }
    if (orient & 0x100) {
        gyro_regs[2] |= 1;
        accel_regs[2] |= 1;
    }

    /* Chip-to-body, sign only. */
    if (write_mem(FCFG_3, 3, gyro_regs))
        return -1;
    if (write_mem(FCFG_7, 3, accel_regs))
        return -1;
    return 0;
}

unsigned short orientation_matrix_to_scalar(signed char *mtx){
    unsigned short scalar;
    /*

       XYZ  010_001_000 Identity Matrix
       XZY  001_010_000
       YXZ  010_000_001
       YZX  000_010_001
       ZXY  001_000_010
       ZYX  000_001_010
     */
    scalar = row_to_scale(mtx);
    scalar |= row_to_scale(mtx + 3) << 3;
    scalar |= row_to_scale(mtx + 6) << 6;
    return scalar;
}

unsigned short row_to_scale(signed char *row){
    unsigned short b;

    if (row[0] > 0)
        b = 0;
    else if (row[0] < 0)
        b = 4;
    else if (row[1] > 0)
        b = 1;
    else if (row[1] < 0)
        b = 5;
    else if (row[2] > 0)
        b = 2;
    else if (row[2] < 0)
        b = 6;
    else
        b = 7;      // error
    return b;
}

int dmp_enable_features(unsigned short mask){
    unsigned char tmp[10];

    // TODO: All of these settings can probably be integrated into the default
    // DMP image.
    //
    // Set integration scale factor. 
    tmp[0] = (unsigned char)((GYRO_SF >> 24) & 0xFF);
    tmp[1] = (unsigned char)((GYRO_SF >> 16) & 0xFF);
    tmp[2] = (unsigned char)((GYRO_SF >> 8) & 0xFF);
    tmp[3] = (unsigned char)(GYRO_SF & 0xFF);
    write_mem(104, 4, tmp);
    for (char count = 0; count < 10; count++){
        tmp[count] = 0xA3;
    }
    // Send sensor data to the FIFO. 
    //tmp[0] = 0xA3;
    if (mask & 0x040) {
        tmp[1] = 0xC0;
        tmp[2] = 0xC8;
        tmp[3] = 0xC2;
    } else {
        tmp[1] = 0xA3;
        tmp[2] = 0xA3;
        tmp[3] = 0xA3;
    }
    if (mask & DMP_FEATURE_SEND_ANY_GYRO) {
        tmp[4] = 0xC4;
        tmp[5] = 0xCC;
        tmp[6] = 0xC6;
    } else {
        tmp[4] = 0xA3;
        tmp[5] = 0xA3;
        tmp[6] = 0xA3;
    }
    tmp[7] = 0xA3;
    tmp[8] = 0xA3;
    tmp[9] = 0xA3;
    write_mem(2727,10,tmp);

    // Send gesture data to the FIFO. 
    if (mask & (0x003))
        tmp[0] = 0x20;
    else{
        tmp[0] = 0xD8;
    }
    write_mem(2742,1,tmp);

    if (mask & 0x020)
        enable_gyro_cal(1);
    else
        enable_gyro_cal(0);

    if (mask & DMP_FEATURE_SEND_ANY_GYRO) {
        if (mask & DMP_FEATURE_SEND_CAL_GYRO) {
            tmp[0] = 0xB2;
            tmp[1] = 0x8B;
            tmp[2] = 0xB6;
            tmp[3] = 0x9B;
        } else {
            tmp[0] = 0xC0;
            tmp[1] = 0x80;
            tmp[2] = 0xC2;
            tmp[3] = 0x90;
        }
        write_mem(2722, 4, tmp);
    }

    if (mask & 0x004){
        
        enable_lp_quat(1);
    }else{
        enable_lp_quat(0);
    }
    if (mask & 0x010)
        enable_6x_lp_quat(1);
    else
        enable_6x_lp_quat(0);
    reset_fifo();

    return 0;
}

int write_mem(unsigned short mem_addr, unsigned short length,
                       unsigned char *data)
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
    write_multiple_addresses(MPU6050, BANK_SEL, tmp, 2);
    write_multiple_addresses(MPU6050, MEM_RW, data, length);
    return 0;
}

int read_mem(unsigned short mem_addr, unsigned short length,
unsigned char *data)
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
    write_multiple_addresses(MPU6050, BANK_SEL, tmp, 2);
    read_multiple_addresses(MPU6050, MEM_RW, data, length);
    return 0;
}

int enable_gyro_cal(unsigned char enable)
{
    if (enable) {
        unsigned char regs[9] = {0xb8, 0xaa, 0xb3, 0x8d, 0xb4, 0x98, 0x0d, 0x35, 0x5d};
        return write_mem(1208, 9, regs);
        } else {
        unsigned char regs[9] = {0xb8, 0xaa, 0xaa, 0xaa, 0xb0, 0x88, 0xc3, 0xc5, 0xc7};
        return write_mem(1208, 9, regs);
    }
}

int reset_fifo(void){
    unsigned char data;

    //if (!(chip_cfg.sensors))
    //return -1;

    data = 0;
    write_address(MPU6050, INT_ENABLE, data, 1);
    write_address(MPU6050, FIFO_EN, data, 1);
    write_address(MPU6050, USER_CTRL, data, 1);

    if (1) { //assume truth TODO fix
        data = 0x04 | 0x08;
        write_address(MPU6050, USER_CTRL, data, 1);
        usleep(50000);
        data = 0x80 | 0x40;
        if (1) //chip_cfg.sensors & 0x01
            data |= 0x20;
        write_address(MPU6050, USER_CTRL, data, 1);
        if (1) //chip_cfg.int_enable
            data = 0x02;
        else
            data = 0;
        write_address(MPU6050, INT_ENABLE, data, 1);
        data = 0;
        write_address(MPU6050, FIFO_EN, data, 1);
    } else {
        data = 0x04;
        write_address(MPU6050, USER_CTRL, data, 1);
        if (0) //chip_cfg.bypass_mode || !(chip_cfg.sensors & 0x01)
            data = 0x40;
        else
            data = 0x04 | 0x20;
        write_address(MPU6050, USER_CTRL, data, 1);
        usleep(50000);
        if (1) //chip_cfg.int_enable
            data = 0x01;
        else
            data = 0;
        write_address(MPU6050, INT_ENABLE, data, 1);
        write_address(MPU6050, FIFO_EN, 1, 1); //chip_cfg.fifo_enable
    }
    return 0;
}

int enable_lp_quat(unsigned char enable)
{
    unsigned char regs[4];
    if (enable) {
        regs[0] = 0xC0;
        regs[1] = 0xC2;
        regs[2] = 0xC4;
        regs[3] = 0xC6;
    }
    else
    memset(regs, 0x8B, 4);
    write_mem(2712, 4, regs);

    return reset_fifo();
}

int load_firmware(unsigned short length, const unsigned char *firmware,
                           unsigned short start_addr, unsigned short sample_rate)
{
    unsigned short ii;
    unsigned short this_write;
    int errCode;
    uint8_t *progBuffer;
    /* Must divide evenly into st->hw->bank_size to avoid bank crossings. */
    #define LOAD_CHUNK  (16)
    unsigned char cur[LOAD_CHUNK], tmp[2];

    //if (chip_cfg.dmp_loaded) //TODO maybe actually implement this
    /* DMP should only be loaded once. */
    //return -1;

    if (!firmware)
    return -2;
    
    progBuffer = (uint8_t *)malloc(LOAD_CHUNK);
    for (ii = 0; ii < length; ii += this_write) {
        this_write = min(LOAD_CHUNK, length - ii);
        
        for (int progIndex = 0; progIndex < this_write; progIndex++)
        progBuffer[progIndex] = firmware[ii + progIndex];
        
        if ((errCode = write_mem(ii, this_write, progBuffer))) {
            //Serial.print("firmware write failed: ");
            //Serial.println(errCode);
            printf("Firmware write failed\n");
            return -3;
        }
        
        if (read_mem(ii, this_write, cur))
        return -4;
        
        //if (memcmp(progBuffer, cur, this_write)) {
        //    printf("Firmware compare failed\n");
            //Serial.print("Firmware compare failed addr "); Serial.println(ii);
            /*for (int i = 0; i < 10; i++) {
                printf("%s\n", );
                Serial.print(progBuffer[i]);
                Serial.print(" ");
            }
            Serial.println();
            for (int i = 0; i < 10; i++) {
                Serial.print(cur[i]);
                Serial.print(" ");
            }
            Serial.println();*/
        //    return -5;
        //}
    }

    /* Set program start address. */
    tmp[0] = start_addr >> 8;
    tmp[1] = start_addr & 0xFF;
    write_multiple_addresses(MPU6050, 0x70, tmp, 2);

    //chip_cfg.dmp_loaded = 1;
    //chip_cfg.dmp_sample_rate = sample_rate;
    //Serial.println("Firmware loaded");
    free(progBuffer);
    return 0;
}

int enable_6x_lp_quat(unsigned char enable)
{
    unsigned char regs[4];
    if (enable) {
        regs[0] = 0x20;
        regs[1] = 0x28;
        regs[2] = 0x30;
        regs[3] = 0x38;
    } else
    memset(regs, 0xA3, 4);

    write_mem(2718, 4, regs);

    return reset_fifo();
}

int set_fifo_rate(unsigned short rate){
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
    if (write_mem(534, 2, tmp))
        return -1;
    if (write_mem(2753, 12, (unsigned char*)regs_end))
        return -1;
    return 0;
}

int set_lpf(unsigned short lpf)
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

    write_address(MPU6050, LPF, data, 1);
    return 0;
}

int set_dmp_state(unsigned char enable)
{
    unsigned char tmp;
    //if (chip_cfg.dmp_on == enable)
    //    return 0;

    if (enable) {
        //if (!chip_cfg.dmp_loaded)
        //    return -1;
        // Disable data ready interrupt. 
        set_int_enable(0);
        // Keep constant sample rate, FIFO rate controlled by DMP. 
        //set_sample_rate(chip_cfg.dmp_sample_rate);
        // Remove FIFO elements. 
        tmp = 0;
        write_address(MPU6050, 0x23, tmp, 1);
        //chip_cfg.dmp_on = 1;
        // Enable DMP interrupt. 
        set_int_enable(1);
        reset_fifo();
    } else {
        // Disable DMP interrupt. 
        set_int_enable(0);
        // Restore FIFO settings. 
        //tmp = chip_cfg.fifo_enable;
        write_address(MPU6050, 0x23, 1, 1);
        //chip_cfg.dmp_on = 0;
        reset_fifo();
    }
    return 0;
}

int set_int_enable(unsigned char enable){
    unsigned char tmp;

    if (1) { //chip_cfg.dmp_on
        if (enable)
            tmp = 0x02;
        else
            tmp = 0x00;
        write_address(MPU6050, INT_ENABLE, tmp, 1);
        //chip_cfg.int_enable = tmp;
    } else {
        if (0) //!chip_cfg.sensors
            return -1;
        if (enable && 1) // chip_cfg.int_enable
            return 0;
        if (enable)
            tmp = 0x01;
        else
            tmp = 0x00;
        write_address(MPU6050, INT_ENABLE, tmp, 1);
        //chip_cfg.int_enable = tmp;
    }
    return 0;
}

int set_sample_rate(unsigned short rate)
{
    unsigned char data;

    //if (!(chip_cfg.sensors))
    //    return -1;

    if (0) //chip_cfg.dmp_on
        return -1;
    else {
        if (rate < 4)
            rate = 4;
        else if (rate > 1000)
            rate = 1000;

        data = 1000 / rate - 1;
        write_address(MPU6050, RATE_DIV, data, 1);

        //chip_cfg.sample_rate = 1000 / (1 + data);
        return 0;
    }
}

unsigned char read_address(unsigned char device_addr, unsigned char register_addr, char stop){
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


char mpu_read(){
    short intStatus;
    unsigned char tmp[2];
    unsigned char errCode;
    
    
    // getting int status
    /*if (!chip_cfg.sensors)
        return false;
    if (!chip_cfg.dmp_on)
        return false;*/
        
    read_multiple_addresses(MPU6050, DMP_INT_STATUS, tmp, 2);
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
    
    return 0;
}

int mpu_read_fifo(){
    unsigned char fifo_data[32];
    int errCode;
    unsigned char more;

    // Get a packet
    unsigned char tmp[2];
    unsigned short fifo_count;
    read_multiple_addresses(MPU6050, FIFO_COUNT_H, tmp, 2);
    fifo_count = (tmp[0] << 8) | tmp[1];
    if (fifo_count < PACKET_LENGTH) {
        return -1;
    }
    if (fifo_count > (MAX_FIFO >> 1)) {
        // check overflow
        tmp[0] = read_address(MPU6050, INT_STATUS, 1);
        if (tmp[0] & 0x10) {
            reset_fifo();
            return -2;
        }
    }
    read_multiple_addresses(MPU6050, FIFO_RW, fifo_data, PACKET_LENGTH);

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

void writeBits(uint8_t DEV_ADD, uint8_t DATA_REGADD, uint8_t data, int length, int startBit) {
    int8_t temp = read_address(DEV_ADD, DATA_REGADD, 0);
    uint8_t bits = 1;
    uint8_t i = 0;

    while (i < length - 1) {
        bits = (bits << 1);
        ++bits;
        ++i;
    }

    temp &= ~(bits << startBit);

    temp |= (data << startBit);

    write_address(DEV_ADD, DATA_REGADD, temp, 0);

}

void writeBit(uint8_t DEV_ADD, uint8_t DATA_REGADD, uint8_t data, int bitNum) {
    int8_t temp = read_address(DEV_ADD, DATA_REGADD, 0);
    if (data == 0) {
        temp = temp & ~(1 << bitNum);
    } else if (data == 1) {
        temp = temp | (1 << bitNum);
    } else {
        printf("Value must be 0 or 1! --> Address %d.\n", DEV_ADD);
        exit(1);
    }

    write_address(DEV_ADD, DATA_REGADD, temp, 0);

}

void readByteBuffer(uint8_t DEV_ADD, uint8_t DATA_REGADD, uint8_t *data, uint8_t length) {

    int file;
    char *filename = "/dev/i2c-1";

    if ((file = open(filename, O_RDWR)) < 0) {
        printf("%s do not open. Address %d.\n", filename, DEV_ADD);
        exit(1);
    }

    if (ioctl(file, I2C_SLAVE, DEV_ADD) < 0) {
        printf("Can not join I2C Bus. Address %d.\n", DEV_ADD);
        exit(1);
    }

    uint8_t buffer[1];
    buffer[0] = DATA_REGADD;

    if (write(file, buffer, 1) != 1) {
        printf("Can not write data. Address %d.\n", DEV_ADD);
        exit(1);
    }

    if (read(file, data, length) != length) {
        printf("Can not read data. Address %d.\n", DEV_ADD);
        exit(1);
    }

    close(file);

}
