#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//IIC���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/9
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

   	   		   
//IO��������
#define SDA_IN()  {GPIOC->MODER=(GPIOC->MODER&0XFFFF3FFF);}  						//����  
#define SDA_OUT() {GPIOC->MODER=(GPIOC->MODER&0XFFFF3FFF)|1<<14;	}   	//���(n*2)




//IO��������	 
#define IIC_SCL    PCout(6) //SCL
#define IIC_SDA    PCout(7) //SDA	 
#define READ_SDA   PCin(7)  //����SDA 



//IIC���в�������
void IIC_Init(uint16_t Gpio_pin,GPIO_TypeDef * Gpio);                //��ʼ��IIC��IO��				 

u8 IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 slave_addr,u8 reg);	  
u8 IIC_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf);
u8 IIC_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf);
#endif
















