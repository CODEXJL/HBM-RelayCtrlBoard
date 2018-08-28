/*******************************************************************************
|  File Name  :  tca5939.c
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
| 2018-07-10     01.00.00     xujialiang        Creation
*******************************************************************************/
#include "tca5939.h"

/*******************************************************************************
|    Other Header File Inclusion
*******************************************************************************/
#include "i2c.h"
#include "myiic.h"
#include "delay.h"
/*******************************************************************************
|    Compile Option or configuration Section  (??)
*******************************************************************************/

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

/*******************************************************************************
|    global variables Declaration
*******************************************************************************/

/*******************************************************************************
|    static local variables Declaration
*******************************************************************************/

/*******************************************************************************
|    Function Source Code
*******************************************************************************/

/*******************************************************************************
| FUNCTION NAME   :tca5939_Init	
| CALLED BY 	  	: 													 
| PRECONDITIONS   : 										  
| INPUT PARAMETERS: 														   
| RETURN VALUE	  :
| DESCRIPTION	  	:I2C-GPIO芯片初始化	
*******************************************************************************/
void tca5939_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	//RST引脚4个
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	//默认拉高reset 低电平有效
	GPIO_SetBits(GPIOB, GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
	
	I2C1_init();//初始化i2c接口
	
	#if SUB_NUM == 4
			tca5939_ResetIO(Sub1);
			tca5939_ResetIO(Sub2);
			tca5939_ResetIO(Sub3);
			tca5939_ResetIO(Sub4);
			tca5939_ConfigOutput(Sub1);
			tca5939_ConfigOutput(Sub2);
			tca5939_ConfigOutput(Sub3);
			tca5939_ConfigOutput(Sub4);
	#elif SUB_NUM == 3
			tca5939_ResetIO(Sub1);
			tca5939_ResetIO(Sub2);
			tca5939_ResetIO(Sub3);		
			tca5939_ConfigOutput(Sub1);
			tca5939_ConfigOutput(Sub2);
			tca5939_ConfigOutput(Sub3);
	#elif SUB_NUM == 2
			tca5939_ResetIO(Sub1);
			tca5939_ResetIO(Sub2);
			tca5939_ConfigOutput(Sub1);
			tca5939_ConfigOutput(Sub2);
	#elif SUB_NUM == 1
			tca5939_ResetIO(Sub1);
			tca5939_ConfigOutput(Sub1);
	#endif
}

/*******************************************************************************
| FUNCTION NAME   :tca5939_Cmd
| CALLED BY 	  	: 													 
| PRECONDITIONS   : 										  
| INPUT PARAMETERS:													   
| RETURN VALUE	  :
| DESCRIPTION	  	:芯片操作
*******************************************************************************/
void tca5939_Cmd(u8 subid,u8 reg_Addr,u8 reg_Data)
{
	I2C_Start();
	I2C_Send_Byte(subid);//send addr
	I2C_WaitACK();
	I2C_Send_Byte(reg_Addr);//send cmd reg
	I2C_WaitACK();
	I2C_Send_Byte(reg_Data);//send data
	I2C_WaitACK();
	I2C_Stop();
}

/*******************************************************************************
| FUNCTION NAME   :tca5939_Reset
| CALLED BY 	  	: 													 
| PRECONDITIONS   : 										  
| INPUT PARAMETERS: 														   
| RETURN VALUE	  :
| DESCRIPTION	  	:芯片内部寄存器复位
*******************************************************************************/
void tca5939_Reset(u8 subid)
{
	switch(subid)
	{
		case Sub1:
						RST1 = 0;break;
		case Sub2:
						RST2 = 0;break;
		case Sub3:
						RST3 = 0;break;
		case Sub4:
						RST4 = 0;break;
		default:
				break;
	}
}

/*******************************************************************************
| FUNCTION NAME   :tca5939_Reset
| CALLED BY 	  	: 													 
| PRECONDITIONS   : 										  
| INPUT PARAMETERS: 														   
| RETURN VALUE	  :
| DESCRIPTION	  	:配置输出寄存器为0
*******************************************************************************/
void tca5939_ConfigOutput(u8 subid)
{
	tca5939_Cmd(subid,CONFIG_P0,0x00);
	tca5939_Cmd(subid,CONFIG_P1,0x00);
}

/*******************************************************************************
| FUNCTION NAME   :tca5939_ResetIO
| CALLED BY 	  	: 													 
| PRECONDITIONS   : 										  
| INPUT PARAMETERS: 														   
| RETURN VALUE	  :
| DESCRIPTION	  	:芯片IO复位
*******************************************************************************/
void tca5939_ResetIO(u8 subid)
{
	tca5939_Cmd(subid,OUTPUT_P0,0x00);
	tca5939_Cmd(subid,OUTPUT_P1,0x00);
}

/*******************************************************************************
| FUNCTION NAME   :tca5939_SetIO
| CALLED BY 	  	: 													 
| PRECONDITIONS   : 										  
| INPUT PARAMETERS: 														   
| RETURN VALUE	  :
| DESCRIPTION	  	:芯片设置IO
*******************************************************************************/
void tca5939_SetIO(u8 subid,u8 port,u8 pin,u8 status)
{
	u8 pinSel = pin;
	switch(status)
	{
		case 0x01:
			tca5939_Cmd(subid,port,pinSel);break;
		case 0x00:
			pinSel &= ~pin; 
			tca5939_Cmd(subid,port,pinSel);break;
		default:
			break;
	}
}


//***********************************END FILE***********************************//
