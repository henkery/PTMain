#include <stdio.h>
#include "gpio.h"
#include <time.h>

const int pin_A = 38;
const int pin_B = 39;
int timeout;

int valueA = 0;
int valueB = 0;
int valueA_prev = 0;

int increaseAmount = 1;    // how many points to fade the LED per turn
int pulse = 0;    // how many turns have been completed
int oldpulse = 0;

clock_t currentTime;

float RPM = 0;
int resolution = 1000;

void encoder_init(){
    encode_setup();
    printf("SETUP DONE\n");
    while(1){
        int end = 0;
        clock_t start = clock();
        oldpulse = pulse;
        while(!end){
            usleep(50000);
            currentTime = clock();
            readRotary();
            if (currentTime == (10000 + start)){
                end = 1;
                printf("pulse - oldpulse: %d\n", pulse - oldpulse);
                RPM = ((pulse - oldpulse) * 60)/resolution;
                printf("RPM: %f\n", RPM);
            }
        }
    }
}

void encode_setup(){
    gpio_export(pin_A);
    gpio_export(pin_B);
    gpio_set_dir(pin_A, 0);
    gpio_set_dir(pin_B, 0);
    gpio_set_edge(pin_A, "falling");
    gpio_set_edge(pin_B, "falling");
}

void readRotary(){
    gpio_get_value(pin_A, &valueA);
    gpio_get_value(pin_B, &valueB);
    if((!valueA) && (valueA_prev)){
       // A has gone from high to low
        if(valueB) {
            // B is high so clockwise, increase pulse by increaseAmount
            pulse += increaseAmount;
        }
        else {
            // B is low so counter-clockwise, decrease pulse by increaseAmount
            pulse -= increaseAmount;

        }
    }
    else if((valueA) && (!valueA_prev)){
        // A has gone from low to high
        if(!valueB){
            // B is low so clockwise, increase pulse by increaseAmount
            pulse += increaseAmount;

        }
        else {
            // B is high so counter-clockwise, decrease pulse by increaseAmount
            pulse -= increaseAmount;

        }
    }
    valueA_prev = valueA;     // Store value of A for next time
}
