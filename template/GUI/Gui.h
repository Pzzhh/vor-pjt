#ifndef Gui_h
#define Gui_h

#include "../lvgl/lvgl.h"
void Menu_init(void);

struct data1
{
    //按键状态
    uint8_t flag;
    //频率和倾角（公用）
    float Frep_VOR;
    //角速度
    short Vel;
    //设置的时间
    int Set_Time;
    //设置的模式
    short mode;
    //开始时间
    int time_start;
    //电机运转状态
    short motor_run;
    //倾角启动
    short inc_Rec;
    //倾角当前值
    float inc_ang;
    //当前任务 与启动一起被置位0
    short task;
    //链表状态
    short list_state;
    //启动角度
    float std_ang;
    //实际角度
    float act_ang;
    //方向设置
    short dir;
};

void Model_init(void);
void Ovar_init(void);
void Continue_init(void);
void VOR_init(void);
void start_btn_flash(void);
void start_btn_change(short id, int Num);
void M4_init(void);
void wait_slave_ready_ui(void);
int f_int_tran(float k, short num);
#define VOR_ID 1
#define Ctn_ID 2
#define OVAR_ID 3
#define VHIT_ID 4

enum btn_id
{
    stop = 1,
    wait_inc,
    wait_back,
    start = 4,
    next,

};

extern short Dropdown_mode;

extern lv_timer_t *Start_timer;
extern lv_timer_t *flag_sev_timer;
extern lv_timer_t *END_timer;
extern lv_timer_t *NEXT_timer;
extern lv_timer_t *INC_timer;
extern lv_timer_t *Motor_timer;
extern void Start_timer_handler(void);

#endif // Gui_h
