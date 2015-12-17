#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "balance.h"

float prevgyro = 0;

void bal_balance(float* gyroangles, uint8_t motorspeed, int* newmotorspeed)
{
  float gyroangle = *gyroangles;
  float gyrospeed;

  int power;
  clock_t tMotorPosOK;
  if (prevgyro == 0) //drop first data in favor calculating speed
  {
    prevgyro = gyroangle;
    printf("returning\n");
    printf("gyroangle: %f\n", gyroangle);
    return;
  }
  gyrospeed = gyroangle - prevgyro;


  tMotorPosOK = clock();

     power = (gyrospeed + 8.0 * gyroangle * 36)/0.8 + 0.1 * motorspeed; // power = (gyroSpeed + 8.0 * gyroAngle)/0.8 + 0.05 * motorPos + 0.1 * motorSpeed; removed motorspos
     if((power) > 200 || power < -200 ){
      return;
     }
     else{

     if (abs(power) < 100)
       tMotorPosOK = clock();
     if (power > 100)
       *newmotorspeed = 126;
     else if (power <-100)
       *newmotorspeed = -126;
     else
       *newmotorspeed = power;
        //TODO power motors 
     printf("Setting power to %i\n", power);
     printf("Gyrospeed: %f GyroAngle: %f motorspeed: %i\n", gyrospeed, gyroangle, *newmotorspeed);
   }

  //TODO stop motors
  //TODO inform system of critical failure (fell down)
}