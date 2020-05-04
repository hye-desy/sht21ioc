#ifndef I2C_H
#define I2C_H
#include <linux/i2c-dev.h>


#define	ERROR_I2C_OPEN		1
#define	ERROR_I2C_SETUP		2
#define	ERROR_I2C_READ		4
#define	ERROR_I2C_WRITE		8

typedef unsigned char     uint8;
typedef unsigned short    uint16;
typedef unsigned long     uint32;
typedef unsigned long     ulong;
typedef signed char       int8;
typedef signed short      int16;
typedef signed long       int32;


void I2C_Open(const char *dev);
void I2C_Close(void);
void I2C_Setup(uint32 mode,uint8 addr);
void I2C_Write1(uint8 d);
void I2C_Write2(uint8 d0,uint8 d1);
void I2C_Read(uint8 *data,uint8 length);
void I2C_PrintError(void);
uint8 I2C_GetError(void);
void I2C_ClearError(void);

#define	ERROR_SHT21_I2C			1
#define	ERROR_SHT21_CRC_TEMP		2
#define	ERROR_SHT21_CRC_HUMIDITY	4


uint8 SHT21_CalcCrc(uint8 *data,uint8 nbrOfBytes);
uint8 SHT21_Read(int16 *temp,uint8 *humidity);
void SHT21_PrintError(void);
uint8 SHT21_GetError(void);
void SHT21_ClearError(void);


#endif
