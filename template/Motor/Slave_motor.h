#ifndef _Slave_motor_H
#define _Slave_motor_H

#include "stm32f4xx.h"
#include "sys.h"
#include "math.h"
#define DIR_Pin
#define Ir_IO PCin(4)
#define motor_Middele 25000
#define inc_up      PCout(0)
#define inc_down    PCout(1)
#define cam_led     PCout(6)

void TIM1_encoder_Init(u32 arr, u32 psc);
void TIM4_Motor_Int_Init(u16 arr, u16 psc);
void TIM7_DataScope_Init(u16 arr, u16 psc);
void IO_init(void);
void EXTIX_Init(void);
void tim_f_set(int f);
void Dir_Ctl(u8 i);
u8 Slave_Back(int Tag_Pos);
void Slave_motor(int Angle_speed, float Feq);
void Slave_DIR_Init(void);
void TIM5_Slave_Init(u32 arr, u32 psc);
void TIM5_Slave_Set(u32 arr, u32 psc);
u8 slave_sin_back(void);
void Slave_Motor_Vel_Mode(float Vel, u8 clear_fg);
u8 INC_Ctrl(float tag);
void VOR_handler(void);
void Continue_handler(void);
void motor_handler(void);
void OVAR_handler(void);
void INC_IO_init(void);
void mode4(void);
u8 Slave_Back_spd(int Tag_Pos,int spd);
void VHIT_handler(void);
void ir_sensor_init(void);
void tim_f_sin_set(int f);

#endif
