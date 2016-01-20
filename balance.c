#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "balance.h"
#include "mpudefines.h"

float gyroangleprev = 0;
int i = 0;

void bal_balance(float* gyroangles, int motorspeed, int* newmotorspeed, int* gyrospeeds) 
{
  float gyroangle = *gyroangles;

  int gyrospeed = *gyrospeeds;

  gyrospeed += 100; //offset gyroSpeed

  if(gyroangle >= 0.8 || gyroangle <= -0.8){
    return;
  }

  gyroangle = gyroangle * (180.0 / M_PI);

  gyroangle += 5.2;

  float verschil = gyroangleprev-gyroangle;

  if(abs(verschil)> 8){
    if(i > 10)
    {
        //printf("dropping line n %i\n", i);
        gyroangleprev = gyroangle;
    }
    //printf("gyroangle %f\n", gyroangle);
    i++;
    return;
  }
  i = 0;
  gyroangleprev = gyroangle;
  int power;
  clock_t tMotorPosOK;
    tMotorPosOK = clock();
    power = 2.57 * gyroangle + 0.210 * motorspeed;// power = (gyroSpeed + 8.0 * gyroAngle)/0.8 + 0.05 * motorPos + 0.1 * motorSpeed; removed motorspos
    //power = (2.65 * gyroangle - 2) + 0.103 * motorspeed;
    //printf("gyrospeed: %d \tgyroangle: %f \tmotorspeed: %d \tpower: %d\t\n", gyrospeed, gyroangle, motorspeed, power);
    if (abs(power) < 90)
      tMotorPosOK = clock();
    if (power > 100)
      *newmotorspeed = 126;
    else if (power <-90)
      *newmotorspeed = -126;
    else
      *newmotorspeed = power;
}