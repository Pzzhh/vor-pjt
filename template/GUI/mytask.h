#ifndef __mytask_h
#define __mytask_h
#include "lvgl.h"
#include "list_c.h"

int table_get_cnt(int Num);
void Table_btn_init(void);
void Table_list_add(void);
void table_set(int flag, int Num);
void table_hidden(int i);
void table_arrange(void);
int table_get_cnt(int Num);
link *Task_lisk(int num);
void table_init(void);
void Table_reflush(lv_obj_t *table,link *e);
void table_arrange_auto();
#define TASK_Num 14
#define list_Create 1
#define list_Delate 2
#define list_save 3

#endif