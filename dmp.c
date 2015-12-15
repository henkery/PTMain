#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "i2c_functions.h"
#include "mpu.h"
#include "mpudefines.h"
#include "dmp.h"

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
    if (i2c_write_mem(FCFG_1, 3, gyro_regs))
        return -1;
    if (i2c_write_mem(FCFG_2, 3, accel_regs))
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
    if (i2c_write_mem(FCFG_3, 3, gyro_regs))
        return -1;
    if (i2c_write_mem(FCFG_7, 3, accel_regs))
        return -1;
    return 0;
}

unsigned short dmp_orientation_matrix_to_scalar(signed char *mtx){
    unsigned short scalar;
    /*

       XYZ  010_001_000 Identity Matrix
       XZY  001_010_000
       YXZ  010_000_001
       YZX  000_010_001
       ZXY  001_000_010
       ZYX  000_001_010
     */
    scalar = dmp_row_to_scale(mtx);
    scalar |= dmp_row_to_scale(mtx + 3) << 3;
    scalar |= dmp_row_to_scale(mtx + 6) << 6;
    return scalar;
}

unsigned short dmp_row_to_scale(signed char *row){
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
    i2c_write_mem(104, 4, tmp);
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
    i2c_write_mem(2727,10,tmp);

    // Send gesture data to the FIFO. 
    if (mask & (0x003))
        tmp[0] = 0x20;
    else{
        tmp[0] = 0xD8;
    }
    i2c_write_mem(2742,1,tmp);

    if (mask & 0x020)
        dmp_enable_gyro_cal(1);
    else
        dmp_enable_gyro_cal(0);

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
        i2c_write_mem(2722, 4, tmp);
    }

    if (mask & 0x004){
        
        dmp_enable_lp_quat(1);
    }else{
        dmp_enable_lp_quat(0);
    }
    if (mask & 0x010)
        dmp_enable_6x_lp_quat(1);
    else
        dmp_enable_6x_lp_quat(0);
    mpu_reset_fifo();

    return 0;
}


int dmp_enable_gyro_cal(unsigned char enable)
{
    if (enable) {
        unsigned char regs[9] = {0xb8, 0xaa, 0xb3, 0x8d, 0xb4, 0x98, 0x0d, 0x35, 0x5d};
        return i2c_write_mem(1208, 9, regs);
        } else {
        unsigned char regs[9] = {0xb8, 0xaa, 0xaa, 0xaa, 0xb0, 0x88, 0xc3, 0xc5, 0xc7};
        return i2c_write_mem(1208, 9, regs);
    }
}

int dmp_enable_lp_quat(unsigned char enable)
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
    i2c_write_mem(2712, 4, regs);

    return mpu_reset_fifo();
}

int dmp_load_firmware(unsigned short length, const unsigned char *firmware,
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
        
        if ((errCode = i2c_write_mem(ii, this_write, progBuffer))) {
            //Serial.print("firmware write failed: ");
            //Serial.println(errCode);
            printf("Firmware write failed\n");
            return -3;
        }
        
        if (i2c_read_mem(ii, this_write, cur))
            return -4;
        
        if (memcmp(progBuffer, cur, this_write)) {
            printf("Firmware compare failed on %d\n", ii);
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
            return -5;
        }
    }

    /* Set program start address. */
    tmp[0] = start_addr >> 8;
    tmp[1] = start_addr & 0xFF;
    i2c_write_multiple_addresses(MPU6050, 0x70, tmp, 2);
    printf("Firmware loaded\n");

    //chip_cfg.dmp_loaded = 1;
    //chip_cfg.dmp_sample_rate = sample_rate;
    //Serial.println("Firmware loaded");
    free(progBuffer);
    return 0;
}

int dmp_enable_6x_lp_quat(unsigned char enable)
{
    unsigned char regs[4];
    if (enable) {
        regs[0] = 0x20;
        regs[1] = 0x28;
        regs[2] = 0x30;
        regs[3] = 0x38;
    } else
    memset(regs, 0xA3, 4);

    i2c_write_mem(2718, 4, regs);

    return mpu_reset_fifo();
}


int dmp_set_dmp_state(unsigned char enable)
{
    unsigned char tmp;
    //if (chip_cfg.dmp_on == enable)
    //    return 0;

    if (enable) {
        //if (!chip_cfg.dmp_loaded)
        //    return -1;
        // Disable data ready interrupt. 
        dmp_set_int_enable(0);
        // Keep constant sample rate, FIFO rate controlled by DMP. 
        //dmp_set_sample_rate(chip_cfg.dmp_sample_rate);
        // Remove FIFO elements. 
        tmp = 0;
        i2c_write_address(MPU6050, 0x23, tmp, 1);
        //chip_cfg.dmp_on = 1;
        // Enable DMP interrupt. 
        dmp_set_int_enable(1);
        mpu_reset_fifo();
    } else {
        // Disable DMP interrupt. 
        dmp_set_int_enable(0);
        // Restore FIFO settings. 
        //tmp = chip_cfg.fifo_enable;
        i2c_write_address(MPU6050, 0x23, 1, 1);
        //chip_cfg.dmp_on = 0;
        mpu_reset_fifo();
    }
    return 0;
}

int dmp_set_int_enable(unsigned char enable){
    unsigned char tmp;

    if (1) { //chip_cfg.dmp_on
        if (enable)
            tmp = 0x02;
        else
            tmp = 0x00;
        i2c_write_address(MPU6050, INT_ENABLE, tmp, 1);
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
        i2c_write_address(MPU6050, INT_ENABLE, tmp, 1);
        //chip_cfg.int_enable = tmp;
    }
    return 0;
}

int dmp_set_sample_rate(unsigned short rate)
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
        i2c_write_address(MPU6050, RATE_DIV, data, 1);

        //chip_cfg.sample_rate = 1000 / (1 + data);
        return 0;
    }
}