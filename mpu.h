#ifndef MPU_H
#define MPU_H

float dmpEuler[3];
uint8_t rawaccel;


int mpu_set_fifo_rate(unsigned short rate);
int mpu_set_lpf(unsigned short lpf);
int mpu_reset_fifo(void);
int mpu_read_fifo();
char mpu_read();
void mpu_calculate_angles_2();
void mpu_print_angles(float *vec);

#endif
