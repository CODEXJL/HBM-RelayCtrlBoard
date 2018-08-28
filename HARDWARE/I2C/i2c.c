#include "i2c.h"

//4组i2c接口
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
		case 0x00: //输出模式
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;break;
		case 0x01://输入模式
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;break;
		default:
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;break;
	}
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

//产生I2C起始信号
void I2C_Start(void)
{
	SDA1_OUT();//SDA设置输出
	I2C1_SDA = 1;
	I2C1_SCL = 1;
	delay_us(4);
	I2C1_SDA = 0;
	delay_us(4);
	I2C1_SCL =0;//钳住I2C总线,准备收发数据
}

//I2C停止信号
void I2C_Stop(void)
{
	SDA1_OUT();
	I2C1_SCL = 0;
	I2C1_SDA = 0;
	delay_us(4);
	I2C1_SCL = 1;
	I2C1_SDA = 1;//发送总线停止信号
	delay_us(4);
}

//等待答应信号
u8 I2C_WaitACK(void)
{
	u8 ErrTime = 0;
	SDA1_IN();//设置为输入
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

//产生ACK应答
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

//不产生应答信号
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

//I2C发送一个byte
void I2C_Send_Byte(u8 byte)
{
	u8 t;
	SDA1_OUT();
	I2C1_SCL = 0;//拉低时钟开始传输
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

//读取一个字节 ack = 1 发送ACK ack=0 发送nACK
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

