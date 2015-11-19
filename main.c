#include <stdio.h>
#include <pthread.h>
#include "header.h"
#include "sensors.h"

pthread_t th_sensor;
pthread_t th_motor;
mn_core_data data;

int main(int argc, char const *argv[])
{
	data.run = 1; // Set all threads to run
	sns_sensor_run(&th_sensor, &data);
	while (data.run) {
	}
	return 0;
}
