//This header defines some common variables in the project like datastructures
#ifndef HEADER_H
#define HEADER_H

struct st_core_data
{
	int run;
	char selectbuf;
	int buf_speed_1;
	float buf_angle_1;
	float buf_angle_2;
	int batstat;
};

typedef struct st_core_data mn_core_data ;
#endif