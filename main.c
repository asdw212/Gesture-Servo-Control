#include "debug.h"
#include "oled.h"

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    USART1->CTLR1 |= USART_CTLR1_RE;//开启接收使能
    OLED_Init();
    OLED_ShowString(0, 0, "NUM:");
    OLED_ShowString(0, 4, "1111221");
    while(1)
    {
        if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
        {
            char c = USART_ReceiveData(USART1);
            switch (c)
            {
                case '1': OLED_ShowString(0, 2, "1");   break;
                case '2': OLED_ShowString(0, 2, "2");  break;
                case '3': OLED_ShowString(0, 2, "3"); break;
            }
        }
        Delay_Ms(10);
    }
}
