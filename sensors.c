//This is the sensors module of the PT control system.
#include <pthread.h>
#include "header.h"
#include "sensors.h"

int run;

void *sns_sensor_loop(void* vd_data)
{
	mn_core_data *data = (mn_core_data *)vd_data;
	while (data->run) {

	}
	return 0;
}

int sns_sensor_run(pthread_t *thread, mn_core_data *data) {
	pthread_create(thread, NULL, sns_sensor_loop, data);
	return 0;
}
