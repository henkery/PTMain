#ifndef SENSOR_H
#define SENSOR_H

int sns_sensor_run(pthread_t *thread, mn_core_data *data);
unsigned short row_to_scale(signed char *row);
unsigned short orientation_matrix_to_scalar(signed char *mtx);
int dmp_set_orientation(unsigned short orient);
char sns_mpu_init(int mpuRate, int lpf);
int dmp_enable_features(unsigned short mask);
int load_firmware(unsigned short length, const unsigned char *firmware,
                           unsigned short start_addr, unsigned short sample_rate);
int set_fifo_rate(unsigned short rate);
int set_lpf(unsigned short lpf);
int set_dmp_state(unsigned char enable);
int enable_gyro_cal(unsigned char enable);
int reset_fifo(void);
int enable_lp_quat(unsigned char enable);
int enable_6x_lp_quat(unsigned char enable);
int set_int_enable(unsigned char enable);
int set_sample_rate(unsigned short rate);
int mpu_read_fifo();
char mpu_read();
void writeBits(uint8_t DEV_ADD, uint8_t DATA_REGADD, uint8_t data, int length, int startBit);
char sns_mpu_newinit();
void writeBit(uint8_t DEV_ADD, uint8_t DATA_REGADD, uint8_t data, int bitNum);
void readByteBuffer(uint8_t DEV_ADD, uint8_t DATA_REGADD, uint8_t *data, uint8_t length);
void mpu_calculate_angles_2();
void mpu_print_angles(float *vec);

#endif
