#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include "balance.h"

int prevgyro = 0;

void bal_balance(int gyroangle, int motorspeed)
{
  int gyrospeed;

  int power;
  clock_t tMotorPosOK;
  if (prevgyro == 0) //drop first data in favor calculating speed
  {
    prevgyro = gyroangle;
    return;
  }
  gyrospeed = gyroangle - prevgyro;


  tMotorPosOK = clock();

  while(1) {
     power = (gyrospeed + 8.0 * gyroangle)/0.8 + 0.1 * motorspeed; // power = (gyroSpeed + 8.0 * gyroAngle)/0.8 + 0.05 * motorPos + 0.1 * motorSpeed; removed motorspos

     if (abs(power) < 100)
       tMotorPosOK = clock();

    //TODO power motors
     printf("set motors to %d\n", power);

     if ((clock() - tMotorPosOK) > 1500) {
       break;
     }
     usleep(200); //TODO consider if this delay is neccesary
  }
  //TODO stop motors
  //TODO inform system of critical failure (fell down)
}