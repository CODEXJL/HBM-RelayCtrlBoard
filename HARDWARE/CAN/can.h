#ifndef _CAN_H_
#define _CAN_H_

#include "stm32f10x_can.h"
#include "usart.h"

#define CANBUFSIZE  8
#define hPowerCtrl_Id 0x1
#define IV_Ctrl_Id 		0x2

#define RelayCtrlBoard1_Id 0x41  //当前继电器控制板ID

void can_init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);
u8 CAN1_Send_Msg(u8* msg,u8 len,u32 StdId);
u8 Can_Receive_Msg(u8 *buf);
void CAN_MsgProcess(void);
void Can_RelayCtrl(void);
void CAN_RelaySetALL(void);

#endif
