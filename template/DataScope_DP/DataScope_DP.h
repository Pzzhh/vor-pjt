

#ifndef __DATA_PRTOCOL_H
#define __DATA_PRTOCOL_H
#include "stm32f4xx.h" 
 
extern unsigned char DataScope_OutPut_Buffer[42];	   //待发送帧数据缓存区


void DataScope_Get_Channel_Data(float Data,unsigned char Channel);    // 写通道数据至 待发送帧数据缓存区

unsigned char DataScope_Data_Generate(unsigned char Channel_Number);  // 发送帧数据生成函数 
 int Shangweiji(int Channel_num,float a1,float a2,float a3,float a4,float a5,float a6,float a7,float a8,float a9);
 void DataScope_DMA_init(void);
 void DataScope_DMA_Enable(DMA_Stream_TypeDef *DMA_Streamx, u16 ndtr);
 int DataScope_DMA(short num, ...);
 
#endif 



