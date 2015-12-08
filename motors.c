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
int fd;
char port[20] = "/dev/ttyO0";
speed_t baud = B9600;
int address = 128;
int run;

void *mtr_motor_loop(void* vd_data)
{
    char *filename = port;
    fd = open(filename, O_RDWR);
    mn_core_data *data = (mn_core_data *)vd_data;
    printf("Start succes, starting loop\n");
    while (data->run) {
        if (!motors_reverse(0)) {
        	break;
        }
    }
    return 0;
}

int mtr_motor_run(pthread_t *thread, mn_core_data *data) {
    pthread_create(thread, NULL, mtr_motor_loop, data);
    return 0;
}
uint8_t control_motors_sep ( uint8_t command1, uint8_t speed1,
uint8_t command2, uint8_t speed2) {
    if ( ( command1 < COMMAND_LOW_LIMIT || command1 > DRIVE_MOTOR_2_7_BIT ) ||
    ( command2 < COMMAND_LOW_LIMIT || command2 > DRIVE_MOTOR_2_7_BIT ) ) {
        printf("FALSEn");
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

static void send_command ( uint8_t command, uint8_t value, uint8_t address ) {
    int file;
    char *filename = port;
    struct termios attribs;
    speed_t speed;
    if((file = open(filename, O_RDWR))< 0)
    {
        printf("ERROR, device doesnt open\n");
        return;
    }
    if(tcgetattr(file, &attribs) < 0) {
        perror("stdin");
    }
    if(cfsetispeed(&attribs, B9600) < 0)
    {
        perror("invalid baud rate");
    }
    speed = cfgetispeed(&attribs);
    if(speed == 13){
        uint8_t buffer[4];
        buffer[0] = address;
        buffer[1] = command;
        buffer[2] = value;
        buffer[3] = ( address + command + value ) & CRC_MASK;
        if(write(file, buffer, 4) < 0){
            printf("ERROR, write didnt work\n");
        }
    }
    else{
        printf("Baudrate not correct\n");
    }
    close(file);
}