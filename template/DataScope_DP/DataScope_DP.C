
#include "DataScope_DP.h"
#include "usart.h"
// #include "dma.h"
#include "stdio.h"
#include "stdarg.h"

unsigned char DataScope_OutPut_Buffer[42] = {0}; //串口发送缓冲区

//函数说明：将单精度浮点数据转�?4字节数据并存入指定地址
//附加说明：用户无需直接操作此函�?
// target:目标单精度数�?
// buf:待写入数�?
// beg:指定从数组第几个元素开始写�?
//函数无返�?
void Float2Byte(float *target, unsigned char *buf, unsigned char beg)
{
    unsigned char *point;
    point = (unsigned char *)target; //得到float的地址
    buf[beg] = point[0];
    buf[beg + 1] = point[1];
    buf[beg + 2] = point[2];
    buf[beg + 3] = point[3];
}

//函数说明：将待发送通道的单精度浮点数据写入发送缓冲区
// Data：通道数据
// Channel：选择通道�?1-10�?
//函数无返�?
void DataScope_Get_Channel_Data(float Data, unsigned char Channel)
{
    if ((Channel > 10) || (Channel == 0))
        return; //通道个数大于10或等�?0，直接跳出，不执行函�?
    else
    {
        switch (Channel)
        {
        case 1:
            Float2Byte(&Data, DataScope_OutPut_Buffer, 1);
            break;
        case 2:
            Float2Byte(&Data, DataScope_OutPut_Buffer, 5);
            break;
        case 3:
            Float2Byte(&Data, DataScope_OutPut_Buffer, 9);
            break;
        case 4:
            Float2Byte(&Data, DataScope_OutPut_Buffer, 13);
            break;
        case 5:
            Float2Byte(&Data, DataScope_OutPut_Buffer, 17);
            break;
        case 6:
            Float2Byte(&Data, DataScope_OutPut_Buffer, 21);
            break;
        case 7:
            Float2Byte(&Data, DataScope_OutPut_Buffer, 25);
            break;
        case 8:
            Float2Byte(&Data, DataScope_OutPut_Buffer, 29);
            break;
        case 9:
            Float2Byte(&Data, DataScope_OutPut_Buffer, 33);
            break;
        case 10:
            Float2Byte(&Data, DataScope_OutPut_Buffer, 37);
            break;
        }
    }
}

//函数说明：生�? DataScopeV1.0 能正确识别的帧格�?
// Channel_Number，需要发送的通道个数
//返回发送缓冲区数据个数
//返回0表示帧格式生成失�?
unsigned char DataScope_Data_Generate(unsigned char Channel_Number)
{
    if ((Channel_Number > 10) || (Channel_Number == 0))
    {
        return 0;
    } //通道个数大于10或等�?0，直接跳出，不执行函�?
    else
    {
        DataScope_OutPut_Buffer[0] = '$'; //帧头

        switch (Channel_Number)
        {
        case 1:
            DataScope_OutPut_Buffer[5] = 5;
            return 6;
        case 2:
            DataScope_OutPut_Buffer[9] = 9;
            return 10;
        case 3:
            DataScope_OutPut_Buffer[13] = 13;
            return 14;
        case 4:
            DataScope_OutPut_Buffer[17] = 17;
            return 18;
        case 5:
            DataScope_OutPut_Buffer[21] = 21;
            return 22;
        case 6:
            DataScope_OutPut_Buffer[25] = 25;
            return 26;
        case 7:
            DataScope_OutPut_Buffer[29] = 29;
            return 30;
        case 8:
            DataScope_OutPut_Buffer[33] = 33;
            return 34;
        case 9:
            DataScope_OutPut_Buffer[37] = 37;
            return 38;
        case 10:
            DataScope_OutPut_Buffer[41] = 41;
            return 42;
        }
    }
    return 0;
}

int Shangweiji(int Channel_num, float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8, float a9)
{
    int Send_Count = 0, i;
    DataScope_Get_Channel_Data(a1, 1);
    DataScope_Get_Channel_Data(a2, 2);
    DataScope_Get_Channel_Data(a3, 3);
    DataScope_Get_Channel_Data(a4, 4);
    DataScope_Get_Channel_Data(a5, 5);
    DataScope_Get_Channel_Data(a6, 6);
    //			DataScope_Get_Channel_Data(a7, 7 );
    //				DataScope_Get_Channel_Data(a8,8 );
    //	DataScope_Get_Channel_Data(a9, 9 );
    Send_Count = DataScope_Data_Generate(Channel_num);
    for (i = 0; i < Send_Count; i++)
    {
        while ((USART1->SR & 0X40) == 0)
            ;
        USART1->DR = DataScope_OutPut_Buffer[i];
    }
    return 0;
}

// DMAx的各通道配置
//这里的传输形式是固定�?,这点要根据不同的情况来修�?
//从存储器->外设模式/8位数据宽�?/存储器增量模�?
// DMA_Streamx:DMA数据�?,DMA1_Stream0~7/DMA2_Stream0~7
// chx:DMA通道选择,@ref DMA_channel DMA_Channel_0~DMA_Channel_7
// par:外设地址
// mar:存储器地址
// ndtr:数据传输�?
void DataScope_DMA_Config(DMA_Stream_TypeDef *DMA_Streamx, u32 chx, u32 par, u32 mar, u16 ndtr)
{

    DMA_InitTypeDef DMA_InitStructure;

    if ((u32)DMA_Streamx > (u32)DMA2) //得到当前stream是属于DMA2还是DMA1
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE); // DMA2时钟使能
    }
    else
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); // DMA1时钟使能
    }
    DMA_DeInit(DMA_Streamx);

    while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE)
    {
    } //等待DMA可配�?

    /* 配置 DMA Stream */
    DMA_InitStructure.DMA_Channel = chx;                                    //通道选择
    DMA_InitStructure.DMA_PeripheralBaseAddr = par;                         // DMA外设地址
    DMA_InitStructure.DMA_Memory0BaseAddr = mar;                            // DMA 存储�?0地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                 //存储器到外设模式
    DMA_InitStructure.DMA_BufferSize = ndtr;                                //数据传输�?
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        //外设非增量模�?
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 //存储器增量模�?
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //外设数据长度:8�?
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         //存储器数据长�?:8�?
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // 使用普通模�?
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                   //中等优先�?
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;         //存储器突发单次传�?
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; //外设突发单次传输
    DMA_Init(DMA_Streamx, &DMA_InitStructure);                          //初始化DMA Stream
}
//开启一次DMA传输
// DMA_Streamx:DMA数据�?,DMA1_Stream0~7/DMA2_Stream0~7
// ndtr:数据传输�?
void DataScope_DMA_Enable(DMA_Stream_TypeDef *DMA_Streamx, u16 ndtr)
{

    DMA_Cmd(DMA_Streamx, DISABLE); //关闭DMA传输

    while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE)
    {
    } //确保DMA可以被设�?

    DMA_SetCurrDataCounter(DMA_Streamx, ndtr); //数据传输�?

    DMA_Cmd(DMA_Streamx, ENABLE); //开启DMA传输
}

double Data_input[10];

/**
 * 当通道数大�?7 存在掉包现象，注意不要被上位机欺�? 如要准确数据建议通道数低�?4
 * */
int DataScope_DMA(short num, ...)
{
    // #define num 5
    int Send_Count = 0;
    va_list valist;
    // double sum = 0.0;
    int i;
    /* �? num 个参数初始化 valist */
    va_start(valist, num);

    /* 访问所有赋�? valist 的参�? */
    for (i = 0; i < num; i++)
    {
        Data_input[i] = va_arg(valist, double);
        //  printf("\r\n %f",Data_input[i]);
    }
    /* 清理�? valist 保留的内�? */
    va_end(valist);

    for (i = 0; i < num; i++)
    {
        //该函数从1开�?
        DataScope_Get_Channel_Data(Data_input[i], i+1);
    }
    Send_Count = DataScope_Data_Generate(num);

    if (DMA_GetFlagStatus(DMA2_Stream7, DMA_FLAG_TCIF7) != RESET) //等待DMA2_Steam7传输完成
        DMA_ClearFlag(DMA2_Stream7, DMA_FLAG_TCIF7);              //清除DMA2_Steam7传输完成标志
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);                //使能串口1的DMA发�?
    DataScope_DMA_Enable(DMA2_Stream7, Send_Count);

    return 0;
}
/**
 * 带dma与串口初始化
 * */
void DataScope_DMA_init(void)
{
#define DMA_USART USART1
#define bound 256000
    uart_init(bound);
    DataScope_DMA_Config(DMA2_Stream7, DMA_Channel_4, (u32)&USART1->DR, (u32)DataScope_OutPut_Buffer, 42);
}
