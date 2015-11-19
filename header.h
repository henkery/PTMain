//This header defines some common variables in the project like datastructures
#ifndef HEADER_H
#define HEADER_H

struct st_core_data
{
	int run;
	pthread_mutex_t mu_data_lock;
};

typedef struct st_core_data mn_core_data ;
#endif