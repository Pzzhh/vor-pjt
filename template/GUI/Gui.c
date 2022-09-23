#include "../GUI/Gui.h"
#include "mytask.h"
#include "Slave_motor.h"
#include "led.h"
#include "wwdg.h"
short Set_time = 1;
lv_obj_t *time_lable;
lv_obj_t *time_box;
extern struct data1 State;
float Dropdown_read_float(lv_obj_t *dropdown);
short Dropdown_read(lv_obj_t *dropdown);
lv_obj_t *dropdown_1;
short Dropdown_mode = 1;
void start_init(void);
extern lv_timer_t *INC_timer;
extern void cam_timer_on_off(int Period);

void Btn_Num_sever(lv_event_t *e)
{

    int add = 0;
    add = (int)(e->user_data);
    lv_event_code_t code = lv_event_get_code(e);
    // lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT)
    {
        if (add == 1)
        {
            lv_spinbox_increment(time_box);
            Set_time = lv_spinbox_get_value(time_box);
        }
        else
        {
            lv_spinbox_decrement(time_box);
            Set_time = lv_spinbox_get_value(time_box);
        }
    }
    // lv_event_code_t code = lv_event_get_code(e);
    // lv_obj_t *obj = lv_event_get_target(e);
    // if (code == LV_EVENT_CLICKED)
    // {
    //     if (e->user_data == (void *)1)
    //     {
    //         if (Set_time < 300)
    //             Set_time += 30;
    //         else
    //             Set_time = 300;
    //         lv_label_set_text_fmt(time_lable, "%ds", Set_time);
    //     }
    //     else
    //     {
    //         if (Set_time > 30)
    //             Set_time -= 30;
    //         else
    //             Set_time = 30;
    //         lv_label_set_text_fmt(time_lable, "%ds", Set_time);
    //     }
    // }
}

static void event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    LV_LOG_USER("Num %d", code);
    if (code == LV_EVENT_VALUE_CHANGED)
    {
        char buf[32];
        lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
        switch (buf[0])
        {
        case 'V':
        {
            if (buf[1] == 'O')
                VOR_init(), Dropdown_mode = VOR_ID;
            else
                M4_init(), Dropdown_mode = VHIT_ID;
        }
        break;
        case 'O':
            Ovar_init(), Dropdown_mode = OVAR_ID;
            break;
        case 'C':
            Continue_init(), Dropdown_mode = Ctn_ID;
            break;
        }
    }
}

void Menu_init(void)
{
    start_init();
    table_init();
    Table_btn_init();

    /**< 1 */ lv_obj_t *label_0 = lv_label_create(lv_scr_act());
    //频率
    /**< 2 */ dropdown_1 = lv_dropdown_create(lv_scr_act());
    //    lv_obj_set_size(dropdown_1,110,40);
    //    lv_obj_set_size(dropdown_1,130,42);
    /**< 3 */ lv_obj_t *dropdown_1_label = lv_label_create(lv_scr_act());
    lv_obj_align(dropdown_1, LV_ALIGN_LEFT_MID, 110, -120);
    //速度
    //开始按键
    static lv_style_t font_style1;
    lv_style_init(&font_style1);
    lv_style_set_text_font(&font_style1, &lv_font_montserrat_18);
    lv_obj_add_style(label_0, &font_style1, 0);

    lv_label_set_text_fmt(label_0, "Mouse vestibular oculomotor reflex test system");
    lv_obj_align(label_0, LV_ALIGN_TOP_MID, 0, 50);
    //频率
    lv_label_set_text_fmt(dropdown_1_label, "Model:");
    lv_obj_align_to(dropdown_1_label, dropdown_1, LV_ALIGN_OUT_LEFT_MID, -5, 0);
    lv_dropdown_set_options(dropdown_1, "VOR\n"
                                        "Continue\n"
                                        "Ovar\n"
                                        "VHIT");
    //数值
    lv_obj_add_event_cb(dropdown_1, event_handler, LV_EVENT_VALUE_CHANGED, NULL);
    Model_init();
}
struct
{
    lv_timer_t *e;
    uint8_t State;
} INC_manual_timer;

void INC_manual_timer_handler(void)
{
    inc_up = 0;
    inc_down = 0;
    // inc_up = 0;
    // inc_down = 0;
    INC_manual_timer.State = 0;
}

void INC_manual_event(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    static int time;
    if (code == LV_EVENT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT)
    {
        if (State.inc_Rec)
        {
            return;
        }
        if (e->user_data == (void *)1)
            // LED0 = 0, LED1 = 1;
            inc_up = 1, inc_down = 0;
        else
            // LED0 = 1, LED1 = 0;
            inc_up = 0, inc_down = 1;
        if (INC_manual_timer.State)
        {
            INC_manual_timer.e->period = 200;
            INC_manual_timer.e->last_run = lv_tick_get(); //每按一次继续运行3秒
            INC_manual_timer.e->repeat_count = 1;         //只运行一次
        }
        else
        {
            INC_manual_timer.State = 1;
            INC_manual_timer.e = lv_timer_create(INC_manual_timer_handler, 200, 0);
            INC_manual_timer.e->repeat_count = 1;
        }
    }
}

void reset_btn_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    static int32_t time;
    lv_obj_t *label = (lv_obj_t *)e->user_data;
    if (code == LV_EVENT_CLICKED)
    {
        // State.std_ang = State.act_ang;
    }
    if (code == LV_EVENT_LONG_PRESSED_REPEAT)
    {
        if (lv_tick_get() - time > 5000)
            time = lv_tick_get();
        // lv_label_set_text_fmt(label,"reset after %ds",6-(lv_tick_get() - time)/1000);
        if (lv_tick_get() - time > 4 * 1000)
            WWDG_Init(4, 150, WWDG_Prescaler_8);
    }
    else
        lv_label_set_text_fmt(label, " ");
}

void cam_btn_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        LED1 = 1;
        cam_timer_on_off(1000);
    }
}

lv_obj_t *label_F;
lv_obj_t *label_V;
lv_obj_t *dropdown_F_V;
lv_obj_t *dropdown_V;

void Model_init(void)
{

    /**< 频率 */ label_F = lv_label_create(lv_scr_act());
    dropdown_F_V = lv_dropdown_create(lv_scr_act());
    lv_obj_align(dropdown_F_V, LV_ALIGN_LEFT_MID, 110, 120);
    lv_obj_align_to(label_F, dropdown_F_V, LV_ALIGN_OUT_LEFT_MID, -25, 0);

    //小模式初始化（不可移动初始化在上面）
    VOR_init();

    /*end*/

    /**< 速度 */ label_V = lv_label_create(lv_scr_act());
    dropdown_V = lv_dropdown_create(lv_scr_act());
    lv_obj_align(dropdown_V, LV_ALIGN_LEFT_MID, 110, 40);
    lv_obj_align_to(label_V, dropdown_V, LV_ALIGN_OUT_LEFT_MID, -25, 0);
    lv_label_set_text_fmt(label_V, "Vel:");

    lv_dropdown_set_options(dropdown_V, "5/s\n"
                                        "10/s\n"
                                        "20/s\n"
                                        "40/s\n"
                                        "60/s\n"
                                        "80/s");

    /*end*/

    /**< 时间 */ time_box = lv_spinbox_create(lv_scr_act());

    lv_spinbox_set_value(time_box, State.Set_Time);
    lv_spinbox_set_range(time_box, 1, 1000);     //幅度限制
    lv_spinbox_set_digit_format(time_box, 4, 0); //整数两位
    lv_spinbox_step_prev(time_box);
    lv_obj_set_width(time_box, 100);
    lv_obj_t *time_box_lable = lv_label_create(lv_scr_act());
    // time_lable = lv_label_create(time_box);
    // lv_obj_align(time_lable, LV_ALIGN_LEFT_MID, -10, 0);
    lv_obj_set_size(time_box, 130, 42);
    lv_obj_align(time_box, LV_ALIGN_LEFT_MID, 110, -40);
    lv_obj_align_to(time_box_lable, time_box, LV_ALIGN_OUT_LEFT_MID, -25, 0);
    lv_obj_clear_flag(time_box, LV_OBJ_FLAG_SCROLLABLE);
    // btn
    lv_obj_t *time_btn_m = lv_btn_create(lv_scr_act()); //加
    lv_obj_t *time_btn_a = lv_btn_create(lv_scr_act()); //减
    lv_obj_set_size(time_btn_m, 40, 40);
    lv_obj_set_size(time_btn_a, 40, 40);
    lv_obj_align_to(time_btn_m, time_box, LV_ALIGN_OUT_RIGHT_MID, 65, 0);
    lv_obj_align_to(time_btn_a, time_box, LV_ALIGN_OUT_RIGHT_MID, 15, 0);
    // set
    lv_label_set_text_fmt(time_box_lable, "Time");
    lv_label_set_text_fmt(time_lable, "30s");
    lv_obj_set_style_bg_img_src(time_btn_a, LV_SYMBOL_PLUS, 0);
    lv_obj_set_style_bg_img_src(time_btn_m, LV_SYMBOL_MINUS, 0);
    // btn event
    lv_obj_add_event_cb(time_btn_a, Btn_Num_sever, LV_EVENT_ALL, (void *)1);
    lv_obj_add_event_cb(time_btn_m, Btn_Num_sever, LV_EVENT_ALL, (void *)0);
    /*end*/
    // inc btn      倾斜按键
    lv_obj_t *inc_btn_label = lv_label_create(lv_scr_act());
    lv_obj_t *btn_up = lv_btn_create(lv_scr_act());   //加
    lv_obj_t *btn_down = lv_btn_create(lv_scr_act()); //减
    lv_obj_set_size(btn_up, 40, 40);
    lv_obj_set_size(btn_down, 40, 40);
    lv_obj_align(btn_up, LV_ALIGN_BOTTOM_LEFT, 160, -30);
    lv_obj_align_to(inc_btn_label, btn_up, LV_ALIGN_OUT_TOP_MID, 30, -7);
    lv_obj_align_to(btn_down, btn_up, LV_ALIGN_OUT_RIGHT_MID, 20, 0);

    lv_label_set_text(inc_btn_label, "INC ");
    lv_obj_set_style_bg_img_src(btn_up, LV_SYMBOL_UP, 0);
    lv_obj_set_style_bg_img_src(btn_down, LV_SYMBOL_DOWN, 0);
    // btn event
    lv_obj_add_event_cb(btn_up, INC_manual_event, LV_EVENT_ALL, (void *)1);
    lv_obj_add_event_cb(btn_down, INC_manual_event, LV_EVENT_ALL, (void *)0);
    /*end*/
    // reset btn         复位按键
    // lv_obj_t *reset_btn_label = lv_label_create(lv_scr_act());
    // lv_obj_t *reset_btn = lv_btn_create(lv_scr_act()); //减
    // lv_obj_set_size(reset_btn, 40, 40);
    // lv_obj_align(reset_btn, LV_ALIGN_BOTTOM_LEFT, 20, -30);
    // lv_obj_align_to(reset_btn_label, reset_btn, LV_ALIGN_OUT_TOP_MID, 0, -20);

    // lv_label_set_text(reset_btn_label, " ");
    // lv_obj_set_style_bg_img_src(reset_btn, LV_SYMBOL_REFRESH, 0);
    // // btn event
    // lv_obj_add_event_cb(reset_btn, reset_btn_handler, LV_EVENT_ALL, (void *)reset_btn_label);
    // end
    // cam btn         复位按键
    // lv_obj_t *cam_btn_label = lv_label_create(lv_scr_act());
    // lv_obj_t *cam_btn = lv_btn_create(lv_scr_act()); //减
    // lv_obj_set_size(cam_btn, 40, 40);
    // lv_obj_align(cam_btn, LV_ALIGN_BOTTOM_LEFT, 100, -30);
    // lv_obj_align_to(cam_btn_label, cam_btn, LV_ALIGN_OUT_TOP_MID, 0, -7);

    // lv_label_set_text(cam_btn_label, "cam ");
    // lv_obj_set_style_bg_img_src(cam_btn, LV_SYMBOL_PAUSE, 0);
    // // btn event
    // lv_obj_add_event_cb(cam_btn, cam_btn_handler, LV_EVENT_ALL, (void *)cam_btn_label);
    // end
}
void VOR_init()
{
    lv_obj_clear_flag(label_F, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(dropdown_F_V, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(label_V, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(dropdown_V, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text_fmt(label_F, "Frep:");
    lv_dropdown_set_options(dropdown_F_V, "0.1HZ\n"
                                          "0.2HZ\n"
                                          "0.5HZ\n"
                                          "0.8HZ\n"
                                          "1.0HZ\n"
                                          "1.6HZ\n"
                                          "3.2HZ\n"
                                          "5.0HZ\n"
                                          "6.4HZ\n"
                                          "7.2HZ\n"
                                          "8.0HZ\n"
                                          "10HZ");
}

void Continue_init()
{
    lv_obj_add_flag(label_F, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(dropdown_F_V, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(label_V, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(dropdown_V, LV_OBJ_FLAG_HIDDEN);
}

void Ovar_init()
{
    lv_obj_clear_flag(label_F, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(dropdown_F_V, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(label_V, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(dropdown_V, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text_fmt(label_F, "Ovar:");
    lv_dropdown_set_options(dropdown_F_V, "17\n"
                                          "30\n"
                                          "45");
}

void M4_init(void)
{
    lv_obj_add_flag(label_F, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(dropdown_F_V, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(label_V, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(dropdown_V, LV_OBJ_FLAG_HIDDEN);
}

short Dropdown_read(lv_obj_t *dropdown)
{
    char buf[20];
    int i;
    short num = 0;
    memset(buf,0,20);
    lv_dropdown_get_selected_str(dropdown, buf, 20);
    for (i = 0; i < 3; i++)
    {
        if (buf[i] <= '9' && buf[i] >= '0')
            num = 10 * num + (buf[i] - 48);
    }
    LV_LOG_USER("\r\n %d", num);
    return num;
}
lv_obj_t *lable_start;
lv_obj_t *btn_start;
void Start_btn_sever(lv_event_t *e)
{
    lv_obj_t *btn_last = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *label = lv_obj_get_child(btn_last, 0);

    if (code == LV_EVENT_CLICKED)
    {
        lv_obj_t *dropdown1 = lv_obj_get_child(lv_scr_act(), 2);
        lv_obj_t *dropdown2 = lv_obj_get_child(lv_scr_act(), 4);
        if (State.inc_Rec)
        {
            State.flag = 0;
            State.inc_Rec = 0;
            State.task = 8;
            start_btn_flash();
            return;
        }
        if (State.motor_run == 0 || State.flag)
            State.flag = !State.flag;
        if (State.flag == 1 && State.list_state == 1)
        {
            State.task = 0;
            Start_timer = lv_timer_create(Start_timer_handler, 5, 0);
            // State.Frep_VOR = Dropdown_read_float(dropdown_F_V);
            // State.Vel = Dropdown_read(dropdown_V);
            // State.Set_Time = Set_time;
        }
        if (State.flag == next)
            State.flag = 0;
        LV_LOG_USER("%d", State.flag);
        start_btn_flash();
    }
}
void start_btn_flash(void)
{
    //    int temp= lv_tick_get()-start_time,tempb =State.Set_Time ;
    if (State.flag == 1 && (State.motor_run == 1))
    {
        // if (State.Set_Time*1000 + start_time - lv_tick_get() <0)
        // return;

        lv_label_set_text_fmt(lable_start, "#000000 Stop#");
        lv_obj_set_style_bg_color(btn_start, lv_color_hex(0xFF0000), 0); //红
    }
    else
    {
        if (State.flag == 0 && State.motor_run != 0 || State.inc_Rec)
        {
            if (State.inc_Rec)
                lv_label_set_text(lable_start, "#000000 Wait#\n #000000 INC#");
            else
                lv_label_set_text(lable_start, "#000000 Wait#");
            lv_obj_set_style_bg_color(btn_start, lv_color_hex(0xFFFF00), 0); //绿
        }
        else
        {
            lv_label_set_text(lable_start, "#000000 Start#");
            lv_obj_set_style_bg_color(btn_start, lv_color_hex(0x7CFC00), 0); //绿
        }
    }
}

void start_btn_change(short id, int Num)
{
    //    int temp= lv_tick_get()-start_time,tempb =State.Set_Time ;
    switch (id)
    {
    case 1:
    {
        lv_label_set_text_fmt(lable_start, "#000000 Stop#");
        lv_obj_set_style_bg_color(btn_start, lv_color_hex(0xFF0000), 0); //红
    }
    break;
    case 2:
    {
        lv_label_set_text(lable_start, "#000000 Wait#\n #000000 INC#");
        lv_obj_set_style_bg_color(btn_start, lv_color_hex(0xFFFF00), 0); //黄
    }
    break;
    case 3:
    {
        lv_label_set_text(lable_start, "#000000 Wait#");
        lv_obj_set_style_bg_color(btn_start, lv_color_hex(0xFFFF00), 0); //黄
    }
    break;
    case 4:
    {
        lv_label_set_text(lable_start, "#000000 Start#");
        lv_obj_set_style_bg_color(btn_start, lv_color_hex(0x7CFC00), 0); //绿色
    }
    break;
    case 5:
    {
        lv_label_set_text_fmt(lable_start, "#000000 %ds Next#", Num);
        lv_obj_set_style_bg_color(btn_start, lv_color_hex(0XFFF000), 0); //绿色
    }
    break;
    default:
        break;
    }
    // if (State.flag == 1 && (State.motor_run == 1))
    // {
    //     // if (State.Set_Time*1000 + start_time - lv_tick_get() <0)
    //     // return;
    // }
    // else
    // {
    //     if (State.flag == 0 && State.motor_run != 0 || State.inc_Rec)
    //     {
    //         if (State.inc_Rec)

    //         else

    //         lv_obj_set_style_bg_color(btn_start, lv_color_hex(0xFFFF00), 0); //绿
    //     }
    //     else
    //     {
    //         lv_label_set_text(lable_start, "#000000 Start#");
    //         lv_obj_set_style_bg_color(btn_start, lv_color_hex(0x7CFC00), 0); //绿
    //     }
    // }
}

// lv_obj_t *label_1;

/** \brief 开始按键初始化函数
 */

void start_init(void)
{
    /**< 11 */ btn_start = lv_btn_create(lv_scr_act());

    lable_start = lv_label_create(btn_start);
    lv_label_set_recolor(lable_start, 1);
    lv_label_set_text(lable_start, "#000000 Start#");
    lv_obj_set_size(btn_start, 80, 80);
    lv_obj_set_style_radius(btn_start, 35, 0);
    lv_obj_align(lable_start, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align(btn_start, LV_ALIGN_TOP_RIGHT, -50, 0);
    lv_obj_set_style_bg_color(btn_start, lv_color_hex(0x7CFC00), 0);
    lv_obj_add_event_cb(btn_start, Start_btn_sever, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn_start, Start_btn_sever, LV_EVENT_DRAW_MAIN_BEGIN, NULL);
    //    btn_last=btn_start;
}

float Dropdown_read_float(lv_obj_t *dropdown)
{
    char buf[10];
    float num = 0, f = 1;
    int i = 1;
    short flag = 0;
    memset(buf,0,10);
    lv_dropdown_get_selected_str(dropdown, buf, 10);
    for (i = 0; i < 5; i++)
    {
        if (buf[i] == '.')
            flag = 1;
        if (buf[i] <= '9' && buf[i] >= '0' && flag == 0)
            num = 10 * num + (buf[i] - 48);
        if (buf[i] <= '9' && buf[i] >= '0' && flag == 1)
        {
            f = f * 0.1;
            num = num + (buf[i] - 48) * f;
        }
    }
    printf("/r/n drop %f :%s", num,buf);
    return num;
}

static void set_angle(void *obj, int32_t v)
{

    unsigned short i = v - 50;
    if (v < 50)
        i = 0;
    if (v > 720)
        v = 720;
    lv_arc_set_angles(obj, i, v);
}

void wait_slave_ready_ui(void)
{
    lv_obj_t *arc = lv_arc_create(lv_scr_act());
    lv_arc_set_rotation(arc, 270);
    lv_arc_set_bg_angles(arc, 0, 360);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);  /*Be sure the knob is not displayed*/
    lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE); /*To not allow adjusting by click*/
    lv_obj_center(arc);

    //    lv_arc_set_start_angle(arc,100);
    lv_arc_set_value(arc, 120);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, arc);
    lv_anim_set_exec_cb(&a, set_angle);
    lv_anim_set_time(&a, 8000);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE); /*Just for the demo*/
    lv_anim_set_repeat_delay(&a, 0);
    lv_anim_set_values(&a, 0, 770);
    lv_anim_start(&a);
}
// float_int_transfer
int f_int_tran(float k, short num)
{
    int res = k * (num) - (int)k * num;
    if (res < 0)
        res = -res;
    return res;
}