#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "key.h"
#include "timer.h"
#include "sram.h"
#include "touch.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_conf.h"
#include "myiic.h"
#include "Gui.h"
#include "DataScope_DP.h"
#include "Slave_motor.h"
#include "mytask.h"
#include "list_c.h"
#include "../Motor/motor_control.h"

#define datascope 0

/**
 * 变量区
 * */
volatile struct data1 State = {
    .mode = VOR_ID,
    .task = 0,
    .Vel = 1,
    .Frep_VOR = 10,
    .Set_Time = 1,
    .dir = 1,
};

// extern link *Task_strat;
// 测试用

void Sys_init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置系统中断优先级分组2
    delay_init(168);                                // 初始化延时函数
    uart_init(256000);                              // 初始化串口波特率为115200
    LED_Init();                                     // 初始化LED
    LCD_Init();                                     // LCD初始化
    KEY_Init();                                     // 按键初始化
    INC_IO_init();
    ir_sensor_init();
    Cam_LED_Init();
    // IIC_Init(GPIO_Pin_6 | GPIO_Pin_7, GPIOC);

    TIM3_Int_Init(1999, 83); // 定时器初始化(1ms中断),用于给lvgl提供1ms的心跳节拍

    FSMC_SRAM_Init(); // 初始化外部sram
                      //   tp_dev.init();			//触摸屏初始化
                      //  LCD_Init();
    FT5206_Init();
    tp_dev.scan = FT5206_Scan;             // 扫描函数指向GT9147触摸屏扫描
    tp_dev.touchtype |= 0X80;              // 电容屏
    tp_dev.touchtype |= lcddev.dir & 0X01; // 横屏还是竖屏
    LCD_Display_Dir(1);

    lv_init();            // lvgl系统初始化
    lv_port_disp_init();  // lvgl显示接口初始化,放在lv_init()的后面
    lv_port_indev_init(); // lvgl输入接口初始化,放在lv_init()的后面
    DataScope_DMA_init();
    TIM1_encoder_Init(50000 + 1, 2 - 1);
#if datascope
    TIM7_DataScope_Init(2500, 839);
#endif
    TIM5_Slave_Init(50000, 10);
    Slave_DIR_Init();
    uart3_init(9600);
    TIM4_Motor_Int_Init(999, 83);
    PCout(6) = 0; // 相机led归零
}

struct
{
    u8 Cmd;
    u8 Mode;
    u16 Time;
    u8 M_Vel;
    u16 M_F;
} U_D; // 串口协议

int k;
void usart_data_send(int i)
{
    u8 send_buff[10] = {0XFE, 0XFD};
    short temp;
    memset(&send_buff[2], 0, 8);
    k = i;
    send_buff[2] = State.flag;
    if (Task_lisk(i)->state)
    {
        send_buff[3] = Task_lisk(i)->mode;
        send_buff[4] = Task_lisk(i)->Set_Time / 256;
        send_buff[5] = Task_lisk(i)->Set_Time;
        send_buff[6] = Task_lisk(i)->Vel;
        temp = Task_lisk(i)->Frep_VOR * 10;
        send_buff[7] = temp / 256;
        send_buff[8] = temp;
    }

    send_buff[9] = 0XFC;
    for (i = 0; i < 10; i++)
    {
        while ((USART1->SR & USART_SR_TXE) == 0)
            ;
        USART1->DR = send_buff[i];
    }
}

void usart_data_read(void)
{

    short i = 2, k;
    link *temp;
    u16 Res;
    float Motor_f;
    U_D.Cmd = USART_RX_BUF[2];
    U_D.Mode = USART_RX_BUF[3];
    Res = USART_RX_BUF[4];
    U_D.Time = Res * 256 + USART_RX_BUF[5];
    U_D.M_Vel = USART_RX_BUF[6];
    Res = USART_RX_BUF[7];
    U_D.M_F = Res * 256 + USART_RX_BUF[8];
    Uart_len = 2; // 标志位清楚
    if (U_D.Cmd == 1 && Task_lisk(1)->state == 1)
    {
					State.uart_cmd=1;
//        State.task = 0; // 用于作为启示指示
//        State.flag = 1;
        // Start_timer = lv_timer_create(Start_timer_handler, 5, 0);
    }
    if (U_D.Cmd == 2)
    {
        State.uart_cmd=2;
    }

    if (U_D.Cmd / 10 == 3)
    {
        // 读取
        i = U_D.Cmd % 10;
        Motor_f = U_D.M_F;
        if (i >= 0 && i <= 8) // 输入限制
        {
            Task_lisk(i)->mode = U_D.Mode;
            Task_lisk(i)->Set_Time = U_D.Time;
            Task_lisk(i)->Vel = U_D.M_Vel;
            Task_lisk(i)->Frep_VOR = Motor_f / 10;
            Task_lisk(i)->state = 1;
            table_show();
            table_arrange();
            Table_reflush(Task_lisk(i)->Table, Task_lisk(i));
        }
    }
    if (U_D.Cmd / 10 == 4)
    {
        i = U_D.Cmd % 10;
        usart_data_send(i);
    }
    if (U_D.Cmd / 10 == 5)
    {
        temp = Task_lisk(0);
        for (;;)
        {
            if (temp->next != NULL)
            {
                if (temp->next->state == 0)
                {
                    temp->state = 0;
                    lv_obj_add_flag(temp->Table, LV_OBJ_FLAG_HIDDEN);
                    break;
                }
            }
            else
                break;
            temp = temp->next;
        }

        table_arrange();
        Table_reflush(Task_lisk(i)->Table, Task_lisk(i));
    }
    memset(USART_RX_BUF, 0, 50);
}

void host_usart_handler(void)
{
    if (Uart_len >= 9)
    {
        usart_data_read();
    }
}

int main(void)
{
    Sys_init();
    if ((RCC->CSR & RCC_CSR_WWDGRSTF) == 0)
        cam_timer_on_off(5000, 1);
    RCC->CSR |= RCC_CSR_RMVF;
    ui_motor_sever_init();
    while (1)
    {
        if (my_lv_time % 100 == 0)
            host_usart_handler();
        if (Task_lisk(1)->state)
            State.list_state = 1;
        else
            State.list_state = 0;
        // State_Ctrl();
        touch_sever(1, NULL);
        lv_task_handler();
        delay_ms(5);
    }
}

unsigned char Send_Count; // 串口需要发送的数据个数
// short Acc_Data[4]={0,0,0,0};
short Angle_Data[4] = {0, 0, 0, 0};
// short Angle_Speed[4]={0,0,0,0};
int Acc_R_Data[4] = {0, 0, 0, 0};

void USART3_IRQHandler(void) // 串口1中断服务程序
{
    // u8 Res;
    static u8 last_data = 0;
    float temp;
    u8 data = 0;
    static int Data_state = 0,                             // 尾帧
        cut = 0,                                           // 计数
        Data_Flag = 0;                                     // 接收标志位
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) // 接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
        data = USART3->DR;
        // printf(" %02X",data);
        if (last_data == 0X55 && data >= 0X50 && Data_Flag == 0)
        {
            Data_Flag = 1;
            Data_state = data;
            cut = 0;
            // printf("\r\n%d",Data_state);
            goto End;
        }
        if (Data_Flag == 1)
        {
            switch (Data_state)
            {
            case 0X53: // 角度输出
            {
                // Angle_Data[cut]=data;
                if (cut % 2 == 1)
                {
                    Angle_Data[(int)cut / 2] = (data << 8) | last_data;
                    temp = (float)Angle_Data[1] / 65536 * 360;
                    temp = temp * temp;
                    State.inc_ang = (float)Angle_Data[0] / 65536 * 360;
                    State.inc_ang = State.inc_ang * State.inc_ang;
                    State.inc_ang = State.inc_ang + temp;
                    State.inc_ang = sqrt(State.inc_ang);
                    State.act_ang = State.inc_ang;
                    State.inc_ang -= State.std_ang;
                }
            }
            break;
            default:
            {
                Data_state = 0;
                Data_Flag = 0;
                cut = 0;
                last_data = 0;
                goto End;
            }
            break;
            }
            cut++;
            if (cut >= 8)
            {
                cut = 0;
                Data_Flag = 0;
                Data_state = 0;
                goto End;
            }
        }
    End:
        last_data = data;
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}
void hex_show(char *buff, u32 data)
{
    int i = 10, temp;
    char res[10];
    memset(res, '0', 7);
    res[0] = '0';
    res[1] = 'X';
    while (i > 0)
    {
        if (data != 0)
        {
            i--;
            temp = data % 16;
            if (temp > 9)
            {
                res[i] = 55 + temp;
            }
            else
                res[i] = '0' + temp;
            data /= 16;
        }
        else
            break;
        /* code */
    }
    memcpy(buff, res, 10);
    return;
}

unsigned int addr;
unsigned int *add;
extern char hex_buff[20];
/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
#define x_Ros 80
#define y_Ros 120

void HardFault_Handler(void)
{
    char hex[10];
    u32 i;
    /* Go to infinite loop when Hard Fault exception occurs */
    addr = readsps();

    add = (unsigned int *)addr;
    printf("\r\n0X%x ,0X%x", add[5], add[6]);
    GPIOF->ODR = (GPIOF->IDR & ~GPIO_Pin_9);
    inc_down = 0;
    inc_up = 0;
    TIM5->CR1 = 0;
    LCD_Fill(0, 0, 800, 480, 0XFF);
    LCD_ShowString(30, 30, 500, 100, 24, "HardFault Handler SP REG");
    hex_show(hex, (u32)addr);
    LCD_ShowString(x_Ros, 80, 150, 100, 24, hex);
    hex_show(hex, addr);
    LCD_ShowString(x_Ros, y_Ros, 150, 100, 24, hex);
    for (i = 0; i < 10; i++)
    {
        LCD_ShowNum(x_Ros - 30, y_Ros + i * 30, i, 2, 24);
        hex_show(hex, add[i]);
        LCD_ShowString(x_Ros, y_Ros + i * 30, 150, 100, 24, hex);
    }
    for (i = 0; i < 10; i++)
    {
        LCD_ShowNum(x_Ros - 30 + 200, y_Ros + i * 30, i + 10, 2, 24);
        hex_show(hex, add[i + 10]);
        LCD_ShowString(x_Ros + 200, y_Ros + i * 30, 150, 100, 24, hex);
    }
    for (i = 0; i < 10; i++)
    {
        LCD_ShowNum(x_Ros - 30 + 400, y_Ros + i * 30, i + 20, 2, 24);
        hex_show(hex, add[i + 20]);
        LCD_ShowString(x_Ros + 400, y_Ros + i * 30, 150, 100, 24, hex);
    }

    while (1)
    {

        if ((USART1->SR & USART_SR_RXNE) != 0)
        {
            GPIOF->ODR = ((~GPIOF->IDR) & GPIO_Pin_9) | (GPIOF->IDR & ~GPIO_Pin_9);
            printf("\r\n0X%x ,0X%x", add[5], add[6]);
            printf("\r\nDR 0X%x ,0X%x", add[USART1->DR], add[USART1->DR + 1]);
            USART1->SR &= ~USART_SR_RXNE;
        }
    }
}
