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
 * 变量�?
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
// 测试�?

void Sys_init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置系统中断优先级分�?2
    delay_init(168);                                // 初始化延时函�?
    uart_init(256000);                              // 初始化串口波特率�?115200
    LED_Init();                                     // 初始化LED
    LCD_Init();                                     // LCD初始�?
    KEY_Init();                                     // 按键初始�?
    INC_IO_init();
    ir_sensor_init();
    Cam_LED_Init();
    // IIC_Init(GPIO_Pin_6 | GPIO_Pin_7, GPIOC);

    TIM3_Int_Init(1999, 83); // 定时器初始化(1ms中断),用于给lvgl提供1ms的心跳节�?

    FSMC_SRAM_Init(); // 初始化外部sram
                      //   tp_dev.init();			//触摸屏初始化
                      //  LCD_Init();
    FT5206_Init();
    tp_dev.scan = FT5206_Scan;             // 扫描函数指向GT9147触摸屏扫�?
    tp_dev.touchtype |= 0X80;              // 电容�?
    tp_dev.touchtype |= lcddev.dir & 0X01; // 横屏还是竖屏
    LCD_Display_Dir(1);

    lv_init();            // lvgl系统初始�?
    lv_port_disp_init();  // lvgl显示接口初始�?,放在lv_init()的后�?
    lv_port_indev_init(); // lvgl输入接口初始�?,放在lv_init()的后�?
    DataScope_DMA_init();
    TIM1_encoder_Init(50000 + 1, 20 - 1);
#if datascope
    TIM7_DataScope_Init(2500, 839);
#endif
    TIM5_Slave_Init(50000, 10);
    Slave_DIR_Init();
    uart3_init(9600);
    TIM4_Motor_Int_Init(999, 83);
    PCout(6) = 0; // 相机led归零
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
        // if (my_lv_time % 10 == 0)
        //     host_usart_handler();
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
