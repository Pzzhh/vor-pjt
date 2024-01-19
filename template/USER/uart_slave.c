#include "uart_slave.h"
#include "math.h"
#include "stm32f4xx.h"
const u8 SendPackageTemplete[] = {[0] = 0xfe, [1] = 0xfd, [11] = 0xfc};
#define Use_Monitor 0
struct
{
    u8 Cmd;
    u8 Mode;
    u16 Time;
    u8 M_Vel;
    u16 M_F;
} U_D; // 串口协议
extern struct data1 State;

int k;
void usart_data_send(int i, u8 ReadID)
{
    u8 send_buff[11] = {0XFE, 0XFD};
    short temp;
    memset(&send_buff[2], 0, 8);
    k = i;
    send_buff[2] = ReadID;
    send_buff[3] = i;
    if (Task_lisk(i)->state)
    {
        send_buff[4] = Task_lisk(i)->mode;
        send_buff[5] = Task_lisk(i)->Set_Time / 256;
        send_buff[6] = Task_lisk(i)->Set_Time;
        send_buff[7] = Task_lisk(i)->Vel;
        temp = Task_lisk(i)->Frep_VOR * 10;
        send_buff[8] = temp / 256;
        send_buff[9] = temp;
    }

    send_buff[10] = 0XFC;
    for (i = 0; i < 11; i++)
    {
        while ((USART1->SR & USART_SR_TXE) == 0)
            ;
        USART1->DR = send_buff[i];
    }
}

void Usart_Send_Bytes(u8 *bytes, u16 length)
{
    int i = 0;
    u8 send_buff[11] = {0XFE, 0XFD};
    send_buff[10] = 0XFC;
    short temp;
    memset(&send_buff[2], 0, 8);
    memcpy(&send_buff[2], bytes, length);

    for (i = 0; i < 10; i++)
    {
        while ((USART1->SR & USART_SR_TXE) == 0)
            ;
        USART1->DR = send_buff[i];
    }
}

void usart_data_send_task_num(u8 Num)
{
    int i = 0;
    u8 send_buff[10] = {0XFE, 0XFD};
    short temp;
    // memset(&send_buff[2], 0, 8);
    send_buff[2] = Num;

    send_buff[3] = 0;
    send_buff[4] = 0;
    send_buff[5] = 0;
    send_buff[6] = 0;
    // temp = Task_lisk(i)->Frep_VOR * 10;
    send_buff[7] = 0;
    send_buff[8] = 0;

    send_buff[9] = 0XFC;
    for (i = 0; i < 10; i++)
    {
        while ((USART1->SR & USART_SR_TXE) == 0)
            ;
        USART1->DR = send_buff[i];
    }
}

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
    Uart_len = 2; // 标志位清�?
    if (U_D.Cmd == 1 && Task_lisk(1)->state == 1)
    {
        State.uart_cmd = 1;
        //        State.task = 0; // 用于作为启示指示
        //        State.flag = 1;
        // Start_timer = lv_timer_create(Start_timer_handler, 5, 0);
    }
    if (U_D.Cmd == 2)
    {
        State.uart_cmd = 2;
    }

    if (U_D.Cmd / 0x10 == 3)
    {
        // 读取
        i = U_D.Cmd % 0x10;
        Motor_f = U_D.M_F;
        if (i >= 0 && i <= 8) // 输入限制
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
    if (U_D.Cmd / 0x10 == 4)
    {
        i = U_D.Cmd % 0x10;
        //        usart_data_send(i);
    }
    if (U_D.Cmd / 0x10 == 5)
    {
        extern short Table_Choose;
        int k = U_D.Cmd % 0x10;
        //**<ע��>����Table_Choose����
        Table_Choose = k;
        table_set(list_Delate, 1);
        // temp = Task_lisk(0);
        // for (int j = 2;; j++)
        // {
        //     if (temp->next != NULL || j == k)
        //     {
        //         if (temp->next->state == 0)
        //         {
        //             temp->state = 0;
        //             lv_obj_add_flag(temp->Table, LV_OBJ_FLAG_HIDDEN);
        //             break;
        //         }
        //     }
        //     else
        //         break;
        //     temp = temp->next;
        // }

        table_arrange();
        Table_reflush(Task_lisk(i)->Table, Task_lisk(i));
    }
    if (U_D.Cmd / 0x10 == 6)
    {
        temp = Task_lisk(0);
        k = 0;
        for (;;)
        {
            if (temp->next != NULL)
            {
                if (temp->next->state == 0)
                {
                    break;
                }
            }
            temp = temp->next;
            k++;
        }
        usart_data_send_task_num(k);
    }
    memset(USART_RX_BUF, 0, 50);
}
void usart_protocol_decoding(u8 *bytes);
void slave_usart_handler(void)
{
    if (uart_flag == 1)
    {
#if Use_Monitor
        usart_protocol_decoding(&USART_RX_BUF[2]);
#endif
    }
}

void slave_usart_timer_init()
{
    extern void usart_protocol_init();
    usart_protocol_init();
    lv_timer_create(slave_usart_handler, 10, NULL);
}

unsigned char Send_Count; // 串口需要发送的数据个数
// short Acc_Data[4]={0,0,0,0};
short Angle_Data[4] = {0, 0, 0, 0};
// short Angle_Speed[4]={0,0,0,0};
int Acc_R_Data[4] = {0, 0, 0, 0};

void USART3_IRQHandler(void) // 串口1中断服务程序
{
    // u8 Res;
    static u8 last_data = 0;
    float temp;
    u8 data = 0;
    static int Data_state = 0,                             // 尾帧
        cut = 0,                                           // 计数
        Data_Flag = 0;                                     // 接收标志�?
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) // 接收中断(接收到的数据必须�?0x0d 0x0a结尾)
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
            case 0X53: // 角度输出
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

typedef enum
{
    Cmd_error,
    Cmd_no_match,
    Cmd_match,
} DecodeFuntionReturn;
#define DecodingFuntion_Length 10
DecodeFuntionReturn (*decodingFuntion[DecodingFuntion_Length])(u8 *bytes);
DecodeFuntionReturn Decode_UploadTask_Cmd(u8 *bytes);
DecodeFuntionReturn Decode_downloadTask_Cmd(u8 *bytes);
DecodeFuntionReturn Decode_ReturnSlaveState_Cmd(u8 *bytes);
void usart_protocol_init()
{
    decodingFuntion[0] = Decode_UploadTask_Cmd;
    decodingFuntion[1] = Decode_downloadTask_Cmd;
    decodingFuntion[2] = Decode_ReturnSlaveState_Cmd;
}

void usart_protocol_decoding(u8 *bytes)
{
    for (int i = 0; i < DecodingFuntion_Length; i++)
    {
        if (decodingFuntion[i] != 0)
        {
            DecodeFuntionReturn e = (decodingFuntion[i])(bytes);
            if (e == Cmd_match)
            {
                uart_flag = 0;
                Uart_len = 0;
            }
        }
    }
}

DecodeFuntionReturn Decode_UploadTask_Cmd(u8 *bytes)
{
    const u8 ModeId = 0x04;
    if (bytes == 0)
        return Cmd_error;
    if (bytes[0] != ModeId)
        return Cmd_no_match;
    u8 ReadID = bytes[1];
    usart_data_send(ReadID, ModeId);
    return Cmd_match;
}

// void Decode_downloadTask_Return(u8 ID)
// {
//     usart_data_send(ID);
// }

DecodeFuntionReturn Decode_downloadTask_Cmd(u8 *bytes)
{
    const u8 ModeId = 0x03;
    typedef struct
    {
        u8 id;
        u8 mode;
        u16 sec_times;
        u8 velocity;
        u16 freq_X10;
    } Download_parameter; // 注意不能直接使用内存赋值 要逐个赋值 涉及内存分配问题

    if (bytes == 0)
        return Cmd_error;
    if (bytes[0] != ModeId)
        return Cmd_no_match;
    Download_parameter e = {
        .id = bytes[1],
        .mode = bytes[2],
        .sec_times = ((u16)bytes[3]) * 256 + (u16)bytes[4],
        .velocity = bytes[5],
        .freq_X10 = ((u16)bytes[6]) * 256 + (u16)bytes[7]};
    int length = Task_lisk_length();
    if (e.id > length) // 当id小于当前的任务数自动创建任务
    {
        for (int i = length + 1; i < e.id; i++) // id是从1开始的 如：当前有1个，那应该在2处开始添加
        {
            table_set(list_Create, 1);
            Table_reflush(Task_lisk(i)->Table, Task_lisk(i)); // 保证创建的任务都被刷新
        }
    }
    Task_lisk(e.id)->mode = e.mode;
    Task_lisk(e.id)->Set_Time = e.sec_times;
    Task_lisk(e.id)->Vel = e.velocity;
    Task_lisk(e.id)->Frep_VOR = ((float)e.freq_X10) / 10;
    Task_lisk(e.id)->state = 1;
    table_show();
    table_arrange();
    Table_reflush(Task_lisk(e.id)->Table, Task_lisk(e.id));
    usart_data_send(e.id, ModeId);
    return Cmd_match;
}

DecodeFuntionReturn Decode_ReturnSlaveState_Cmd(u8 *bytes)
{
    const u8 ModeId = 0x01;
    typedef struct
    {
        u8 Task_Count;
        u8 Runing_ID;
    } state_parameter; // 注意不能直接使用内存赋值 要逐个赋值 涉及内存分配问题

    if (bytes == 0)
        return Cmd_error;
    if (bytes[0] != ModeId)
        return Cmd_no_match;
    u8 returnbytes[2] = {(u8)Task_lisk_length(), State.task};
    Usart_Send_Bytes(returnbytes, sizeof(returnbytes));
    return Cmd_match;
}