#ifndef _LCD_DRIVER_H_
#define _LCD_DRIVER_H_

#include <stdint.h>
#include "cookie_cfg.h"
#include "cookie.h"

#define RED  	0xf800
#define GREEN	0x07e0
#define BLUE 	0x001f
#define WHITE	0xffff
#define BLACK	0x0000
#define YELLOW  0xFFE0
#define GRAY0   0xEF7D   	    //Grey0 3165 00110 001011 00101
#define GRAY1   0x8410      	//Grey1      00000 000000 00000
#define GRAY2   0x4208      	//Grey2  1111111111011111

#define LCD_CS        	sD11  //MCU_PB2--->>TFT --PIN_CS
#define LCD_RS         	sD10  //PB11--->>TFT --RS
#define LCD_SDA        	sD13  //PB15 MOSI--->>TFT --SDA
#define LCD_SCL        	sD12  //PB13--->>TFT --SCL
#define LCD_RST     	sD9   //PB10--->>TFT --RST



#define	LCD_CS_SET  	xGPIOSPinWrite(LCD_CS, 1)//LCD_CTRL->BSRR=LCD_CS    
#define	LCD_RS_SET  	xGPIOSPinWrite(LCD_RS, 1)//LCD_CTRL->BSRR=LCD_RS    
#define	LCD_SDA_SET  	xGPIOSPinWrite(LCD_SDA, 1)//LCD_CTRL->BSRR=LCD_SDA    
#define	LCD_SCL_SET  	xGPIOSPinWrite(LCD_SCL, 1)//LCD_CTRL->BSRR=LCD_SCL    
#define	LCD_RST_SET  	xGPIOSPinWrite(LCD_RST, 1)//GPIOA->BSRR=LCD_RST

#define	LCD_CS_CLR  	xGPIOSPinWrite(LCD_CS, 0)//LCD_CTRL->BRR=LCD_CS    
#define	LCD_RS_CLR  	xGPIOSPinWrite(LCD_RS, 0)//LCD_CTRL->BRR=LCD_RS    
#define	LCD_SDA_CLR  	xGPIOSPinWrite(LCD_SDA, 0)//LCD_CTRL->BRR=LCD_SDA    
#define	LCD_SCL_CLR  	xGPIOSPinWrite(LCD_SCL, 0)//LCD_CTRL->BRR=LCD_SCL    
#define	LCD_RST_CLR  	xGPIOSPinWrite(LCD_RST, 0)//GPIOA->BRR=LCD_RST 

#define LCD_WR_DATA(data){\
LCD_RS_SET;\
LCD_CS_CLR;\
LCD_DATAOUT(data);\
LCD_WR_CLR;\
LCD_WR_SET;\
LCD_CS_SET;\
} 

void LCD_GPIO_Init(void);
void Lcd_WriteIndex(uint8_t Index);
void Lcd_WriteData(uint8_t Data);
void Lcd_WriteReg(uint8_t Index,uint8_t Data);
uint16_t Lcd_ReadReg(uint8_t LCD_Reg);
void Lcd_Reset(void);
void Lcd_Init(void);
void Lcd_Clear(uint16_t Color);
void Lcd_SetXY(uint16_t x,uint16_t y);
void Gui_DrawPoint(uint16_t x,uint16_t y,uint16_t Data);
unsigned int Lcd_ReadPoint(uint16_t x,uint16_t y);

#endif
