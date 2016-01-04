#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "balance.h"


void bal_balance(float* gyroangles, int motorspeed, int* newmotorspeed, unsigned char* gyrospeeds)
{
  float gyroangle = *gyroangles;
  unsigned char gyrospeed = *gyrospeeds;

  if(gyroangle >= 1.1 || gyroangle <= -1.1){
    printf("TE HOGE WAARDE RETURNING\n");
    return;
  }

  int power;
  clock_t tMotorPosOK;

  tMotorPosOK = clock();
     power = (gyrospeed + 91* gyroangle)/0.4 + 0.1 * motorspeed; // power = (gyroSpeed + 8.0 * gyroAngle)/0.8 + 0.05 * motorPos + 0.1 * motorSpeed; removed motorspos

     if (abs(power) < 100)
       tMotorPosOK = clock();
     if (power > 100)
       *newmotorspeed = 126;
     else if (power <-100)
       *newmotorspeed = -126;
     else
       *newmotorspeed = power;
    //TODO power motors

  //TODO stop motors
  //TODO inform system of critical failure (fell down)
}