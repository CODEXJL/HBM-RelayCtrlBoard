#include "i2c.h"

//4��i2c�ӿ�
void I2C1_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
		
	I2C1_SCL = 1;I2C1_SDA = 1;
}

void I2C_SetSDAMode(u8 mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	switch(mode)
	{
		case 0x00: //���ģʽ
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;break;
		case 0x01://����ģʽ
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;break;
		default:
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;break;
	}
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

//����I2C��ʼ�ź�
void I2C_Start(void)
{
	SDA1_OUT();//SDA�������
	I2C1_SDA = 1;
	I2C1_SCL = 1;
	delay_us(4);
	I2C1_SDA = 0;
	delay_us(4);
	I2C1_SCL =0;//ǯסI2C����,׼���շ�����
}

//I2Cֹͣ�ź�
void I2C_Stop(void)
{
	SDA1_OUT();
	I2C1_SCL = 0;
	I2C1_SDA = 0;
	delay_us(4);
	I2C1_SCL = 1;
	I2C1_SDA = 1;//��������ֹͣ�ź�
	delay_us(4);
}

//�ȴ���Ӧ�ź�
u8 I2C_WaitACK(void)
{
	u8 ErrTime = 0;
	SDA1_IN();//����Ϊ����
	I2C1_SDA = 1;
	delay_us(1);
	I2C1_SCL = 1;
	delay_us(1);
	while(read_SDA)
	{
		ErrTime++;
		if(ErrTime > 250)
		{
			I2C_Stop();
			return 1;
		}
	}
	I2C1_SCL = 0;
	return 0;
}

//����ACKӦ��
void I2C_ACK(void)
{
	I2C1_SCL = 0;
	SDA1_OUT();
	I2C1_SDA = 0;
	delay_us(2);
	I2C1_SCL = 1;
	delay_us(2);
	I2C1_SCL = 0;
}

//������Ӧ���ź�
void I2C_NonACK(void)
{
	I2C1_SCL = 0;
	SDA1_OUT();
	I2C1_SDA = 0;
	delay_us(2);
	I2C1_SCL = 1;
	delay_us(2);
	I2C1_SCL = 0;
}

//I2C����һ��byte
void I2C_Send_Byte(u8 byte)
{
	u8 t;
	SDA1_OUT();
	I2C1_SCL = 0;//����ʱ�ӿ�ʼ����
	for(t=0;t<8;t++)
	{
		I2C1_SDA = (byte&0x80)>>7;
		byte <<= 1;
		delay_us(2);
		I2C1_SCL = 1;
		delay_us(2);
		I2C1_SCL =0;
		delay_us(2);
	}
}

//��ȡһ���ֽ� ack = 1 ����ACK ack=0 ����nACK
u8 I2C_Read_Byte(unsigned char ack)
{
	unsigned char i,receive = 0;
	SDA1_IN();
	for(i=0;i<8;i++)
	{
		I2C1_SCL = 0;
		delay_us(2);
		I2C1_SCL = 1;
		receive <<= 1;
		if(read_SDA) receive++;
		delay_us(1);
	}
	if(!ack)
		I2C_NonACK();
	else
		I2C_ACK();
	return receive;
}

