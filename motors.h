#ifndef MOTOR_H
#define MOTOR_H

#include <stdio.h>
#include <stdint.h> 

#define FALSE   0
#define TRUE    1

#define COMMAND_LOW_LIMIT            0 
#define DRIVE_MOTOR_2_7_BIT          7
#define COMMAND_HIGH_LIMIT           17

#define CRC_MASK                     0b01111111

int mtr_motor_run(pthread_t *thread, mn_core_data *data);
uint8_t control_motors_sep ( uint8_t command1, uint8_t speed1, uint8_t command2, uint8_t speed2);
uint8_t motors_forward(uint8_t speed);
uint8_t motors_reverse(uint8_t speed);
static void send_command ( uint8_t command, uint8_t value, uint8_t address);



#endif
