#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//IIC驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/9
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

   	   		   
//IO方向设置
#define SDA_IN()  {GPIOC->MODER=(GPIOC->MODER&0XFFFF3FFF);}  						//输入  
#define SDA_OUT() {GPIOC->MODER=(GPIOC->MODER&0XFFFF3FFF)|1<<14;	}   	//输出(n*2)




//IO操作函数	 
#define IIC_SCL    PCout(6) //SCL
#define IIC_SDA    PCout(7) //SDA	 
#define READ_SDA   PCin(7)  //输入SDA 



//IIC所有操作函数
void IIC_Init(uint16_t Gpio_pin,GPIO_TypeDef * Gpio);                //初始化IIC的IO口				 

u8 IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 slave_addr,u8 reg);	  
u8 IIC_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf);
u8 IIC_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf);
#endif
















