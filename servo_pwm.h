#ifndef __SERVO_PWM_H
#define __SERVO_PWM_H

#include "ch32v30x.h"      // 👈 加上这行，uint8_t在这里定义的

void Servo_Init(void);
void Servo_SetAngle(uint8_t angle);

#endif