#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "balance.h"
#include "mpudefines.h"


void bal_balance(float* gyroangles, int motorspeed, int* newmotorspeed, int* gyrospeeds) 
{
  float gyroangle = *gyroangles;
  int gyrospeed = *gyrospeeds;
  gyrospeed += 21.8;

  if(gyroangle >= 1.1 || gyroangle <= -1.1){
    return;
  }

  gyroangle = gyroangle * (180.0 / M_PI);

  int power;
  clock_t tMotorPosOK;

    tMotorPosOK = clock();
    power = 0.3 * gyrospeed + 5.3 * gyroangle + 0.1 * motorspeed; // power = (gyroSpeed + 8.0 * gyroAngle)/0.8 + 0.05 * motorPos + 0.1 * motorSpeed; removed motorspos
      printf("gyrospeed: %d gyroangle: %f motorspeed: %d power: %d\n", gyrospeed, gyroangle, motorspeed, power);
    if (abs(power) < 100)
      tMotorPosOK = clock();
    if (power > 100)
      *newmotorspeed = 126;
    else if (power <-100)
      *newmotorspeed = -126;
    else
      *newmotorspeed = power;
}