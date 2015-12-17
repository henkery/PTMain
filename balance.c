#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "balance.h"

float prevgyro = 0;

void bal_balance(float gyroangle, int motorspeed, int* newmotorspeed)
{
  float gyrospeed;

  int power;
  clock_t tMotorPosOK;
  if (prevgyro == 0) //drop first data in favor calculating speed
  {
    prevgyro = gyroangle;
    printf("setting prevgyro to %f\n", prevgyro);
    return;
  }
  
  gyrospeed = gyroangle - prevgyro;
  
  if (gyrospeed > 1.0) {
    printf("gyrospeed too fast!\n");
    return;
  }
  prevgyro = gyroangle;


  tMotorPosOK = clock();
     printf("generating power based on a gyrospeed of %f a gyroangle of %f and a motorspeed of %d\n", gyrospeed, gyroangle, motorspeed);
     power = (gyrospeed + 18.2 * gyroangle)/0.8 +0.1 * motorspeed; // power = (gyroSpeed + 8.0 * gyroAngle)/0.8 + 0.05 * motorPos + 0.1 * motorSpeed; removed motorspos
     printf("Setting power to %d\n", power);

     if (abs(power) < 100)
       tMotorPosOK = clock();
     if (power > 100)
       *newmotorspeed = 126;
     else if (power <-100)
       *newmotorspeed = -126;
     else
       *newmotorspeed = power;
    //TODO power motors 
     printf("set motors to %d\n", *newmotorspeed);

  //TODO stop motors
  //TODO inform system of critical failure (fell down)
}