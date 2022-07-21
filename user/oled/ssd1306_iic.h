#ifndef SSD1306_IIC_H
#define SSD1306_IIC_H

#include <stdint.h>

#define WRITE 0
#define READ  1
//#define SSD1306_I2C_ADDR 0x3c
#define SSD1306_I2C_ADDR 0x78

extern void SSD1306_I2C_Init(void);
//extern void ssd1306_I2C_Write(uint8_t reg, uint8_t data);
extern void SSD1306_WriteCommand(uint8_t cmd);
extern void SSD1306_WriteData(uint8_t data);
extern void SSD1306_I2C_WriteMulti(uint8_t reg, uint8_t *data, uint16_t count);
extern uint8_t SSD1306_I2C_Read(uint8_t reg);
extern uint8_t* SSD1306_I2C_ReadMulti(uint8_t reg, uint8_t len);

#endif
