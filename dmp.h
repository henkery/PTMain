#ifndef DMP_H
#define DMP_H

unsigned short dmp_row_to_scale(signed char *row);
unsigned short dmp_orientation_matrix_to_scalar(signed char *mtx);
int dmp_set_orientation(unsigned short orient);
int dmp_enable_features(unsigned short mask);
int dmp_load_firmware(unsigned short length, const unsigned char *firmware,
                           unsigned short start_addr, unsigned short sample_rate);
int dmp_set_dmp_state(unsigned char enable);
int dmp_enable_gyro_cal(unsigned char enable);
int dmp_enable_lp_quat(unsigned char enable);
int dmp_enable_6x_lp_quat(unsigned char enable);
int dmp_set_int_enable(unsigned char enable);
int dmp_set_sample_rate(unsigned short rate);

#endif
