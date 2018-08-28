#ifndef _TCA5929_H_
#define _TCA5939_H_

#include "sys.h"

#define SUB_NUM 4			//I2C总线挂载芯片数量

#define RST1 PBout(15)
#define RST2 PBout(14)
#define RST3 PBout(13)
#define RST4 PBout(12)

//芯片寄存器地址
#define OUTPUT_P0 0x02
#define OUTPUT_P1 0x03
#define CONFIG_P0 0x06
#define CONFIG_P1 0x07

//IO编号
#define IO_0	0x01
#define IO_1 	0x02
#define IO_2 	0x04
#define IO_3 	0x08
#define IO_4 	0x10
#define IO_5 	0x20
#define IO_6 	0x40
#define IO_7 	0x80

//芯片ID由硬件地址A0 A1决定
extern enum{
	Sub1 = 0xE8,
	Sub2 = 0xEA,
	Sub3 = 0xEC,
	Sub4 = 0xEE,
}SubId_EnumTypeDef;


void tca5939_Init(void);
void tca5939_Cmd(u8 subid,u8 reg_Addr,u8 reg_Data);
void tca5939_Reset(u8 subid);
void tca5939_ResetIO(u8 subid);
void tca5939_ConfigOutput(u8 subid);
void tca5939_SetIO(u8 subid,u8 port,u8 pin,u8 status);
	
#endif
