#ifndef DEFINES_H_
#define DEFINES_H_

#define MPU6050			(0x68) //0xD0 << 1
#define WHO_AM_I		(0x75)
#define RATE_DIV		(0x19)
#define LPF				(0x1A)
#define PROD_ID			(0x0C)
#define USER_CTRL		(0x6A)
#define FIFO_EN			(0x23)
#define GYRO_CFG		(0x1B)
#define ACCEL_CFG		(0x1C)
#define MOTION_THR		(0x1F)
#define MORION_DUR		(0x20)
#define FIFO_COUNT_H	(0x72)
#define FIFO_COUNT_L	(0x73)
#define FIFO_RW			(0x74)
#define RAW_GYRO		(0x43)
#define RAW_ACCEL		(0x3B)
#define TEMP			(0x41)
#define INT_ENABLE		(0x38)
#define DMP_INT_STATUS	(0x39)
#define INT_STATUS		(0x3A)
#define PWR_MGMT_1		(0x6B)
#define PWR_MGMT_2		(0x6C)
#define INT_PIN_CFG		(0x37)
#define MEM_RW			(0x6F)
#define ACCEL_OFFSET	(0x06)
#define I2C_MST			(0x24)
#define BANK_SEL		(0x6D)
#define MEM_START_ADDR	(0x6E)
#define PRGRM_START_H	(0x70)
#define RAW_COMPASS		(0x49)
#define YG_OFFS_TC		(0x01)
#define I2C_S0_ADDR		(0x25)
#define I2C_S0_REG		(0x26)
#define I2C_S0_CTRL		(0x27)
#define I2C_S1_ADDR		(0x28)
#define I2C_S1_REG		(0x29)
#define I2C_S1_CTRL		(0x2A)
#define I2C_S4_CTRL		(0x34)
#define I2C_S0_DO		(0x63)
#define I2C_S1_DO		(0x64)
#define I2C_DELAY_CTRL	(0x67)

#define M_PI   (3.14159265358979323846)

// these are magnetometer defines
#define AKM8975			(0x1C)
#define AKM_WHO_AM_I	(0x00)
#define	AKM_ST1			(0x02)
#define AKM_HXL			(0x03)
#define AKM_ST2			(0x09)
#define AKM_CTRL		(0x0A)
#define AKM_ASTC		(0x0C)
#define AKM_ASAX		(0x10)
#define AKM_ASAY		(0x11)
#define AKM_ASAZ		(0x12)
#define AKM_DATA_RDY	(0x01)
#define AKM_DA_OVERRUN	(0x02)
#define AKM_OVERFLOW	(0x80)
#define AKM_DA_ERROR	(0x40)
#define AKM_PWR_DOWN	(0x00)
#define AKM_SINGLE_MSR	(0x01)
#define AKM_FUSE_ROM	(0x0F)
#define AKM_SELF_TEST	(0x08)

//other defines
#define MAX_FIFO		1024
#define NUM_REG			118
#define BANK_SIZE		256
#define PACKET_LENGTH	28 //28
#define AK89xx_FSR		(9830)
#define MAX_PACKET_LNGT	(12)
#define ACCEL_HALF		(0)
#define DMP_CODE_SIZE   (3062)
#define DMP_SAMPLE_RATE (100)
#define GYRO_SF         (46850825LL * 200 / DMP_SAMPLE_RATE)
#define DMP_FEATURE_SEND_ANY_GYRO   (DMP_FEATURE_SEND_RAW_GYRO | \
									 DMP_FEATURE_SEND_CAL_GYRO)
#define DMP_FEATURE_SEND_RAW_GYRO   (0x080)
#define DMP_FEATURE_SEND_CAL_GYRO   (0x100)
#define FCFG_1			(1062)
#define FCFG_2			(1066)
#define FCFG_3			(1088)
#define FCFG_7			(1073)
#define QUATW			(0)
#define QUATX			(1)
#define QUATY			(2)
#define QUATZ			(3)
#define EULERX			(0)
#define EULERY			(1)
#define EULERZ			(2)
#define VEC3_X			(0)
#define VEC3_Y			(1)
#define VEC3_Z			(2)
#define RAD_TO_DEGREE	(180.0f / (float)M_PI)
#define min(a,b) ((a<b)?a:b)

struct chip_cfg_s {
    unsigned char sensors;
    unsigned short sample_rate;
	unsigned char fifo_enable;
    unsigned char lp_accel_mode;
    unsigned char int_motion_only;
    unsigned char active_low_int;
    unsigned char latched_int;
	unsigned char int_enable;
    unsigned char dmp_on;
    unsigned char dmp_loaded;
	unsigned char bypass_mode;
    unsigned short dmp_sample_rate;
};

#endif /* DEFINES_H_ */