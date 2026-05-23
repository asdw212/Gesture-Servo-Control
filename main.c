#include "debug.h"
#include "oled.h"
#include "Bluetooth.h"
#include "servo_pwm.h"

#define LED1_OPEN    GPIO_SetBits(GPIOA, GPIO_Pin_0)
#define LED1_CLOSED  GPIO_ResetBits(GPIOA, GPIO_Pin_0)
#define LED2_OPEN    GPIO_SetBits(GPIOA, GPIO_Pin_1)
#define LED2_CLOSED  GPIO_ResetBits(GPIOA, GPIO_Pin_1)
#define LED3_OPEN    GPIO_SetBits(GPIOA, GPIO_Pin_4)
#define LED3_CLOSED  GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define LED4_OPEN    GPIO_SetBits(GPIOA, GPIO_Pin_5)
#define LED4_CLOSED  GPIO_ResetBits(GPIOA, GPIO_Pin_5)

u8 OPEN = 0;
u8 mode = 0;
u8 cruise_angle = 90;
u8 cruise_dir = 1;

void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void kaiji(void)
{
    uint8_t i;
    for(i = 0; i < 3; i++)
    {
        LED3_OPEN; LED4_OPEN; Delay_Ms(300);
        LED3_CLOSED; LED4_CLOSED; Delay_Ms(300);
    }
}

void guanji()
{
    uint8_t i;
    for(i = 0; i < 3; i++)
    {
        LED1_OPEN; LED2_OPEN; Delay_Ms(300);
        LED1_CLOSED; LED2_CLOSED; Delay_Ms(300);
    }
}

void guanbi()
{
    OLED_Clear();
    OLED_ShowString(0, 0, "Status: OFF   ");     // 第0行
    OLED_ShowString(0, 2, "BT: Waiting   ");     // 第1行
    OLED_ShowString(0, 4, "LED: All Off  ");     // 第2行
    OLED_ShowString(0, 6, "Mode: None    ");     // 第3行
}

void kaiqi()
{
    OLED_Clear();
    OLED_ShowString(0, 0, "Status: ON    ");     // 第0行
    OLED_ShowString(0, 2, "BT: Connected ");     // 第1行
    OLED_ShowString(0, 4, "LED: Waiting  ");     // 第2行
    OLED_ShowString(0, 6, "Mode: CRUISE  ");     // 第3行
}

void usart_revice()
{
    if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
    {
        char data = USART_ReceiveData(USART1);

        if(OPEN == 1)
        {
            if(data == 'f')
            {
                mode = 1;
                LED3_OPEN;
                OLED_ShowString(0, 6, "Mode: FOLLOW ");   // 第3行
            }
            else if(data == 'c')
            {
                mode = 0;
                LED3_CLOSED;
                OLED_ShowString(0, 6, "Mode: CRUISE ");   // 第3行
            }
            else if(data == 'o')
            {
                LED1_OPEN; LED2_OPEN; LED3_CLOSED; LED4_CLOSED;
                OLED_ShowString(0, 4, "LED: RED      ");  // 第2行
            }
            else if(data == 'x')
            {
                LED1_CLOSED; LED2_CLOSED; LED3_OPEN; LED4_OPEN;
                OLED_ShowString(0, 4, "LED: GREEN    ");  // 第2行
            }
        }
    }
}

int main(void)
{
    SystemCoreClockUpdate();
    Delay_Init();
    LED_Init();
    Servo_Init();
    Bluetooth_Init();
    OLED_Init();
    USART_Printf_Init(115200);
    USART1->CTLR1 |= USART_CTLR1_RE;

    guanbi();

    while(1)
    {
        if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET)
        {
            char data = USART_ReceiveData(USART2);

            if(data == 'a')
            {
                kaiji();
                kaiqi();
                OPEN = 1;
                mode = 0;
                cruise_angle = 0;
                cruise_dir = 1;
            }
            else if(data == 'b')
            {
                guanji();
                guanbi();
                OPEN = 0;
            }
        }

        usart_revice();

        if(OPEN == 1 && mode == 0)
        {
            Servo_SetAngle(cruise_angle);
            Delay_Ms(30);

            if(cruise_dir == 1)
            {
                cruise_angle++;
                if(cruise_angle >= 180) cruise_dir = 0;
            }
            else
            {
                cruise_angle--;
                if(cruise_angle == 0) cruise_dir = 1;
            }
        }

        Delay_Ms(5);
    }
}