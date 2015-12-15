//This is the motors module of the PT control system.
#include <pthread.h>
#include "header.h"
#include <stdint.h>
#include <assert.h>
#include "motors.h"
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "balance.h"

int fd;
char port[20] = "/dev/ttyO0";
speed_t baud = B9600;
int address = 128;
int run;

void *mtr_motor_loop(void* vd_data)
{
    char *filename = port;
    uint8_t motorspeed = 0;
    fd = open(filename, O_RDWR);
    mn_core_data *data = (mn_core_data *)vd_data;
    printf("Start succes, starting loop\n");
    while (data->run) {
        if (!data->selectbuf) //always select a different buffer
            {
            	bal_balance(data->buf_angle_2, motorspeed, &motorspeed);
            }
            else
            {
                bal_balance(data->buf_angle_1, motorspeed, &motorspeed);
            }
    if (motorspeed>=0)
        motors_forward(motorspeed);
    else
        motors_reverse(motorspeed*-1);
    return 0;
    }
}

int mtr_motor_run(pthread_t *thread, mn_core_data *data) {
    pthread_create(thread, NULL, mtr_motor_loop, data);
    return 0;
}

uint8_t control_motors_sep ( uint8_t command1, uint8_t speed1, uint8_t command2, uint8_t speed2) {
    if ( ( command1 < COMMAND_LOW_LIMIT || command1 > DRIVE_MOTOR_2_7_BIT ) ||
    ( command2 < COMMAND_LOW_LIMIT || command2 > DRIVE_MOTOR_2_7_BIT ) ) {
        return FALSE;
    }
    else {
        send_command ( command1, speed1, address );
        send_command ( command2, speed2, address );
        return TRUE;
    }
}


uint8_t motors_forward(uint8_t speed) {
        send_command ( 0, speed, address );
        send_command ( 4, speed, address );
        return TRUE;
}

uint8_t motors_reverse(uint8_t speed) {
        send_command ( 1, speed, address );
        send_command ( 5, speed, address );
        return TRUE;
}

/*
 * Function:  send_command 
 * --------------------
 * Send commands trough the port to the motor driver: *
 * uint8_t command: Command based on Individual Motor Commands:
 *                          0:  Drive Forward      Motor 1
 *                          1:  Drive Reverse      Motor 1
 *                          4:  Drive Forward      Motor 2
 *                          5:  Drive Reverse      Motor 2
 *                          6:  Drive 7-Bit        Motor 1
 *                          7:  Drive 7-Bit        Motor 2
 *
 * uint8_t value: value is the speed/data/number you want to send
 *
 * uint8_t address: is the address used for the driver probally 128 in our case
 *
 */
static void send_command ( uint8_t command, uint8_t value, uint8_t address ) {
    int file;
    char *filename = port;
    struct termios attribs;
    speed_t speed;
    if((file = open(filename, O_RDWR))< 0)
    {
		perror("Error [3]:");
        return;
    }
    if(tcgetattr(file, &attribs) < 0) {
		perror("Error [3]:");
    }
    if(cfsetispeed(&attribs, B9600) < 0)
    {
        perror("Error [3]:");
    }
    speed = cfgetispeed(&attribs);
    if(speed == 13){
        uint8_t buffer[4];
        buffer[0] = address;
        buffer[1] = command;
        buffer[2] = value;
        buffer[3] = ( address + command + value ) & CRC_MASK;
        if(write(file, buffer, 4) < 0){
            perror("Error [1]:");
        }
    }
    else{
        perror("Error [2]:");
    }
    close(file);
}