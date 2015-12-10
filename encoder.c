//This is the motors module of the PT control system.
#include "encoder.h"
#include <stdio.h>
#include <stdint.h> 

int pulse = 0;    // how many turns have been completed
int oldpulse = 0;
float RPM = 0;
int resolution = 1000;
int increaseAmount = 1;    // how many points to fade the LED per turn
unsigned long currentTime;
const int pin_A = 3; // Green
const int pin_B = 4; // Yellow
const int pin_C = 5; // OUtput 
int C = 0;
int encoder_A;
int encoder_B;
int encoder_A_prev = 0;