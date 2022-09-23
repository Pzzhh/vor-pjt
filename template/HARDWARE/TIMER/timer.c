#include "timer.h"
#include "lvgl.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/4
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 


//通用定时器3中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///使能TIM3时钟
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//初始化TIM3
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	TIM_Cmd(TIM3,ENABLE); //使能定时器3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x0; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x2; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

//通用定时器3中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
void TIM5_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);  ///使能TIM3时钟
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);//初始化TIM3
	
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	TIM_Cmd(TIM5,ENABLE); //使能定时器3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM5_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}
//TIM4 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
// //psc：时钟预分频数
// void TIM4_PWM_Init(u32 arr,u32 psc)
// {		 					 
// 	//此部分需手动修改IO口设置
	
// 	GPIO_InitTypeDef GPIO_InitStructure;
// 	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
// 	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
// 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  	//TIM14时钟使能    
// 	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 	//使能PORTF时钟	
	
// 	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_TIM4); //GPIOF9复用为定时器14
	
// 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;           //GPIOF9
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
// 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
// 	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
// 	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
// 	GPIO_Init(GPIOB,&GPIO_InitStructure);              //初始化PF9
	  
// 	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
// 	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
// 	TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载值
// 	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
// 	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);//初始化定时器14
	
// 	//初始化TIM14 Channel1 PWM模式	 
// 	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
//  	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
// 	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性低
// 	TIM_OC1Init(TIM4, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 4OC1

// 	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  //使能TIM14在CCR1上的预装载寄存器
 
//   TIM_ARRPreloadConfig(TIM4,ENABLE);//ARPE使能 
	
// 	TIM_Cmd(TIM14, ENABLE);  //使能TIM14
 
										  
// }  

//定时器3中断服务程序
void TIM3_IRQHandler(void)
{ 		    		  			    
	if(TIM3->SR&TIM_IT_Update)//溢出中断
	{
		 lv_tick_inc(2);//lvgl的1ms心跳
	}				   
	TIM3->SR = (uint16_t)~TIM_IT_Update;
}
