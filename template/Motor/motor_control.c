#include "../Motor/motor_control.h"
#include "led.h"
#include "Gui.h"
#include "Slave_motor.h"
#include "mytask.h"
#include "list_c.h"
extern struct data1 State;
struct
{
    lv_timer_t *e;
    short vail;
} cam_timer;

void cam_timer_on_off(int Period, int flag)
{
    extern void camer_handler(lv_timer_t * e);
    if (cam_timer.e->repeat_count != 0 && cam_timer.vail == 1)
    {
        // printf("\r\n Add Next_repeat %d", cam_timer.e->repeat_count);
        cam_timer.e->repeat_count += 2;
        // printf("\r\n Add Next_repeat %d", cam_timer.e->repeat_count);
    }
    else
    {
        cam_timer.vail = 1;
        cam_timer.e = lv_timer_create(camer_handler, Period, (void *)flag); // 时间启动
        cam_timer.e->repeat_count = 2;                                      // 设置重复
        // cam_timer.e->last_run=my_lv_time+10*1000;
        lv_timer_ready(cam_timer.e); // 就绪
        // printf("\r\n New on");
    }
}
typedef enum
{
    idle = 0,
    cmd_on,
    cmd_off
} motor_info;

motor_info motor_event_monitor(void)
{
    motor_info rec = 0;
    if (State.run == 1)
    {
        if (State.btn_clicked)
            rec = cmd_off;
        if (State.uart_cmd == 2)
            rec = cmd_off;
    }
    if (State.run == 0)
    {
        if (State.btn_clicked)
            rec = cmd_on;
        if (State.uart_cmd == 1)
            rec = cmd_on;
    }
    State.uart_cmd = 0;
    btn_reset_val;
    return rec;
}

void task_display(void)
{
    extern lv_obj_t *table_box;
    int i;
    for (i = 1; i < 10; i++)
    {
        lv_obj_align(table_box, LV_ALIGN_CENTER, 150, 0);
    }
}
void time_reflash(lv_event_t *e)
{
    lv_obj_t *update_label = (lv_obj_t *)e->user_data;
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
    table_arrange_auto();
#endif
    lv_label_set_text_fmt(update_label, " Ang:%d.%.1d REC:%s \r %.2d:%.2d:%.2d", (int)State.inc_ang, f_int_tran(State.inc_ang, 10), (State.cam_state ? "ON" : "OFF"), time / 3600 % 24, time / 60 % 60, time % 60);
}

void camer_handler(lv_timer_t *e)
{
    static uint8_t rec;
    LED0 = !LED0;
    if (e->user_data == (void *)1)
    {
        Cam_set = !Cam_set;
    }
    else
    {
        Cam_rec = !Cam_rec;
        rec++;
        if (rec == 1)
            State.cam_state = !State.cam_state;
        if (rec == 2)
        {
            rec = 0;
        }
    }
    // printf("\r\n timer %d re %d", PFout(10), e->repeat_count);
    if (e->repeat_count == 0)
        cam_timer.vail = 0;
}

void back_timer_handler(lv_timer_t *e)
{
    static short arr = 0;
    extern void host_usart_handler(void);
    inc_down = 1;
    if (my_lv_time < 3000) // 开机启动过快会与制动拖住
    {
        TIM5->CR1 &= ~TIM_CR1_CEN; // 关闭定时器
        return;
    }
    if (arr == 0)
        Slave_Motor_Vel_Mode(-30, 0);
    else
        Slave_Motor_Vel_Mode(30, 0);
    LED0 = Ir_IO;
    if (Ir_IO == 0 || my_lv_time > back_S * 1000 && arr == 0) // 当到达光电开关时
    {
        TIM1->CNT = motor_Middele; // 归中定时器
        arr = 1;
    }
    if (arr && (TIM1->CNT > motor_Middele + 3500)) // 三十度
    {
        Slave_Motor_Vel_Mode(0, 0);
        TIM1->CNT = motor_Middele; // 归中定时器
        TIM5->CR1 &= ~TIM_CR1_CEN; // 关闭定时器
        inc_down = 0;              // 倾斜关闭
        lv_obj_clean(lv_scr_act());
        Menu_init();
        State.std_ang = State.inc_ang;
        static lv_style_t font_style1;
        lv_style_init(&font_style1);
        lv_style_set_text_font(&font_style1, &lv_font_montserrat_18);
        lv_obj_t *update_label = lv_label_create(lv_scr_act());
        lv_obj_add_style(update_label, &font_style1, 0);
        lv_timer_create(time_reflash, 33, (void *)update_label);
        lv_timer_create(task_display, 500, NULL);
        lv_timer_create(host_usart_handler, 50, NULL);
        lv_timer_del(e);
    }
}

void Emg_stop_handler(lv_timer_t *e)
{
    extern void Start_timer_handler(lv_timer_t * e);
    static uint32_t my_time = 0;
    static uint8_t flag;
    uint32_t wait_time = 10 * 1000;
    btn_reset_val;
    if (e->user_data != (void *)0)
    {
        if (e->user_data == (void *)1) // 定时10s功能
            my_time = my_lv_time;
        if (State.mode == OVAR_ID)
            State.inc_Rec = 1, State.Frep_VOR = 0;
        else
            State.inc_Rec = 0, State.inc_ang = 0;
        e->user_data = 0;
        flag = 1;
    }
    start_btn_change(bt_wait_back, 0);
    if (my_lv_time - my_time > wait_time && flag == 1)
    {
        if (State.cam_state == 1)
        {
            cam_timer_on_off(1000, 0);
            printf("\r\n Rec_termination");
            flag = 0;
        }
    }
    if (State.motor_run == 0 && State.inc_Rec == 0 && State.cam_state == 0)
    {
        start_btn_change(bt_start, 0);
        lv_timer_create(Start_timer_handler, 300, (void *)1); // 启动timer
        lv_timer_del(e);
    }
    // start_btn_flash();
}

// 提供结束服务
volatile void Motor_timer_handler(lv_timer_t *e)
{
    extern void INC_timer_handler(lv_timer_t * e);
    uint32_t wait_time = 4 * 1000;
    int limit_time;
    int time = my_lv_time;
    static volatile uint32_t my_time;
    static uint8_t _wait;
    if (e->user_data == (void *)1) // 代表被第一次调用
    {
        const char ModeWord[][5] = {"VOR", "CON", "OVOR", "VHIT", "TC"};
        printf("\r\n %s %.1fhz %dd %ds",
               ModeWord[State.mode - 1],
               State.Frep_VOR,
               State.Vel,
               State.Set_Time);
        cam_timer_on_off(1000, 0);  //CAM_ON
        start_btn_change(bt_stop, 0);
        my_time = my_lv_time;
        _wait = 1;
        e->user_data = 0;
    }
    motor_info event = motor_event_monitor();
    if (event == cmd_off) // 按键监听
    {
        // btn_reset_val;
        State.flag = 0;
        if (my_lv_time - my_time > wait_time)
            lv_timer_create(Emg_stop_handler, 100, (void *)1); // 提供10s延迟
        else
            lv_timer_create(Emg_stop_handler, 100, (void *)2); // 没有10s延迟
        lv_timer_del(e);
        return;
    }
    if (my_lv_time - my_time > wait_time && _wait)
    {
        State.time_start = my_lv_time;
        State.motor_run = 1;
        _wait = 0;
        start_btn_change(bt_run, 0);
    }

    if (_wait == 0)
    {
        time = (int)my_lv_time - State.time_start;
        switch (State.mode) // 时间计算
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
        if (time > limit_time && State.flag) // 超时暂停
        {
            State.flag = 0;
        }
        if (State.motor_run == 0) // 电机完成
        {
            if ((State.mode == OVAR_ID && Task_lisk(State.task)->next->mode != OVAR_ID) || (State.mode == OVAR_ID && Task_lisk(State.task)->next->state == 0)) // 如下一为ovar则不下降   除下一项为 不存在
                State.inc_Rec = 1, State.Frep_VOR = 0.0;
            else
                State.inc_Rec = 0, State.inc_ang = 0;
            start_btn_flash();
            lv_timer_create(INC_timer_handler, 5, NULL);
            lv_timer_del(e);
        }
    }
}

void next_moter_set_all(void)
{
    extern void (*CustomFuntion)(float Vel);
    extern void Slave_Motor_Vel_Mode_Acc(float Vel);
    extern int Slave_Motor_Vel_Mode_Acc_Step;

    State.mode = Task_lisk(State.task)->mode;
    State.Vel = Task_lisk(State.task)->Vel;
    State.Frep_VOR = Task_lisk(State.task)->Frep_VOR;
    State.Set_Time = Task_lisk(State.task)->Set_Time;
    // printf("\r\n %d %d %d %d", Task_lisk(0)->mode, Task_lisk(1)->mode, Task_lisk(2)->mode, Task_lisk(3)->mode);

    if (State.mode == OVAR_ID)
    {
        State.inc_Rec = 1;
        if (State.Vel == 60)
        {
            Slave_Motor_Vel_Mode_Acc_Step = 0;
            CustomFuntion = Slave_Motor_Vel_Mode_Acc;
        }
    }
    else
        State.inc_Rec = 0;
    lv_timer_create(INC_timer_handler, 5, NULL);
    // printf("\r\n Start End");
}

void next_motortask_timer_handler(lv_timer_t *e)
{
    static uint32_t my_time;
    if (State.flag)
    {
        State.flag = 0;
        my_time = my_lv_time;
        start_btn_change(bt_stop, 0);
    }
    if (motor_event_monitor() == cmd_off)
    {
        lv_timer_del(e); //
        start_btn_change(bt_start, 0);
        // btn_reset_val;
    }
    if (my_lv_time - my_time > 4 * 1000)
    {
        State.flag = 1;
    }
    if (State.flag)
    {
        next_moter_set_all();
        lv_timer_del(e); //
    }
    // start_btn_change(bt_stop, 0);
}

void NEXT_timer_handler(lv_timer_t *e)
{
    extern void Start_timer_handler(lv_timer_t * e);
    int time = (int)my_lv_time - State.time_start;
    time = time / 1000;
    static uint32_t my_time;
    // static uint8_t inc_standby=0;
    if (motor_event_monitor() == cmd_off)
    {
        State.flag = 0;
        btn_reset_val;
    }
    if (State.flag == 0)
    {
        cam_timer_on_off(1000, 0);
        start_btn_change(bt_start, 0);
        lv_timer_create(Start_timer_handler, 300, (void *)1); // 启动timer
        lv_timer_del(e);
        return;
    }
    if (my_lv_time - State.time_start > Next_Wait_Time * 1000 || State.mode == TC_ID) // 超时
    {
        start_btn_change(bt_wait_inc, 0);
        if (Task_lisk(State.task)->next->state)
        {
            if (State.cam_state)
                cam_timer_on_off(1000, 0);
            if (State.inc_Rec == 0 || State.mode != OVAR_ID)
            {
                State.task++;
                State.flag = 1;
                // State.btn_clicked = 1; // 触发按键
                lv_timer_create(next_motortask_timer_handler, 300, 0);
                start_btn_change(bt_stop, 0); // 还有任务所以要显示停止
                printf("\r\n Rec_finish");
                // printf("\r\n Rec_end");
                lv_timer_del(e);
                return;
            }
        }
        else
        {
            if (State.cam_state)
                cam_timer_on_off(1000, 0); // 关闭相机
            if (State.inc_Rec == 0 || State.mode != OVAR_ID)
            {
                printf("\r\n Rec_finish");
                lv_timer_create(Start_timer_handler, 300, (void *)1);
                start_btn_change(bt_start, 0); // 没有任务所以要显示启动
                start_btn_flash();
                State.flag = 0;
                lv_timer_del(e);
                return;
            }
        }
    }
    else
        start_btn_change(bt_next, 10 - time);
}

void INC_timer_handler(lv_timer_t *e)
{
    e->period = 5;
    static uint32_t my_time;
    if (motor_event_monitor() == cmd_off)
    {
        lv_timer_create(Emg_stop_handler, 5, (void *)2);
        btn_reset_val;
        lv_timer_del(e);
        return;
    }
    if (State.inc_Rec == 0)
    {
        if (State.motor_run == 0 && State.flag == 1)
        {
            // State.motor_run = 1;
            // State.time_start = my_lv_time;
            lv_timer_create(Motor_timer_handler, 5, (void *)1);
            // start_btn_flash();
            start_btn_change(bt_stop, 0);
            // printf("\r\n inc->motor %d %d %d", State.flag, State.inc_Rec, State.motor_run);
            lv_timer_del(e);
        }
        inc_up = 0;
        inc_down = 0;
    }
    if (State.motor_run == 0 && State.flag == 0)
    {
        // end;
        State.time_start = my_lv_time;
        State.flag = bt_next;
        lv_timer_create(NEXT_timer_handler, 100, NULL);
        // printf("\r\n inc->next %d %d %d", State.flag, State.inc_Rec, State.motor_run);
        lv_timer_del(e);
    }

    // else
    // {
    //     my_time = my_lv_time;
    // }
    start_btn_change(bt_wait_inc, 0);
}

void Start_timer_handler(lv_timer_t *e)
{
    static uint8_t _start = 0;
    if (e->user_data == (void *)1)
    {
        State.flag = 0;
        State.motor_run = 0;
        State.inc_Rec = 0;
        State.run = 0;
        e->user_data = 0;
        if (State.cam_state == 1)
            cam_timer_on_off(1000, 0);
    }
    if (motor_event_monitor() == cmd_on)
    {
        if (State.list_state == 1) // 检查有没有任务
        {
            State.run = 1;
            State.flag = 1;
        }
        btn_reset_val;
    }
    if (State.flag)
    {
        State.task = 1;
        next_moter_set_all();
        lv_timer_del(e); //
    }
}

void ui_motor_sever_init(void)
{
    wait_slave_ready_ui();
    lv_timer_create(back_timer_handler, 20, 0);
    lv_timer_create(Start_timer_handler, 300, (void *)1);
}