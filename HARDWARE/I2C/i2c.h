#ifndef _I2C_H_
#define _I2C_H_

#include "sys.h"
#include "delay.h"

#define I2C1_SCL	PBout(6)
#define I2C1_SDA	PBout(7)
#define read_SDA	PBin(7)

#define SDA1_IN()  		I2C_SetSDAMode(0x01)//{GPIOB->CRL	&= 0X0FFFFFFF; GPIOB->CRL |= (u32)8<<28;}
#define SDA1_OUT()		I2C_SetSDAMode(0x00)//{GPIOB->CRL	&= 0X0FFFFFFF; GPIOB->CRL |= (u32)3<<28;}



void I2C1_init(void);
void I2C_SetSDAMode(u8 mode);
void I2C_Start(void);
void I2C_Stop(void);
u8 I2C_WaitACK(void);
void I2C_ACK(void);
void I2C_NonACK(void);
void I2C_Send_Byte(u8 byte);
u8 I2C_Read_Byte(unsigned char ack);

#endif
