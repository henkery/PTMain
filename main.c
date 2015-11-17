#include <stdio.h>
#include <pthread.h>
#include "sensors.h"

pthread_t th_sensor;
pthread_t th_motor;

int main(int argc, char const *argv[])
{
	sns_sensor_run(&th_sensor);
	while (1) {

	}
	return 0;
}
