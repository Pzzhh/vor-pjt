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

#define datascope 0
#define back_S 15         // 15s
#define Next_Wait_Time 10 //下一任务等待时间 for Next_timer_handler
#define table_del_ani 1

extern lv_obj_t *table_box;
uint32_t my_lv_time = 0;

/**
 * 变量区
 * */
struct data1 State = {
    .mode = VOR_ID,
    .task = 0,
    .Vel = 1,
    .Frep_VOR = 10,
    .Set_Time = 1,
};
lv_obj_t *update_label;
// extern link *Task_strat;
//测试用
void time_reflash()
{
    // const char *a[3] = {"-", "--", "---"};
    // const char *b[3] = {"r", "ru", "run"};
    static u32 link_add[14], cnt[15], i;
    link *t = Task_lisk(0);
    for (i = 0; i < 14; i++)
    {
        link_add[i] = (u32)(t);
        cnt[i] = t->cnt;
        t = t->next;
    }
    int time = my_lv_time / 1000;
#if table_del_ani
    // if (my_lv_time % 200 == 0)
    table_arrange_auto();
#endif
    // if (State.flag)
    lv_label_set_text_fmt(update_label, "%.2d:%.2d:%.2d Ang:%d.%.1d Ang_zero %d.%.1d", time / 3600 % 24, time / 60 % 60, time % 60, (int)State.inc_ang, f_int_tran(State.inc_ang, 10), (int)State.std_ang, f_int_tran(State.std_ang, 10));
    // lv_label_set_text_fmt(update_label, "t:%d   M:%d V:%d F:%d Mr:%d Flag:%d A:%d r:c%d ", my_lv_time - State.time_start, State.mode, State.Vel, (int)(State.Frep_VOR * 10), State.motor_run, State.flag, (int)(State.inc_ang * 10), Dropdown_mode);
    // else
    //     lv_label_set_text_fmt(update_label, a[i++]);
}

lv_timer_t *Start_timer;
lv_timer_t *flag_sev_timer;
lv_timer_t *END_timer;
lv_timer_t *NEXT_timer;
lv_timer_t *INC_timer;
lv_timer_t *Motor_timer;
lv_timer_t *Emg_stop;
lv_timer_t *back_timer;
struct
{
    lv_timer_t *e;
    short vail;
} cam_timer;

void INC_timer_handler(void);
void Motor_timer_handler(void);
void Start_timer_handler(void);
void usart_data_send(int i);
void cam_timer_on_off(int Period, int flag);

void camer_handler(lv_timer_t *e)
{
    LED0 = !LED0;
    if (e->user_data == (void *)1)
    {
        // PFout(10) = !PFout(10);
        Cam_set = !Cam_set;
    }
    else
        Cam_rec = !Cam_rec;
    // PFout(10) = !PFout(10);
    printf("\r\n timer %d re %d", PFout(10), e->repeat_count);
    if (e->repeat_count == 0)
        cam_timer.vail = 0;
    // e->paused = 1;
}

void cam_timer_on_off(int Period, int flag)
{
    if (cam_timer.e->repeat_count != 0 && cam_timer.vail == 1)
    {
        // printf("\r\n Add Next_repeat %d", cam_timer.e->repeat_count);
        cam_timer.e->repeat_count += 2;
        // printf("\r\n Add Next_repeat %d", cam_timer.e->repeat_count);
    }
    else
    {
        cam_timer.vail = 1;
        cam_timer.e = lv_timer_create(camer_handler, Period, (void *)flag); //时间启动
        cam_timer.e->repeat_count = 2;                                      //设置重复
        lv_timer_ready(cam_timer.e);                                        //就绪
        // printf("\r\n New on");
    }
}
void fun(void)
{
    volatile int *SCB_CCR = (volatile int *)0XE000ED14;
    int x, y, z;
    *SCB_CCR |= (1 << 4);
    x = 10;
    y = 0;
    z = x / y;
}
void Start_timer_handler(void)
{
    if (State.flag)
    {
        if (State.task == 0)
            State.task = 1;
        State.mode = Task_lisk(State.task)->mode;
        State.Vel = Task_lisk(State.task)->Vel;
        State.Frep_VOR = Task_lisk(State.task)->Frep_VOR;
        State.Set_Time = Task_lisk(State.task)->Set_Time;
        printf("\r\n %d %d %d %d", Task_lisk(0)->mode, Task_lisk(1)->mode, Task_lisk(2)->mode, Task_lisk(3)->mode);
        if (State.mode == OVAR_ID)
        {
            State.inc_Rec = 1;
        }
        else
            State.inc_Rec = 0;
        INC_timer = lv_timer_create(INC_timer_handler, 5, NULL);

        // printf("\r\n cam_timer %d %d", cam_timer->paused, cam_timer->repeat_count);
        printf("\r\n Start End");
        start_btn_flash();
        lv_timer_del(Start_timer);
        // fun();
    }
}

void NEXT_timer_handler(void)
{
    int time = (int)my_lv_time - State.time_start;
    time = time / 1000;
    if (Task_lisk(State.task)->next->state)
    {
        // printf("...");
        start_btn_change(next, 10 - time);
        if (my_lv_time - State.time_start > Next_Wait_Time * 1000)
        {
            State.task++;
            State.flag = 1;
            Start_timer = lv_timer_create(Start_timer_handler, 5, 0);
            printf("\r\n next");
            lv_timer_del(NEXT_timer);
        }
        if (State.flag == 0)
        {
            start_btn_flash(); //退出等待并刷新按键
            lv_timer_del(NEXT_timer);
        }
    }
    else
    {
        printf("\r\n next_end");
        start_btn_flash();
        State.flag = 0;
        lv_timer_del(NEXT_timer);
    }
}

void INC_timer_handler(void)
{
    if (State.inc_Rec == 0)
    {
        if (State.motor_run == 0 && State.flag == 1)
        {
            State.motor_run = 1;
            State.time_start = my_lv_time;
            Motor_timer = lv_timer_create(Motor_timer_handler, 5, NULL);
            start_btn_flash();
            cam_timer_on_off(1000, 0);                                                      //电机启动后再 启动相机
            printf("\r\n inc->motor %d %d %d", State.flag, State.inc_Rec, State.motor_run);
            lv_timer_del(INC_timer);
        }
        if (State.motor_run == 0 && State.flag == 0)
        {
            // end;
            State.time_start = my_lv_time;
            State.flag = next;
            NEXT_timer = lv_timer_create(NEXT_timer_handler, 100, NULL);
            start_btn_flash();
            // printf("\r\n inc->next %d %d %d", State.flag, State.inc_Rec, State.motor_run);
            lv_timer_del(INC_timer);
        }
        inc_up = 0;
        inc_down = 0;
    }
}

void Emg_stop_handler(void)
{
    start_btn_flash();
    if (State.motor_run == 0 && State.inc_Rec == 0)
        lv_timer_del(Emg_stop);
}

//提供结束服务
void Motor_timer_handler(void)
{
    int limit_time;
    int time = (int)my_lv_time - State.time_start;
    // time = time/1000;
    if (time % 100 == 0)
    {
        start_btn_flash();
    }
    switch (State.mode) //时间计算
    {                   //(float)State.Set_Time / State.Frep_VOR * 1000 - 50;
    case VOR_ID:
        limit_time = (float)State.Set_Time / State.Frep_VOR * 1000 - 50;
        break;
    case VHIT_ID:
        limit_time = State.Set_Time * 5000;
        break;
    default:
        limit_time = State.Set_Time * 1000;
        break;
    }
    if (time > limit_time && State.flag) //超时暂停
    {
        State.flag = 0;
        cam_timer_on_off(1000, 0); //当时间到时停止拍摄    可避免归零还在拍摄的问题（匀速转动的模式）
    }
    if (State.flag == 0 && time < limit_time) //手动停止
    {
        if (State.mode == OVAR_ID)
            State.inc_Rec = 1, State.Frep_VOR = 0;
        else
            State.inc_Rec = 0, State.inc_ang = 0;
        cam_timer_on_off(1000, 0);
        Emg_stop = lv_timer_create(Emg_stop_handler, 100, 0);
        lv_timer_del(Motor_timer);
        return;
    }
    // printf("run");
    // if(State.flag==0)
    // cam_timer_on_off(1000, 0);                                                  //当时间到时停止拍摄    可避免归零还在拍摄的问题（匀速转动的模式）

    if (State.motor_run == 0) //电机完成
    {
        if ((State.mode == OVAR_ID && Task_lisk(State.task)->next->mode != OVAR_ID) || (State.mode == OVAR_ID && Task_lisk(State.task)->next->state == 0)) //如下一为ovar则不下降   除下一项为 不存在
            State.inc_Rec = 1, State.Frep_VOR = 0.0;
        else
            State.inc_Rec = 0, State.inc_ang = 0;
        start_btn_flash();
        INC_timer = lv_timer_create(INC_timer_handler, 5, NULL);
        lv_timer_del(Motor_timer);
    }
}

void Sys_init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置系统中断优先级分组2
    delay_init(168);                                //初始化延时函数
    uart_init(256000);                              //初始化串口波特率为115200
    LED_Init();                                     //初始化LED
    LCD_Init();                                     // LCD初始化
    KEY_Init();                                     //按键初始化
    INC_IO_init();
    ir_sensor_init();
    Cam_LED_Init();
    // IIC_Init(GPIO_Pin_6 | GPIO_Pin_7, GPIOC);

    TIM3_Int_Init(1999, 83); //定时器初始化(1ms中断),用于给lvgl提供1ms的心跳节拍

    FSMC_SRAM_Init(); //初始化外部sram
                      //  tp_dev.init();			//触摸屏初始化
                      // LCD_Init();
    FT5206_Init();
    tp_dev.scan = FT5206_Scan;             //扫描函数指向GT9147触摸屏扫描
    tp_dev.touchtype |= 0X80;              //电容屏
    tp_dev.touchtype |= lcddev.dir & 0X01; //横屏还是竖屏
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
    PCout(6) = 0; //相机led归零
}

void task_display(void)
{
    int i;
    for (i = 1; i < 10; i++)
    {
        // printf("\r\n n:%d  V:%d T:%d F:%f", i, Task_lisk(i)->Vel, Task_lisk(i)->Set_Time, Task_lisk(i)->Frep_VOR);
        lv_obj_align(table_box, LV_ALIGN_CENTER, 150, 0);
        //  for(i=0;i<10;i++)
        // {
        //     printf("\r\n %d %d",i,Task_lisk(i)->state);
        // }
        // printf("s %d",State.list_state);
    }
}

struct
{
    u8 Cmd;
    u8 Mode;
    u16 Time;
    u8 M_Vel;
    u16 M_F;
} U_D; //串口协议

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
    Uart_len = 2; //标志位清楚
    if (U_D.Cmd == 1 && Task_lisk(1)->state == 1)
    {
        State.task = 0; //用于作为启示指示
        State.flag = 1;
        Start_timer = lv_timer_create(Start_timer_handler, 5, 0);
    }
    if (U_D.Cmd == 2)
    {
        State.flag = 0;
    }

    if (U_D.Cmd / 10 == 3)
    {
        //读取
        i = U_D.Cmd % 10;
        Motor_f = U_D.M_F;
        if (i >= 0 && i <= 8) //输入限制
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

void back_timer_handler(void)
{
    static short arr = 0;
    inc_down = 1;
    if (my_lv_time < 3000) //开机启动过快会与制动拖住
    {
        TIM5->CR1 &= ~TIM_CR1_CEN; //关闭定时器
        return;
    }
    if (arr == 0)
        Slave_Motor_Vel_Mode(-30, 0);
    else
        Slave_Motor_Vel_Mode(30, 0);
    LED0 = Ir_IO;
    if (Ir_IO == 0 || my_lv_time > back_S * 1000 && arr == 0) //当到达光电开关时
    {
        TIM1->CNT = motor_Middele; //归中定时器
        arr = 1;
    }
    if (arr && (TIM1->CNT > motor_Middele + 3500)) //三十度
    {
        Slave_Motor_Vel_Mode(0, 0);
        TIM1->CNT = motor_Middele; //归中定时器
        TIM5->CR1 &= ~TIM_CR1_CEN; //关闭定时器
        inc_down = 0;              //倾斜关闭
        lv_obj_clean(lv_scr_act());
        Menu_init();
        State.std_ang = State.inc_ang;
        static lv_style_t font_style1;
        lv_style_init(&font_style1);
        lv_style_set_text_font(&font_style1, &lv_font_montserrat_18);
        update_label = lv_label_create(lv_scr_act());
        lv_obj_add_style(update_label, &font_style1, 0);
        lv_timer_create(time_reflash, 33, NULL);
        lv_timer_create(task_display, 500, NULL);
        lv_timer_del(back_timer);
    }
}

int main(void)
{

    Sys_init();
    if ((RCC->CSR & RCC_CSR_WWDGRSTF) == 0)
        cam_timer_on_off(5000, 1);
    RCC->CSR |= RCC_CSR_RMVF;
    wait_slave_ready_ui();
    back_timer = lv_timer_create(back_timer_handler, 20, 0);
    // TIM1->CNT = motor_Middele - 1;
    // lv_timer_create(host_usart_handler,100,0);
    while (1)
    {
        //       GPIO_ResetBits(GPIOB, GPIO_Pin_2);
        // GPIO_ResetBits(GPIOB, GPIO_Pin_1);

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

unsigned char Send_Count; //串口需要发送的数据个数
// short Acc_Data[4]={0,0,0,0};
short Angle_Data[4] = {0, 0, 0, 0};
// short Angle_Speed[4]={0,0,0,0};
int Acc_R_Data[4] = {0, 0, 0, 0};

void USART3_IRQHandler(void) //串口1中断服务程序
{
    // u8 Res;
    static u8 last_data = 0;
    float temp;
    u8 data = 0;
    static int Data_state = 0,                             //尾帧
        cut = 0,                                           //计数
        Data_Flag = 0;                                     //接收标志位
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //接收中断(接收到的数据必须是0x0d 0x0a结尾)
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
            // case 0X51://加速度计
            // 	{
            // 		if (cut%2==1)
            // 		Acc_Data[(int)cut/2]=(data<<8)|last_data;
            // 	}
            // 	break;
            // case 0X52:
            // 	{
            // 		if(cut%2==1)
            // 		Angle_Speed[(int)cut/2]=(data<<8)|last_data;
            // 	}break;
            case 0X53: //角度输出
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
                    // if (State.inc_ang < 0)
                    //     State.inc_ang = -State.inc_ang;
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
    // lv_deinit();
    // lv_init();
    // lv_port_disp_init();  // lvgl???????,??lv_init()???
    // lv_port_indev_init(); // lvgl???????,??lv_init()???
    // // LCD_Color_Fill(0,0,800,480,)
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
