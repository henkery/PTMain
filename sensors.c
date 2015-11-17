//This is the sensors module of the PT control system.
#include <pthread.h>
#include "sensors.h"

int run;

void *sensorLoop(void* run)
{
	while (1) {

	}
	return 0;
}

int sensorRun(pthread_t *thread) {
	run = 1;
	pthread_create(thread, NULL, sensorLoop, &run);
	return 0;
}
