#include <stdio.h>
#include <pthread.h>
#include "sensors.h"

pthread_t threadSensor;
pthread_t threadMotor;

int main(int argc, char const *argv[])
{
	sensorRun(&threadSensor);
	while (1) {

	}
	return 0;
}
