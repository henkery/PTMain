#include <stdio.h>
#include <pthread.h>
#include "header.h"
#include "sensors.h"
#include "motors.h"

pthread_t th_sensor;
pthread_t th_motor;
mn_core_data data;

int main(int argc, char const *argv[])
{
	data.run = 1; // Set all threads to run
	sns_sensor_run(&th_sensor, &data);
	mtr_motor_run(&th_motor, &data);
	while (data.run) {
	}
	return 0;
}
