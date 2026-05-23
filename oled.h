#ifndef __OLED_H
#define __OLED_H

#include "debug.h"

// OLED分辨率
#define OLED_WIDTH   128
#define OLED_HEIGHT  64

// 字符显示参数
#define ASCII_SIZE   16  // 8x16 ASCII字符大小
#define CHINESE_SIZE 32  // 16x16中文字符大小

void OLED_Init(void);
void OLED_Clear(void);
void OLED_SetPos(uint8_t x, uint8_t y);
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr);
void OLED_ShowString(uint8_t x, uint8_t y, char *str);
void OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t index);

#endif
