#ifndef Gui_h
#define Gui_h

#include "../lvgl/lvgl.h"
void Menu_init(void);

struct data1
{
    //����״̬
    uint8_t flag;
    //Ƶ�ʺ���ǣ����ã�
    float Frep_VOR;
    //���ٶ�
    short Vel;
    //���õ�ʱ��
    int Set_Time;
    //���õ�ģʽ
    short mode;
    //��ʼʱ��
    int time_start;
    //�����ת״̬
    short motor_run;
    //�������
    short inc_Rec;
    //��ǵ�ǰֵ
    float inc_ang;
    //��ǰ���� ������һ����λ0
    short task;
    //����״̬
    short list_state;
    //�����Ƕ�
    float std_ang;
    //ʵ�ʽǶ�
    float act_ang;
    //��������
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
