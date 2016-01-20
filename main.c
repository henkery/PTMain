#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "header.h"
#include "sensors.h"
#include "motors.h"
#include "ipcproto.h"
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "encoder.h"
#include "gpio.h"

pthread_t th_sensor;
pthread_t th_motor;
mn_core_data data;
int sock;

void Segfault_Handler(int signo)
{
    printf("Program crashed: Trying to shutdown motors\n");
    motors_forward(0);
    remove("/tmp/socket");
    gpio_unexport(39);
    close(sock);
    exit(0);
}

void Termfault_Handler(int signo)
{
    printf("Program terminated: Trying to shutdown motors\n");
    motors_forward(0);
    remove("/tmp/socket");
    gpio_unexport(66);
    close(sock);
    exit(0);
}

int main(int argc, char const *argv[])
{
	signal(SIGINT ,Termfault_Handler);
	signal(SIGSEGV ,Segfault_Handler);
	int cl,rc;
	int yes = 1;
	sock = socket(AF_LOCAL, SOCK_STREAM, 0);  // initialize domain socket
	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, "/tmp/socket", sizeof(addr.sun_path)-1); //set socket address /tmp/socket
	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR, &yes, sizeof(int));
	if(bind(sock,(struct sockaddr*)&addr, sizeof(addr)) < 0){
		perror("Socket already in use at /tmp/socket: ");
	}
	chmod(addr.sun_path, 0775);
	data.run = 1; // Set all threads to run
	if (listen(sock, 5) == -1) { //listen to connections on the socket, exit if fails
		return -1;
	}

    //encoder_init();
	printf("SYSTEM CALLED\n");
	//mtr_motor_run(&th_motor, &data);

	char connected = 0;
	while (data.run) {
		if (connected) {
			char buf;
			int n;
			n = read(cl, &buf, 1); //read first byte
			if (n < 0) {
				close(cl); //failed read, drop connection
				cl = 0;
				connected = 0;
				continue;
			}
			switch(buf) {
				case 3:
				buf = PT_RESPONSE;
				n = write(cl, &buf, 1);
				if (n < 0) {
						close(cl); //failed write, drop connection
						cl = 0;
						connected = 0;
						continue;
					}
					else {
						char buff[8];
						int c = data.buf_speed_1;
						int d = data.batstat;
						memcpy(buff, &c, 4);
						memcpy(buff+4, &d, 4);
						n = write(cl, &buff, 8);
						if (n < 0) {
						close(cl); //failed write, drop connection
						cl = 0;
						connected = 0;
						continue;
					}
				}
				break;
				default:
					break;//no other situations
				}
			}
			else
			{
			if ( (cl = accept(sock, NULL, NULL)) == -1) { //wait for connections, fail on broken connection
				perror("accept error");
				cl = 0;
				continue;
			}
			else {
				char buf = 1;
				int n;
			n = read(cl, &buf, 1); //read first byte
			if (n < 0) {
				close(cl); //failed read, drop connection
				cl = 0;
				continue;
			}
			if (buf == PT_HANDSHAKE)
			{
				buf = PT_ACK;
				n = write(cl, &buf, 1);
				if (n < 0) {
					close(cl); //failed write, drop connection
					cl = 0;
					continue;
				}
				else {
					sns_sensor_run(&th_sensor, &data);
					connected = 1;
				}
			}
			else {
				close(cl); //first byte was not a handshake, drop connection
				cl = 0;
				continue;
			}
		}

	}
	
}
return 0;
}
