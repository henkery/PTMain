//This is the sensors module of the PT control system.
#include <stdint.h>
#include <pthread.h>
#include "header.h"
#include "sensors.h"
#include <stdio.h>
#include "gpio.h"
#include "mpudefines.h"
#include "dmpdefines.h"
#include "dmp.h"
#include "mpu.h"
#include "motors.h"

int run;
signed char gyro_orientation[9] = { 1, 0, 0,
                                    0, 1, 0,
                                    0, 0, 1};

void *sns_sensor_loop(void* vd_data)
{
    unsigned int but_pin = 86;
    gpio_export(but_pin);
    gpio_set_dir(but_pin, 0);
    gpio_set_edge(but_pin, "rising");
    mn_core_data *data = (mn_core_data *)vd_data;
    i2c_write_address(MPU6050, 0x6B, 0x00, 0);
    sns_mpu_init(100, 42);
    unsigned int but_pin_val = 0;
    data->selectbuf = 0;
    int motorspeed = 0;
    //sns_mpu_newinit();
    
    /*int16_t ax,ay,az,gx,gy,gz;*/
    while (data->run) {
        gpio_get_value(but_pin, &but_pin_val);
        if (but_pin_val)
        {
            printf("killswitch hit, aborting run\n");
            motors_forward(0);
            break;
        }
        if (mpu_read())
        {
            //printf("Sensor is not ready\n");
        }
        else
        {
            mpu_calculate_angles_2();
            //printf("%f, %f, %f,\n", dmpEuler[0], dmpEuler[1], dmpEuler[2]);
            float* s = &dmpEuler[0];
            bal_balance(s, motorspeed, &motorspeed);
            uint8_t speed = 0;
            data->buf_speed_1 = motorspeed;
            if (motorspeed>=0) {
                speed = motorspeed;
                motors_forward(speed);
            }
            else {
                speed = motorspeed*-1;
                motors_reverse(speed);
            }
            printf("MOTORS SET AT %d\n", motorspeed);
            /*if (!data->selectbuf)
            {
                data->selectbuf =1;
                data->buf_angle_1 = dmpEuler[1]; //not sure if this is the right angle
            }
            else
            {
                data->selectbuf = 0;
                data->buf_angle_2 = dmpEuler[1];
            }*/
            //mpu_print_angles(dmpEuler);

        }
    }
    return 0;
}

int sns_sensor_run(pthread_t *thread, mn_core_data *data) {
    pthread_create(thread, NULL, sns_sensor_loop, data);
    return 0;
}

char sns_mpu_init(int mpuRate, int lpf){
    int result;
    
    // Check validity of data
    /*if ((mpuRate > 1000) || (mpuRate < 1)){
        return -1;f
    }*/
    // Reset device 
    i2c_write_address(MPU6050, PWR_MGMT_1, 0x80, 1);
    usleep(100000);
    // Turn on device 
    i2c_write_address(MPU6050, PWR_MGMT_1, 0x00, 1);
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

    i2c_write_address(MPU6050, GYRO_CFG, 0x18, 1);
    
    // Set acc fsr (2g)
    i2c_write_address(MPU6050, ACCEL_CFG, 0x00, 1);
    
    // Set clock source (PLL) (sensors already enabled by default)
    i2c_write_address(MPU6050, PWR_MGMT_1, 0x01, 1);
    usleep(50000);
    
    // Configure fifo
    //configure_fifo(0x78);
    
    // Load firmware
    if (dmp_load_firmware(DMP_CODE_SIZE, &dmp_memory, 0x0400, DMP_SAMPLE_RATE))
        return -1;
        
    // Setting orientation
    if (dmp_set_orientation(dmp_orientation_matrix_to_scalar(gyro_orientation)))
        return -1;
    
    // Enable features (enable all features to enable DMP calibration)
    dmp_enable_features(0x040 | 0x010 | DMP_FEATURE_SEND_CAL_GYRO | 0x020);
    
    // Setting update rate and Low-pass Filter
    mpu_set_fifo_rate(mpuRate);
    mpu_set_lpf(lpf);
    
    // Enable DMP
    if (dmp_set_dmp_state(1) != 0){
        //Serial.println("enable dmp failed");
        return 1;
    }
    
    
    //dmp_set_int_enable(1);
    return 1;
}