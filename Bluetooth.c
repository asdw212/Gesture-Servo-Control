#include "Bluetooth.h"
#include "ch32v30x.h"
void Bluetooth_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    GPIO_InitTypeDef g;
    g.GPIO_Mode = GPIO_Mode_AF_PP;
    g.GPIO_Speed = GPIO_Speed_50MHz;
    g.GPIO_Pin = GPIO_Pin_2;        // PA2 = TX
    GPIO_Init(GPIOA, &g);

    g.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    g.GPIO_Pin = GPIO_Pin_3;        // PA3 = RX
    GPIO_Init(GPIOA, &g);

    USART_InitTypeDef u;
    u.USART_BaudRate = 9600;
    u.USART_WordLength = USART_WordLength_8b;
    u.USART_StopBits = USART_StopBits_1;
    u.USART_Parity = USART_Parity_No;
    u.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    u.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART2, &u);
    USART_Cmd(USART2, ENABLE);
}

void Bluetooth_send(u8 data)
{
    USART_SendData(USART2,data);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
}

void Bluetooth_sendstring(char *str)
{
    while(*str)
    {
        Bluetooth_send(*str++);
    }
}

u8 Bluetooth_Receive(void)
{
    while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);  // 等待收到数据
    return USART_ReceiveData(USART2);  // 接收并返回数据
}