/*******************************************************************************
|  File Name  :  usart.c
|  Description: 
|-------------------------------------------------------------------------------
| (c) This software is proprietary of xyz.
|     All rights are reserved by xyz.
|-------------------------------------------------------------------------------
| Initials      Name                   Company
| --------      --------------------   -----------------------------------------
| XXX           XXXX                   SHRIME
|-------------------------------------------------------------------------------
|               R E V I S I O N   H I S T O R Y
| Date           Version      Author     Description
| ------------   --------     -------    ------------------------------------
| 2018-05-26     01.00.00     xujialiang        Creation
*******************************************************************************/
#include "sys.h"
#include "usart.h"
#include "CRC.h"
/*******************************************************************************
|    Other Header File Inclusion
*******************************************************************************/

/*******************************************************************************
|    Compile Option or configuration Section  (??)
*******************************************************************************/
#if 1
#pragma import(__use_no_semihosting)    
              
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
 
_sys_exit(int x) 
{ 
	x = x; 
} 

int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0); 
    USART1->DR = (u8) ch;      
	return ch;
}
#endif
/*******************************************************************************
|    Local Macro Definition
*******************************************************************************/

/*************************adpater APIs******************************************/

/*******************************************************************************
|    Local Enum Definition
*******************************************************************************/

/*******************************************************************************
|    Local Typedef Definition
*******************************************************************************/
typedef struct
{
	u8 Rx_Count;					
	u8 Rev_Flag;					
	u8 RxBuf[BUFFSIZE];				
}USART_ReceiveTypeDef;

/*******************************************************************************
|    global variables Declaration
*******************************************************************************/
USART_ReceiveTypeDef USART_Receive;
CAN_MsgTypeDef CAN_Msg;
/*******************************************************************************
|    static local variables Declaration
*******************************************************************************/

/*******************************************************************************
|    Function Source Code
*******************************************************************************/
	
/*******************************************************************************
| FUNCTION NAME   :uart_init		
| CALLED BY 	  : 													 
| PRECONDITIONS   : 										  
| INPUT PARAMETERS: 														   
| RETURN VALUE	  :
| DESCRIPTION	  :
*******************************************************************************/
void uart_init(u32 bound){
    //
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);
	USART_DeInit(USART1);
	//USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

	//Usart1 NVIC ÅäÖÃ
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);	
  
	//
	USART_InitStructure.USART_BaudRate = bound;//
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	

    USART_Init(USART1, &USART_InitStructure); 
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);                    

}

/*******************************************************************************
| FUNCTION NAME   :USART_IRQHandler		
| CALLED BY 	  : 													 
| PRECONDITIONS   : 										  
| INPUT PARAMETERS: 														   
| RETURN VALUE	  :
| DESCRIPTION	  : 
*******************************************************************************/
void USART1_IRQHandler(void)    
{  
		if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
		{
			 if(USART_Receive.Rx_Count > 15)
			 {
				 USART_Receive.Rx_Count = 0;
			 }
			 USART1 -> DR;
			 USART_Receive.RxBuf[USART_Receive.Rx_Count++] = USART_ReceiveData(USART1);
		} 
} 

/*******************************************************************************
| FUNCTION NAME   :USART_Data_Analyse
| CALLED BY 	  : 													 
| PRECONDITIONS   : 										  
| INPUT PARAMETERS: 														   
| RETURN VALUE	  :
| DESCRIPTION	  : 
*******************************************************************************/
void USART_Data_Analyse(void)
{
	if((USART_Receive.RxBuf[14] + USART_Receive.RxBuf[15] == 0x82) && (USART_Receive.Rev_Flag == OFF)) //0XAA + 0xAA 
	{
		USART_Receive.Rev_Flag = ON;							
		switch (Get_Crc8(USART_Receive.RxBuf,2)) 				
		{
			case 0x2f:
					break; //IV
			case 0x42:
					USART_CMD_RelayCtrl();break;//RC
			case 0x36:
					break;//HP
			case 0x75:
					break;//TS
			case 0xD6:
					break;//CA
			default:
					USART_Data_Flush(USART_Receive.RxBuf);break;	
		}
					
	}
	else USART_Data_Flush(USART_Receive.RxBuf);
}

/*******************************************************************************
| FUNCTION NAME   :USART_Data_Cmd
| CALLED BY 	  : 													 
| PRECONDITIONS   : 										  
| INPUT PARAMETERS: 														   
| RETURN VALUE	  :
| DESCRIPTION	  :  
*******************************************************************************/
void USART_Data_Cmd(void)
{
	
}

/*******************************************************************************
| FUNCTION NAME   :USART_Data_Flush
| CALLED BY 	  : 													 
| PRECONDITIONS   : 										  
| INPUT PARAMETERS: 														   
| RETURN VALUE	  :
| DESCRIPTION	  :
*******************************************************************************/
void USART_Data_Flush(u8 Data[])
{
	u8 i = 0;
	for(i = 0;i < BUFFSIZE;i++)
	{
		Data[i] = 0x00;
	}
	USART_Receive.Rx_Count = 0x00;
	USART_Receive.Rev_Flag = OFF;
}

/*******************************************************************************
| FUNCTION NAME   :USART_CMD_RelayCtrl
| CALLED BY 	  : 													 
| PRECONDITIONS   : 										  
| INPUT PARAMETERS: 														   
| RETURN VALUE	  :
| DESCRIPTION	  :
*******************************************************************************/
void USART_CMD_RelayCtrl(void)
{
	u8 i,j;
	j =  4;
	for(i = 0;i < 8;i++)
	{
		CAN_Msg.RelayCtrl[i] = USART_Receive.RxBuf[j];
		j++;
	}
	USART_Data_Flush(USART_Receive.RxBuf);
	printf("RC020000000000AA");
}

