#include "gpio.h"

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

void setup()  {
    pinMode(P8_3, INPUT, 7, PULLDOWN);
    pinMode(P8_4, OUTPUT, 7, PULLDOWN);
    pinMode(P8_5, INPUT, 7, PULLDOWN);
    pinMode(P8_6, OUTPUT, 7, PULLDOWN);
}

void init_encoder()  {
    setup();
    countLoop(1);
}

void countLoop(float time){
    int end = 0;
    int start = gettimeofday();
    
    oldpulse = pulse; // Update amount of pulses
    
    while(!end){
        currentTime = gettimeofday();
        rotary();
        if(currentTime == ((time * 1000) + start)){
            end = 1;
            RPM = (((pulse - oldpulse) * (60 / time)) / resolution);
            printf("RPM:%d\n",RPM);
        }
    }
}

void rotary(){
    encoder_A = digitalRead(P8_3);    // Read encoder pins
    encoder_B = digitalRead(P8_5);
    if((!encoder_A) && (encoder_A_prev)){
        // A has gone from high to low
        if(encoder_B) {
            // B is high so clockwise, increase pulse by increaseAmount
            pulse += increaseAmount;
        }
        else {
            // B is low so counter-clockwise, decrease pulse by increaseAmount
            pulse -= increaseAmount;
        }
    }
    else if((encoder_A) && (!encoder_A_prev)){
        // A has gone from low to high
        if(!encoder_B){
            // B is low so clockwise, increase pulse by increaseAmount
            pulse += increaseAmount;
        }
        else {
            // B is high so counter-clockwise, decrease pulse by increaseAmount
            pulse -= increaseAmount;
        }
    }
    
    // Update Values
    encoder_A_prev = encoder_A;     // Store value of A for next time
}