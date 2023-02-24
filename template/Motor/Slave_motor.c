#include "Slave_motor.h"
#include "led.h"
// #include "pwm.h"
#include "Gui.h"
#include "DataScope_DP.h"
#include "usart.h"
#define motor_step 10000 * 10
#define angle_step 277.8
double Out_Put_data, tempb, tempc;
const float zhengtai[] = {0.0671401181328362, 0.0787266435571731, 0.0921651003682848, 0.107724973016865, 0.125710460380519, 0.146464228763777, 0.170371462141849, 0.197864217263118, 0.229426088805385, 0.265597186890224, 0.306979425865986, 0.354242119344259, 0.408127871992306, 0.469458753526526, 0.539142734707362, 0.618180358900804, 0.707671615951506, 0.808822977724624, 0.922954546746783, 1.05150726095383, 1.19605008869167, 1.35828713888967, 1.54006460182370, 1.74337742621519, 1.97037562869881, 2.22337012207686, 2.50483793942468, 2.81742672219528, 3.16395833218678, 3.54743143979158, 3.97102293456122, 4.43808799902682, 4.95215868314878, 5.51694081497150, 6.13630908326956, 6.81430013042500, 7.55510349869587, 8.36305028063199, 9.24259933485288, 10.1983209418842, 11.2348777913802, 12.3570032119281, 13.5694765777794, 14.8770958532719, 16.2846472653379, 17.7968721272041, 19.4184308720030, 21.1538643932651, 23.0075528298371, 24.9836719752639, 27.0861475356447, 29.3186075048669, 31.6843329713773, 34.1862077155999, 36.8266670010583, 39.6076460044813, 42.5305283698692, 45.5960954079025, 48.8044764943716, 52.1551012487038, 55.6466540953934, 59.2770318264577, 63.0433047912726, 66.9416823406611, 70.9674831443941, 75.1151109848855, 79.3780366045036, 83.7487861493974, 88.2189367089977, 92.7791193975023, 97.4190303619495, 102.127450031335, 106.892270843220, 111.700533599152, 116.538472508835, 121.391568886448, 126.244613361912, 131.081776366588, 135.886686548275, 140.642516665838, 145.332076411009, 149.937911505345, 154.442408325574, 158.827903222264, 163.076795616263, 167.171663886313, 171.095383000717, 174.831242797397, 178.363065780919, 181.675323283072, 184.753248825993, 187.582947534037, 190.151500462857, 192.447062751362, 194.458954554239, 196.177743778858, 197.595319730034, 198.704956858232, 199.501367910180, 199.980745894189, 200.140794394111, 199.980745894189, 199.501367910180, 198.704956858232, 197.595319730034, 196.177743778858, 194.458954554239, 192.447062751362, 190.151500462857, 187.582947534037, 184.753248825993, 181.675323283072, 178.363065780919, 174.831242797397, 171.095383000717, 167.171663886313, 163.076795616263, 158.827903222264, 154.442408325574, 149.937911505345, 145.332076411009, 140.642516665838, 135.886686548275, 131.081776366588, 126.244613361912, 121.391568886448, 116.538472508835, 111.700533599152, 106.892270843220, 102.127450031335, 97.4190303619495, 92.7791193975023, 88.2189367089977, 83.7487861493974, 79.3780366045036, 75.1151109848855, 70.9674831443941, 66.9416823406611, 63.0433047912726, 59.2770318264577, 55.6466540953934, 52.1551012487038, 48.8044764943716, 45.5960954079025, 42.5305283698692, 39.6076460044813, 36.8266670010583, 34.1862077155999, 31.6843329713773, 29.3186075048669, 27.0861475356447, 24.9836719752639, 23.0075528298371, 21.1538643932651, 19.4184308720030, 17.7968721272041, 16.2846472653379, 14.8770958532719, 13.5694765777794, 12.3570032119281, 11.2348777913802, 10.1983209418842, 9.24259933485288, 8.36305028063199, 7.55510349869587, 6.81430013042500, 6.13630908326956, 5.51694081497150, 4.95215868314878, 4.43808799902682, 3.97102293456122, 3.54743143979158, 3.16395833218678, 2.81742672219528, 2.50483793942468, 2.22337012207686, 1.97037562869881, 1.74337742621519, 1.54006460182370, 1.35828713888967, 1.19605008869167, 1.05150726095383, 0.922954546746783, 0.808822977724624, 0.707671615951506, 0.618180358900804, 0.539142734707362, 0.469458753526526, 0.408127871992306, 0.354242119344259, 0.306979425865986, 0.265597186890224, 0.229426088805385, 0.197864217263118, 0.170371462141849, 0.146464228763777, 0.125710460380519, 0.107724973016865, 0.0921651003682848, 0.0787266435571731, 0.0671401181328362};

void Slave_DIR_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); // 使能GPIOF时钟

    // GPIOF9,F10初始化设置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;      // 普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;   // 上拉
    GPIO_Init(GPIOC, &GPIO_InitStructure);             // 初始化

    GPIO_SetBits(GPIOC, GPIO_Pin_2); // GPIOF9,F10设置高，灯灭
}

/**
 * 定时器5脉冲记录
 * */
void TIM1_encoder_Init(u32 arr, u32 psc)
{
    // 此部分需手动修改IO口设置

    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    // TIM_ICInitTypeDef TIM_ICInitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);  // TIM14时钟使能
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // 使能PORTF时钟

    // GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_TIM1); // GPIOF9复用为定时器14

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;   // GPIOF9
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; // 复用功能
    GPIO_Init(GPIOA, &GPIO_InitStructure);       // 初始化PF9

    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  // 定时器分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseStructure.TIM_Period = arr;                     // 自动重装载值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); // 初始化定时器14
                                                    //  TIM_SelectSlaveMode
    TIM1->SMCR = 0X7;                               // 时钟源连接至定时器5通道3
    TIM_ARRPreloadConfig(TIM1, ENABLE);             // ARPE使能

    TIM_Cmd(TIM1, ENABLE); // 使能TIM14
    TIM1->CNT = motor_Middele;
}

/***
 * 方向io口设置
 * */
void IO_init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); // 使能GPIOA时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;             // GPIOF9
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;         // 复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;    // 速度100MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;        // 推挽复用输出
    // GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
    GPIO_Init(GPIOF, &GPIO_InitStructure); // 初始化PF9
    GPIO_SetBits(GPIOF, GPIO_Pin_2);
}

void INC_IO_init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);  // 使能GPIOA时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; // GPIOF9
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;          // 复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;     // 速度100MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;         // 推挽复用输出
    // GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
    GPIO_Init(GPIOC, &GPIO_InitStructure); // 初始化PF9
    GPIO_ResetBits(GPIOC, GPIO_Pin_0);
    GPIO_ResetBits(GPIOC, GPIO_Pin_1);
}

void ir_sensor_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); // 使能GPIOA时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;             // GPIOF9
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;          // 复用功能
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    // GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
    GPIO_Init(GPIOC, &GPIO_InitStructure); // 初始化PF9
}

// /**
//  * 外部中断
//  * */
// void EXTIX_Init(void)
// {
//     NVIC_InitTypeDef NVIC_InitStructure;
//     EXTI_InitTypeDef EXTI_InitStructure;

//     RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); //使能SYSCFG时钟

//     SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF, EXTI_PinSource2); // PE4 连接到中断线4
//     // SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);//PA0 连接到中断线0

//     /* 配置EXTI_Line0 */
//     EXTI_InitStructure.EXTI_Line = EXTI_Line2;                     // LINE0
//     EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;            //中断事件
//     EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; //上升沿触发
//     EXTI_InitStructure.EXTI_LineCmd = ENABLE;                      //使能LINE0
//     EXTI_Init(&EXTI_InitStructure);                                //配置

//     NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;             //外部中断4
//     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; //抢占优先级1
//     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;        //子优先级2
//     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;              //使能外部中断通道
//     NVIC_Init(&NVIC_InitStructure);                              //配置
// }

/**
 * 定时器记录
 * */
// void EXTI2_IRQHandler(void)
// {
//     // if (EXTI_GetFlagStatus(EXTI_Line10) == SET)
//     {
//         if (PFout(2) == 1)
//         {
//             // down_max = TIM1->CNT;
//             TIM1->CR1 &= ~TIM_CR1_DIR;
//             TIM1->CR1 |= TIM_CR1_DIR;
//         }
//         else
//         {
//             // up_max = TIM1->CNT;
//             TIM1->CR1 &= ~TIM_CR1_DIR;
//         }
//     }
//     EXTI_ClearITPendingBit(EXTI_Line10); //清除LINE4上的中断标志位
// }

/***
 * 定时器 999 83 =1000hz
 * */
void TIM4_Motor_Int_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); /// 使能TIM4时钟

    TIM_TimeBaseInitStructure.TIM_Period = arr;                     // 自动重装载值
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;                  // 定时器分频
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure); // 初始化TIM4

    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); // 允许定时器3更新中断
    TIM_Cmd(TIM4, ENABLE);                     // 使能定时器3

    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;              // 定时器3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; // 抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;        // 子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void TIM7_DataScope_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE); /// 使能TIM7时钟

    TIM_TimeBaseInitStructure.TIM_Period = arr;                     // 自动重装载值
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;                  // 定时器分频
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM7, &TIM_TimeBaseInitStructure); // 初始化TIM7

    NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;              // 定时器3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01; // 抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;        // 子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE); // 允许定时器3更新中断
    TIM_Cmd(TIM7, ENABLE);                     // 使能定时器3
}

void TIM5_Slave_Init(u32 arr, u32 psc)
{
    // 此部分需手动修改IO口设置

    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);  // TIM5时钟使能
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // 使能PORTF时钟

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_TIM5); // GPIOF9复用为定时器14

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;          // GPIOF9
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;       // 复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 速度100MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     // 推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       // 上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);             // 初始化PF9

    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  // 定时器分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseStructure.TIM_Period = arr;                     // 自动重装载值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); // 初始化定时器14

    // 初始化TIM5 Channel1 PWM模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             // 选择定时器模式:TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;      // 输出极性:TIM输出比较极性低
    TIM_OC3Init(TIM5, &TIM_OCInitStructure);                      // 根据T指定的参数初始化外设TIM1 4OC1
    // TIM_SelectMasterSlaveMode(TIM5,)
    TIM5->CR2 = 0X60;
    TIM_SelectMasterSlaveMode(TIM5, TIM_MasterSlaveMode_Enable);
    // TIM_OC3PreloadConfig(TIM5, TIM_OCPreload_Enable);  //使能TIM5在CCR1上的预装载寄存器

    TIM_ARRPreloadConfig(TIM5, ENABLE); // ARPE使能

    // TIM_Cmd(TIM5, ENABLE);  //使能TIM14
    TIM5->CCR3 = arr / 2;
}

void TIM5_Slave_Set(u32 arr, u32 psc)
{
    // 此部分需手动修改IO口设置
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    // TIM_OCInitTypeDef  TIM_OCInitStructure;

    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  // 定时器分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseStructure.TIM_Period = arr;                     // 自动重装载值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); // 初始化定时器14
    TIM5->CCR3 = arr / 2;
    // 初始化TIM5 Channel1 PWM模式
    //  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
    //  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    //  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性低
    //  // TIM_OC3Init(TIM5, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 4OC1
    //  TIM_SelectMasterSlaveMode(TIM5,)
    //  TIM5->CR2=0X60;
    //  TIM_SelectMasterSlaveMode(TIM5,TIM_MasterSlaveMode_Enable);
    //  TIM_OC3PreloadConfig(TIM5, TIM_OCPreload_Enable);  //使能TIM5在CCR1上的预装载寄存器

    //  TIM_ARRPreloadConfig(TIM5,ENABLE);//ARPE使能

    TIM_Cmd(TIM5, ENABLE); // 使能TIM14
}

float sin_data;
int sin_time = 0;
int tim_count, last_count, plus_f = 1;
extern struct data1 State;
#define Pi 3.1415926
void TIM4_IRQHandler(void)
{
    if (TIM4->SR & TIM_IT_Update) // 溢出中断
    {
        motor_handler();
    }
    TIM4->SR = (uint16_t)~TIM_IT_Update;
}

void motor_handler(void)
{
    static short last_motor = 1;
    if (sin_time == 1000000 || State.motor_run == 0)
        sin_time = 0;
    switch (State.mode)
    {
    case VOR_ID:
        VOR_handler();
        break;
    case Ctn_ID:
        Continue_handler();
        break;
    case OVAR_ID:
        OVAR_handler();
        break;
    case VHIT_ID:
        VHIT_handler();
        break;
    default:
        break;
    }
    if (last_motor != State.motor_run && State.motor_run == 0)
        TIM5->CR1 &= ~TIM_CR1_CEN, TIM5->ARR = 10, TIM5->PSC = 5;
    last_motor = State.motor_run;
}

void VOR_handler(void)
{
    if (State.motor_run && State.flag)
        Slave_motor(State.Vel, State.Frep_VOR);
    else
    {
        if (State.flag == 0 && State.motor_run)
            if (slave_sin_back())
                State.motor_run = 0;
    }
}

void Continue_handler(void)
{
    if (State.motor_run && State.flag)
        // if (TIM5->CR1 & TIM_CR1_CEN == 0)
        //     Slave_Motor_Vel_Mode(80, 1);
        // else
        Slave_Motor_Vel_Mode(State.Vel, 0);
    else
    {
        if (State.flag == 0 && State.motor_run)
            if (Slave_Back(motor_Middele))
                State.motor_run = 0;
    }
}

void OVAR_handler(void)
{
    if (State.inc_Rec)
        if (INC_Ctrl(State.Frep_VOR))
            State.inc_Rec = 0;
    // State.inc_Rec = 0;
    Continue_handler();
}

void VHIT_handler(void)
{
    if (State.motor_run && State.flag)
        mode4();
    else
    {
        if (State.flag == 0 && State.motor_run)
            if (Slave_Back(motor_Middele))
                State.motor_run = 0;
    }
}

// 上位机
void TIM7_IRQHandler(void)
{
    if (TIM7->SR & TIM_IT_Update) // 溢出中断
    {
        DataScope_DMA(5, (double)Out_Put_data, (double)sin_time, (double)TIM1->CNT, (double)tempc, (double)tempb);
    }
    TIM7->SR = (uint16_t)~TIM_IT_Update;
}

void tim_f_set(int f)
{
    // #define sys_clk 42000000
    u8 Mins_flag;
    int time_arr, time_psc;
    int a, clk = 84000000, c, y, r = 0, t, i;
    if (f < 0)
    {
        f = -f;
        Mins_flag = 0;
    }
    else
        Mins_flag = 1;
    a = clk / (f);
    c = sqrt(a);
    y = 10000000;
    for (i = 2; i <= c; i++)
    {
        t = a % (i);

        if (y > t && a / i < 65536)
        {

            y = t;

            r = i;
            if (y == 0 && a / i < 65536)
                break;
        }
    }
    c = 10;

    time_arr = a / r - 1;
    time_psc = r - 1;
    tempb = time_arr;
    tempc = time_psc;
    // Out_Put_data = sys_clk / time_arr / time_psc;
    if (f > 1000 || TIM5->CNT > TIM5->CCR3)
    {
        Dir_Ctl(Mins_flag);
        if (time_arr != TIM5->ARR || time_psc != TIM5->PSC)
            TIM5_Slave_Set(time_arr, time_psc);
    }
    TIM5->CR1 |= TIM_CR1_CEN;
}

void tim_f_sin_set(int f)
{
    // #define sys_clk 42000000
    u8 Mins_flag;
    int time_arr, time_psc;
    int a, clk = 84000000, c, y, r = 0, t, i;
    if (f < 0)
    {
        f = -f;
        Mins_flag = 0;
    }
    else
        Mins_flag = 1;
    if (f < 100)
        return;
    a = clk / (f);
    c = sqrt(a);
    y = 10000000;
    for (i = 2; i <= c; i++)
    {
        t = a % (i);

        if (y > t && a / i < 65536)
        {

            y = t;

            r = i;
            if (y == 0 && a / i < 65536)
                break;
        }
    }
    c = 10;

    time_arr = a / r - 1;
    time_psc = r - 1;
    tempb = time_arr;
    tempc = time_psc;
    // Out_Put_data = sys_clk / time_arr / time_psc;
    if (f > 1000 || TIM5->CNT > TIM5->CCR3)
    {
        Dir_Ctl(Mins_flag);
        if (time_arr != TIM5->ARR || time_psc != TIM5->PSC)
            TIM5_Slave_Set(time_arr, time_psc);
    }
    TIM5->CR1 |= TIM_CR1_CEN;
}
/**
 * 带边沿检测
 * */
void Dir_Ctl(u8 i)
{
    static u8 i_edge;
    if (i != i_edge)
    {
        // TIM1->CR1 &= ~TIM_CR1_CEN;
        // TIM5->CCR3 = 0;
        // TIM5->CR1 &= (uint16_t)~TIM_CR1_CEN;
        if (i == 0)
        {
            // TIM5->CR1 &= (uint16_t)~TIM_CR1_CEN;
            // io口
            PCout(2) = 1;
            TIM1->CR1 &= ~TIM_CR1_DIR;
            TIM1->CR1 |= TIM_CR1_DIR;

            // TIM5->CR1 |= TIM_CR1_CEN;
        }
        else
        {
            PCout(2) = 0;
            // TIM1->CNT++;
            TIM1->CR1 &= ~TIM_CR1_DIR;
        }
        // TIM1->CR1 |= TIM_CR1_CEN;
        // TIM5->CR1 |= TIM_CR1_CEN;
    }
    i_edge = i;
}

/**
 * 伺服总步数估计在360*180步
 * angle_speed 角速度
 * feq         频率
 * */
void Slave_motor(int Angle_speed, float Feq)
{
    float time_max = 1000;
    int tem = time_max / Feq;
    if (sin_time % tem <= cam_led_time)
        cam_led = 1;
    else
        cam_led = 0;
    sin_data = sin(((float)2 * Pi * Feq * sin_time++ / 1000));
    tim_f_sin_set(angle_step * sin_data * Angle_speed);
    // last_data = sin_time;
}

u8 Slave_Back(int Tag_Pos)
{
// #define P 0.08
#define limit 8000 // 限速
    float p = 250;
#define Val 20
    // static short Last_data;
    float OutPut;
    // float minus;
    // minus = Tag_Pos - (u16)TIM1->CNT;
    OutPut = p * (Tag_Pos - (u16)TIM1->CNT);
    // if(OutPut>1000) OutPut=1000;
    if (OutPut > limit)
        OutPut = limit;
    if (OutPut < -limit)
        OutPut = -limit;
    // if(Last_data==TIM1->CNT)
    // tempb = OutPut;
    //     OutPut*=5;
    // Last_data = TIM1->CNT;
    tim_f_set((int)OutPut);
    Out_Put_data = Tag_Pos - (int)TIM1->CNT;
    if (Out_Put_data < Val && Out_Put_data > -Val)
        return 1;
    else
        return 0;
}

u8 Slave_Back_spd(int Tag_Pos, int spd)
{
    // #define P 0.08
    float p = 250;
#define Val 20
    // static short Last_data;
    float OutPut;
    // float minus;
    // minus = Tag_Pos - (u16)TIM1->CNT;
    OutPut = p * (Tag_Pos - (u16)TIM1->CNT);
    // if(OutPut>1000) OutPut=1000;
    if (OutPut > spd)
        OutPut = spd;
    if (OutPut < -spd)
        OutPut = -spd;
    // if(Last_data==TIM1->CNT)
    // tempb = OutPut;
    //     OutPut*=5;
    // Last_data = TIM1->CNT;
    tim_f_set((int)OutPut);
    Out_Put_data = Tag_Pos - (int)TIM1->CNT;
    if (Out_Put_data < Val && Out_Put_data > -Val)
        return 1;
    else
        return 0;
}
u8 slave_sin_back(void)
{
    static float Last_data = -1;
    // if(sin_time%1000)
    float time_max = 1000;
    int tem = time_max / State.Frep_VOR;
    // 完整周期
    if (sin_time % tem == 0)
    {
        if (Slave_Back(motor_Middele))
        {
            Last_data = 10; // sin_data 大小在-1~1；清除数据
            sin_time = 0;
            return 1;
        }
        else
            return 0;
    }
    else
    {
        // tempb = sin_time % tem;
        Slave_motor(State.Vel, State.Frep_VOR);
        Last_data = sin_data;
        return 0;
    }
}

void Slave_Motor_Vel_Mode(float Vel, u8 clear_fg)
{
    tim_f_set((int)(Vel * angle_step));
}

u8 INC_Ctrl(float tag)
{
#define inc_limit 0.1
    // static u8 times;
    static int time_limit;
    static uint8_t finish = 0;
    float minus = State.inc_ang - tag;
    printf("\r\n%d", time_limit);
    if (finish == 0)
    {
        if (State.inc_ang < 0.2 && tag < 1)
            time_limit++;
        if (State.inc_ang < tag)
            PCout(1) = 0, PCout(0) = 1;
        else
            PCout(1) = 1, PCout(0) = 0;
        if (minus < inc_limit && minus > -inc_limit || time_limit > 8000)
        {
            PCout(1) = 0, PCout(0) = 0;
            finish = 1;
            time_limit = 0;
            // return 1;
        }
        else
            return 0;
    }
    else
    {
        if (State.inc_ang < 5 && time_limit < 2000)
        {
            PCout(1) = 1, PCout(0) = 0;
            time_limit++;
        }
        else
        {
            finish = 0;
            time_limit = 0;
            return 1;
        }
    }
    return 0;
}

void mode4(void)
{
#define one_time 300
    int t = 0;
    static u8 flag = 0;
    t = sin_time * 2 / 3;
    sin_time++;
    // temp = t;
    // t=t-temp;
    // sin_data = zhengtai[t]
    if (sin_time < one_time)
        tim_f_set((int)(zhengtai[t] * angle_step));
    if (sin_time >= one_time && flag == 0 && Slave_Back_spd(motor_Middele, 1150))
        flag = 1;
    if (flag)
    {
        TIM5->CR1 &= ~TIM_CR1_CEN;
    }
    if (sin_time >= 5000)
        sin_time = 0, flag = 0;
}
