#ifndef __USART_H
#define __USART_H

#include "stdio.h"	
#include "sys.h" 

#define BUFFSIZE 16

typedef struct 
{
	u8 RelayCtrl[8];
	u8 hPowerCtrl[8];
	u8 IVCtrl[8];			
}CAN_MsgTypeDef;

extern CAN_MsgTypeDef CAN_Msg;

void uart_init(u32 bound);
void USART_Data_Analyse(void);
void USART_Data_Cmd(void);
void USART_Data_Flush(u8 Data[]);
void USART_CMD_RelayCtrl(void);
#endif


