#ifndef _Motor_Control_H
#define _Motor_Control_H

#include "stm32f4xx.h"
#include "sys.h"
#include "math.h"
#include "lvgl.h"
#define datascope 0
#define back_S 15         // 15s
#define Next_Wait_Time 10 // 下一任务等待时间 for Next_timer_handler
#define table_del_ani 1
void ui_motor_sever_init(void);

#endif
