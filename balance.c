#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "balance.h"

float prevgyro = 0;

void bal_balance(float* gyroangles, int motorspeed, int* newmotorspeed)
{
  float gyroangle = *gyroangles;
  gyroangle = gyroangle;
  printf("gyroangle: %f\n", gyroangle);
  float gyrospeed;

  if(gyroangle >= 1.1 || gyroangle <= -1.1){
    printf("TE HOGE WAARDE RETURNING\n");
    return;
  }

  int power;
  clock_t tMotorPosOK;
  if (prevgyro == 0) //drop first data in favor calculating speed
  {
    prevgyro = gyroangle;
    return;
  }
  
  gyrospeed = gyroangle - prevgyro;
  
  if (gyrospeed > 1.0) {
    return;
  }
  prevgyro = gyroangle;


    tMotorPosOK = clock();
    power = (gyrospeed + 91 * gyroangle)/0.5 + 0.1 * motorspeed; // power = (gyroSpeed + 8.0 * gyroAngle)/0.8 + 0.05 * motorPos + 0.1 * motorSpeed; removed motorspos

    if (abs(power) < 100)
      tMotorPosOK = clock();
    if (power > 100)
      *newmotorspeed = 126;
    else if (power <-100)
      *newmotorspeed = -126;
    else
      *newmotorspeed = power;
}