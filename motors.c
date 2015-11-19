//This is the motors module of the PT control system.
#include <pthread.h>
#include "header.h"
#include "motors.h"

int run;

void *mtr_motor_loop(void* vd_data)
{
	mn_core_data *data = (mn_core_data *)vd_data;
	while (data->run) {

	}
	return 0;
}

int mtr_motor_run(pthread_t *thread, mn_core_data *data) {
	pthread_create(thread, NULL, mtr_motor_loop, data);
	return 0;
}
