#include "servo_pwm.h"
#include "ch32v30x.h"
void Servo_Init(void)
{
    GPIO_InitTypeDef f = {0};
    TIM_TimeBaseInitTypeDef a = {0};
    TIM_OCInitTypeDef s = {0};
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    
    // PB8 复用推挽输出
    f.GPIO_Mode = GPIO_Mode_AF_PP;
    f.GPIO_Speed = GPIO_Speed_50MHz;
    f.GPIO_Pin = GPIO_Pin_8;
    GPIO_Init(GPIOB, &f);
    
    // 时基：50Hz
    a.TIM_ClockDivision = TIM_CKD_DIV1;
    a.TIM_CounterMode = TIM_CounterMode_Up;
    a.TIM_Prescaler = 192 - 1;
    a.TIM_Period = 10000 - 1;
    a.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM4, &a);
    
    // PWM通道3
    s.TIM_OCMode = TIM_OCMode_PWM1;
    s.TIM_OutputState = TIM_OutputState_Enable;
    s.TIM_Pulse = 999;          // 初始90°
    s.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC3Init(TIM4, &s);
    TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
    
    TIM_Cmd(TIM4, ENABLE);
}

void Servo_SetAngle(uint8_t angle)
{
    uint16_t pulse;
    
    if(angle > 180) angle = 180;
    
    pulse = 200 + (uint16_t)((float)angle / 180 * 1050);
    
    TIM_SetCompare3(TIM4, pulse);
}