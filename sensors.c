//This is the sensors module of the PT control system.
#include <pthread.h>
#include "sensors.h"

int run;

void *sns_sensor_loop(void* vd_run)
{
	int run = (int *)vd_run;
	while (run*) {

	}
	return 0;
}

int sns_sensor_run(pthread_t *thread) {
	run = 1;
	pthread_create(thread, NULL, sns_sensor_loop, &run);
	return 0;
}
