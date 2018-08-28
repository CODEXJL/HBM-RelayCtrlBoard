//** �̵������ư������� **//

#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "can.h"
#include "tca5939.h"
#include "i2c.h"

u8 msg[8] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};

 int main(void)
 {	 
		NVIC_Configuration();
		delay_init();	    	 	//��ʱ������ʼ��	  
		uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
		can_init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_Normal);//CAN�ӿڳ�ʼ��
	 	tca5939_Init();				//GPIO��չ�ӿڳ�ʼ��
		while(1)
		{
			//tca5939_SetIO(Sub4,OUTPUT_P0,P1_0,0x01);
			//CAN1_Send_Msg(msg,CANBUFSIZE,RelayCtrlBoard1_Id);
			delay_ms(10);
		}		
}


//***********************************END FILE***********************************//
