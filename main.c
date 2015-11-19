#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "header.h"
#include "sensors.h"
#include "motors.h"

pthread_t th_sensor;
pthread_t th_motor;
mn_core_data data;

int main(int argc, char const *argv[])
{
	int cl,rc;
	int sock = socket(AF_LOCAL, SOCK_STREAM, 0);  // initialize domain socket
	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, "/tmp/socket", sizeof(addr.sun_path)-1); //set socket address /tmp/socket
	bind(sock, (struct sockaddr*)&addr, sizeof(addr)); //bind
	data.run = 1; // Set all threads to run
	if (listen(sock, 5) == -1) { //listen to connections on the socket, exit if fails
	    return -1;
	}


	sns_sensor_run(&th_sensor, &data); //start other threads
	mtr_motor_run(&th_motor, &data);


	while (data.run) {
		if ( (cl = accept(sock, NULL, NULL)) == -1) { //wait for connections, fail on broken connection
	      perror("accept error");
	      continue;
	    }

	}
	return 0;
}
