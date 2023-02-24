#ifndef Gui_h
#define Gui_h

#include "../lvgl/lvgl.h"
void Menu_init(void);

// typedef enum
// {
//     flag_idle,
//     flag_inc,
//     flag_

// }
typedef enum 
{
    bt_stop = 1,
    bt_wait_inc,
    bt_wait_back,
    bt_start = 4,
    bt_next,
    bt_run,
}btn_id;

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
    //����״̬
    uint8_t btn_clicked;
    uint8_t uart_cmd;
    uint8_t cam_state;
    uint8_t run;

};

void Model_init(void);
void Ovar_init(void);
void Continue_init(void);
void VOR_init(void);
void start_btn_flash(void);
void start_btn_change(btn_id id, int Num);
void M4_init(void);
void wait_slave_ready_ui(void);
int f_int_tran(float k, short num);
#define VOR_ID 1
#define Ctn_ID 2
#define OVAR_ID 3
#define VHIT_ID 4



extern short Dropdown_mode;

extern lv_timer_t *Start_timer;
extern lv_timer_t *flag_sev_timer;
extern lv_timer_t *END_timer;
extern lv_timer_t *NEXT_timer;
extern lv_timer_t *INC_timer;
extern lv_timer_t *Motor_timer;
#define btn_reset_val State.btn_clicked=0;

#endif // Gui_h
