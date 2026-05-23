#ifndef __OLED_C
#define __OLED_C

#include "oled.h"
#include "oled_font.h"

#define OLED_I2C_ADDR    0x3C

void I2C1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    I2C_InitStructure.I2C_ClockSpeed = 400000;
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C1, &I2C_InitStructure);

    I2C_Cmd(I2C1, ENABLE);
}

void I2C_Write_Byte(uint8_t addr, uint8_t reg, uint8_t data)
{
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, addr << 1, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(I2C1, reg);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_SendData(I2C1, data);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_GenerateSTOP(I2C1, ENABLE);
}

void OLED_Init_Display(void)
{
    Delay_Ms(100);
    
    I2C_Write_Byte(OLED_I2C_ADDR, 0x00, 0xAE);
    I2C_Write_Byte(OLED_I2C_ADDR, 0x00, 0xD5);
    I2C_Write_Byte(OLED_I2C_ADDR, 0x00, 0x80);
    I2C_Write_Byte(OLED_I2C_ADDR, 0x00, 0xA8);
    I2C_Write_Byte(OLED_I2C_ADDR, 0x00, 0x3F);
    I2C_Write_Byte(OLED_I2C_ADDR, 0x00, 0xD3);
    I2C_Write_Byte(OLED_I2C_ADDR, 0x00, 0x00);
    I2C_Write_Byte(OLED_I2C_ADDR, 0x00, 0x40);
    I2C_Write_Byte(OLED_I2C_ADDR, 0x00, 0x8D);
    I2C_Write_Byte(OLED_I2C_ADDR, 0x00, 0x14);
    I2C_Write_Byte(OLED_I2C_ADDR, 0x00, 0x20);
    I2C_Write_Byte(OLED_I2C_ADDR, 0x00, 0x02);
    I2C_Write_Byte(OLED_I2C_ADDR, 0x00, 0xA1);
    I2C_Write_Byte(OLED_I2C_ADDR, 0x00, 0xC8);
    I2C_Write_Byte(OLED_I2C_ADDR, 0x00, 0xDA);
    I2C_Write_Byte(OLED_I2C_ADDR, 0x00, 0x12);
    I2C_Write_Byte(OLED_I2C_ADDR, 0x00, 0x81);
    I2C_Write_Byte(OLED_I2C_ADDR, 0x00, 0xCF);
    I2C_Write_Byte(OLED_I2C_ADDR, 0x00, 0xD9);
    I2C_Write_Byte(OLED_I2C_ADDR, 0x00, 0xF1);
    I2C_Write_Byte(OLED_I2C_ADDR, 0x00, 0xDB);
    I2C_Write_Byte(OLED_I2C_ADDR, 0x00, 0x40);
    I2C_Write_Byte(OLED_I2C_ADDR, 0x00, 0xA4);
    I2C_Write_Byte(OLED_I2C_ADDR, 0x00, 0xA6);
    I2C_Write_Byte(OLED_I2C_ADDR, 0x00, 0xAF);
}

void OLED_Init(void)
{
    I2C1_Init();
    OLED_Init_Display();
    OLED_Clear();
}

void OLED_Clear(void)
{
    uint8_t i, j;
    
    for(i = 0; i < 8; i++)
    {
        I2C_Write_Byte(OLED_I2C_ADDR, 0x00, 0xB0 + i);
        I2C_Write_Byte(OLED_I2C_ADDR, 0x00, 0x00);
        I2C_Write_Byte(OLED_I2C_ADDR, 0x00, 0x10);
        
        for(j = 0; j < 128; j++)
        {
            I2C_Write_Byte(OLED_I2C_ADDR, 0x40, 0x00);
        }
    }
}

void OLED_SetPos(uint8_t x, uint8_t y)
{
    I2C_Write_Byte(OLED_I2C_ADDR, 0x00, 0xB0 + y);
    I2C_Write_Byte(OLED_I2C_ADDR, 0x00, (x & 0x0F));
    I2C_Write_Byte(OLED_I2C_ADDR, 0x00, ((x & 0xF0) >> 4) | 0x10);
}

void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr)
{
    uint8_t i;
    uint8_t c = chr - ' ';
    
    OLED_SetPos(x, y);
    for(i = 0; i < 8; i++)
    {
        I2C_Write_Byte(OLED_I2C_ADDR, 0x40, OLED_ASCII[c][i]);
    }
    
    OLED_SetPos(x, y + 1);
    for(i = 0; i < 8; i++)
    {
        I2C_Write_Byte(OLED_I2C_ADDR, 0x40, OLED_ASCII[c][i + 8]);
    }
}

void OLED_ShowString(uint8_t x, uint8_t y, char *str)
{
    uint8_t i = 0;
    
    while(str[i] != '\0')
    {
        OLED_ShowChar(x, y, str[i]);
        x += 8;
        if(x > 120)
        {
            x = 0;
            y += 2;
        }
        i++;
    }
}

void OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t index)
{
    uint8_t i;
    
    OLED_SetPos(x, y);
    for(i = 0; i < 16; i++)
    {
        I2C_Write_Byte(OLED_I2C_ADDR, 0x40, OLED_CHINESE[index][i]);
    }
    
    OLED_SetPos(x, y + 1);
    for(i = 0; i < 16; i++)
    {
        I2C_Write_Byte(OLED_I2C_ADDR, 0x40, OLED_CHINESE[index][i + 16]);
    }
}

#endif