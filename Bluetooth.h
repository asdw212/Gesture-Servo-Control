#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#include "ch32v30x.h"
#define BT_TX   GPIO_Pin_3   // PD3 → UART4_TX（单片机发）
#define BT_RX   GPIO_Pin_2   // PD2 → UART4_RX（单片机收）
#define BT_GPIO GPIOD

void Bluetooth_Init(void);
void Bluetooth_send(u8 data);
void Bluetooth_sendstring(char *str);
u8 Bluetooth_Receive(void);
#endif
