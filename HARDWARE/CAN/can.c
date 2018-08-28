#include "can.h"
#include "tca5939.h"
#include "delay.h"

CanRxMsg CanReceiveMsg;
static volatile u8 CAN_ReceiveFlag;
u8 ACK[CANBUFSIZE] = {RelayCtrlBoard1_Id,0x30,0x30,0x30,0x30,0x30,0x30,0x30};

/*******************************************************************************
| FUNCTION NAME   :can_init		
| CALLED BY 	  	:
| PRECONDITIONS   : 										  
| INPUT PARAMETERS: 														   
| RETURN VALUE	  :
| DESCRIPTION	  	:CAN接口初始化
*******************************************************************************/
void can_init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	CAN_InitTypeDef CAN_InitStructure;
	CAN_FilterInitTypeDef  	CAN_FilterInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,ENABLE);
	//CAN_RX 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	//CAM_TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	 
	CAN_InitStructure.CAN_TTCM=DISABLE;
	CAN_InitStructure.CAN_ABOM=ENABLE;  
	CAN_InitStructure.CAN_AWUM=DISABLE;
	CAN_InitStructure.CAN_NART=ENABLE;
	CAN_InitStructure.CAN_RFLM=DISABLE;
	CAN_InitStructure.CAN_TXFP=DISABLE;	
	CAN_InitStructure.CAN_Mode= mode;	
	
	CAN_InitStructure.CAN_SJW=tsjw;	
	CAN_InitStructure.CAN_BS1=tbs1; 
	CAN_InitStructure.CAN_BS2=tbs2;
	CAN_InitStructure.CAN_Prescaler=brp; 
	CAN_Init(CAN1, &CAN_InitStructure);   
		
	CAN_FilterInitStructure.CAN_FilterNumber=0;	
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; 
	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;	
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);			

	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);// 
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}	

/*******************************************************************************
| FUNCTION NAME   :USB_LP_CAN1_RX0_IRQHandler	
| CALLED BY 	  	: 													 
| PRECONDITIONS   : 										  
| INPUT PARAMETERS: 														   
| RETURN VALUE	  :
| DESCRIPTION	  	:can
*******************************************************************************/
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	if( (CAN_MessagePending(CAN1,CAN_FIFO0)!=0) && (CAN_ReceiveFlag == OFF))
	{
		CAN_ReceiveFlag = ON;//
		CAN_Receive(CAN1,CAN_FIFO0,&CanReceiveMsg);
		switch(CanReceiveMsg.StdId)//
		{
			case RelayCtrlBoard1_Id:
						CAN_MsgProcess();break;
			default:
				break;
		}
		CAN_ReceiveFlag = OFF;
	}
}

/*******************************************************************************
| FUNCTION NAME   :CAN_MsgProcess
| CALLED BY 	  	: 													 
| PRECONDITIONS   : 										  
| INPUT PARAMETERS: 														   
| RETURN VALUE	  :
| DESCRIPTION	  	:can报文解析
*******************************************************************************/
void CAN_MsgProcess(void)
{
	switch(CanReceiveMsg.Data[0])//
	{
		case 0x31://CAN应答
				CAN1_Send_Msg(ACK,CANBUFSIZE,RelayCtrlBoard1_Id);break;
		case 0x32://继电器控制
				Can_RelayCtrl();break;
		case 0x33:
				CAN_RelaySetALL();break;
		default:break;
	}
	
	
}

/*******************************************************************************
| FUNCTION NAME   :CAN1_Send_Msg
| CALLED BY 	  	: 													 
| PRECONDITIONS   : 										  
| INPUT PARAMETERS: 														   
| RETURN VALUE	  :
| DESCRIPTION	  	:can
*******************************************************************************/
u8 CAN1_Send_Msg(u8* msg,u8 len,u32 StdId)
{	
  u8 mbox;
  u16 i=0;
  CanTxMsg TxMessage;
  TxMessage.StdId=StdId;	 
  TxMessage.ExtId=0x00;	 
  TxMessage.IDE=CAN_Id_Standard;		 
  TxMessage.RTR=CAN_RTR_DATA;		
  TxMessage.DLC=len;							
  for(i=0;i<len;i++)
  TxMessage.Data[i]=msg[i];				        
  mbox= CAN_Transmit(CAN1, &TxMessage);   
  i=0;
  while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//µÈ´ý·¢ËÍ½áÊø
  if(i>=0XFFF)return 1;
  return 0;		
}

/*******************************************************************************
| FUNCTION NAME   :Can_Receive_Msg
| CALLED BY 	  	: 													 
| PRECONDITIONS   : 										  
| INPUT PARAMETERS: 														   
| RETURN VALUE	  :
| DESCRIPTION	  	:
*******************************************************************************/
u8 Can_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
    if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)return 0;		
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
    for(i=0;i<8;i++)
    buf[i]=RxMessage.Data[i];  
	return RxMessage.DLC;	
}

/*******************************************************************************
| FUNCTION NAME   :Can_RelayCtrl
| CALLED BY 	  	: 													 
| PRECONDITIONS   : 										  
| INPUT PARAMETERS: 														   
| RETURN VALUE	  :
| DESCRIPTION	  	:继电器控制
*******************************************************************************/
void Can_RelayCtrl(void)
{
	u8 subId,portId,iO_Id,status;
	
	/*CAN报文解析*/
	/*芯片ID解析*/
	switch(CanReceiveMsg.Data[4])
	{
		case 0x31:	
							subId = Sub1;break;
		case 0x32:
							subId = Sub2;break;
		case 0x33:
							subId = Sub3;break;
		case 0x34:
							subId = Sub4;break;
		default:
							break;
	}
	/*芯片端口解析*/
	switch(CanReceiveMsg.Data[5])
	{
		case 0x30:	
							portId = OUTPUT_P0;break;
		case 0x31:
							portId = OUTPUT_P1;break;

		default:
							break;
	}
	/*IO编号解析*/
	switch(CanReceiveMsg.Data[6])
	{
		case 0x30:
				iO_Id = IO_0;break;
		case 0x31:
				iO_Id = IO_1;break;
		case 0x32:
				iO_Id = IO_2;break;
		case 0x33:
				iO_Id = IO_3;break;
		case 0x34:
				iO_Id = IO_4;break;
		case 0x35:
				iO_Id = IO_5;break;
		case 0x36:
				iO_Id = IO_6;break;
		case 0x37:
				iO_Id = IO_7;break;
		default:break;
	}
	/*IO 开关状态解析*/
	status = CanReceiveMsg.Data[7]-0x30;
	/*将报文解析后发给对应的I2C扩展IO*/
	tca5939_SetIO(subId,portId,iO_Id,status);
	//tca5939_SetIO(Sub4,OUTPUT_P0,P1_0,0x01);
	/*发送完后发出应答*/
	CAN1_Send_Msg(ACK,CANBUFSIZE,RelayCtrlBoard1_Id);
}

void CAN_RelaySetALL(void)
{
	u8 status;
	status = CanReceiveMsg.Data[7]-0x30;
	tca5939_SetIO(Sub1,OUTPUT_P0,0xff,status);
	tca5939_SetIO(Sub1,OUTPUT_P1,0xff,status);
	//delay_ms(2);
	tca5939_SetIO(Sub2,OUTPUT_P0,0xff,status);
	tca5939_SetIO(Sub2,OUTPUT_P1,0xff,status);
	//delay_ms(2);
	tca5939_SetIO(Sub3,OUTPUT_P0,0xff,status);
	tca5939_SetIO(Sub3,OUTPUT_P1,0xff,status);
	//delay_ms(2);
	tca5939_SetIO(Sub3,OUTPUT_P0,0xff,status);
	tca5939_SetIO(Sub3,OUTPUT_P1,0xff,status);
	
	CAN1_Send_Msg(ACK,CANBUFSIZE,RelayCtrlBoard1_Id);
}

//***********************************END FILE***********************************//
