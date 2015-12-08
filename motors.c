//This is the motors module of the PT control system.
#include <pthread.h>
#include "header.h"
#include <stdint.h>
#include <assert.h>
#include "motors.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>




int run;

void *mtr_motor_loop(void* vd_data)
{
	mn_core_data *data = (mn_core_data *)vd_data;
	while (data->run) {
		if (!control_motors_sep(0, 50, 0, 50, 128)) {
			printf("FUCK MATE\n");
		}
	}
	return 0;
}

int mtr_motor_run(pthread_t *thread, mn_core_data *data) {
	pthread_create(thread, NULL, mtr_motor_loop, data);
	return 0;
}

uint8_t control_motors_sep ( uint8_t command1, uint8_t speed1, \
                             uint8_t command2, uint8_t speed2, \
                             uint8_t address ) {
// = MOTOR_DRIVER_ADDRESS_1 ) { // If your compiler allows overloading, feel free to un-comment the assignment
// and move it immediately after the "address" variable
    if ( ( command1 < COMMAND_LOW_LIMIT || command1 > DRIVE_MOTOR_2_7_BIT ) || \
         ( command2 < COMMAND_LOW_LIMIT || command2 > DRIVE_MOTOR_2_7_BIT ) ) {

   /*     Set error code for invalid command
    *     Call a user error function to do whatever your application requires, such as:
    *         mSTOP_ALL_MOTORS;  */
		printf("FALSE\n");
        return FALSE;
    }

    else {
        send_command ( command1, speed1, address );
        send_command ( command2, speed2, address );
//      Set error code for no error
        return TRUE;
    }
}

/**********************************************************************************************
 * Function:        static void send_command ( uint8_t command, uint8_t value, uint8_t address )
 *
 * Pre-Condition:   None
 * Input:           Receives the command data from the driver functions
 * Output:          Sends the three commands plus their checksum to the serial port, and through
 *                      that to the Sabertooth Motor Controller
 * Side Effects:    None
 * Overview:        None
 * Notes:           Static helper function, for use only by driver functions in this file
 *********************************************************************************************/

    /* Helper Command, for internal driver use only
     * Defining it here, in the .c file, instead of in the .h file, to prevent
     * compiler warning about it being declared "static", but never defined
     * This is correct, since it is not for the user, only the user's functions */


/*********************************************************************************************/
static void send_command ( uint8_t command, uint8_t value, uint8_t address ) {
	int file;
	char *filename = "/dev/ttyO4";
	if((file = open(filename, O_RDWR))< 0)
	{
		printf("ERROR, device doesnt open\n");
	}
	uint8_t buffer[4];
	buffer[0] = address;
	buffer[1] = command;
	buffer[2] = value;
	buffer[3] =  ( address + command + value ) & CRC_MASK;
	if(write(file, buffer, 4) < 0){
		printf("ERROR, write didnt work\n");
	}
	close(file);
}
