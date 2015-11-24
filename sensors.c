//This is the sensors module of the PT control system.
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include "header.h"
#include "sensors.h"

int run;

void *sns_sensor_loop(void* vd_data)
{
    mn_core_data *data = (mn_core_data *)vd_data;
    int file;
    char *filename = "/dev/i2c-1";
    if ((file = open(filename, O_RDWR)) < 0) {
        /* ERROR HANDLING: you can check errno to see what went wrong */
        //perror("Failed to open the i2c bus");
        return 1;
    }
    int addr = 0xD0;          // The I2C address of the MPU
    if (ioctl(file, I2C_SLAVE, addr) < 0) {
        //printf("Failed to acquire bus access and/or talk to slave.\n");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        //exit(1);
        return 1;
    }


    while (data->run) {

    }
    return 0;
}

int sns_sensor_run(pthread_t *thread, mn_core_data *data) {
    pthread_create(thread, NULL, sns_sensor_loop, data);
    return 0;
}
