#include <stdint.h>
#include "xhw_types.h"
#include "xhw_ints.h"
#include "xhw_nvic.h"
#include "xhw_memmap.h"
#include "xdebug.h"
#include "xcore.h"
#include "xsysctl.h"
#include "xgpio.h"
#include "Lcd_Driver.h"
#include "LCD_Config.h"

void LCD_GPIO_Init(void)
{
    xSysCtlPeripheralEnable(xSYSCTL_PERIPH_GPIOA);
    xSysCtlPeripheralEnable(xSYSCTL_PERIPH_GPIOB);
    xGPIODirModeSet( xGPIO_PORTA_BASE, xGPIO_PIN_8, xGPIO_DIR_MODE_OUT); 
    xGPIODirModeSet( xGPIO_PORTB_BASE, xGPIO_PIN_12, xGPIO_DIR_MODE_OUT);
    xGPIODirModeSet( xGPIO_PORTB_BASE, xGPIO_PIN_13, xGPIO_DIR_MODE_OUT);
    xGPIODirModeSet( xGPIO_PORTB_BASE, xGPIO_PIN_14, xGPIO_DIR_MODE_OUT);
    xGPIODirModeSet( xGPIO_PORTB_BASE, xGPIO_PIN_15, xGPIO_DIR_MODE_OUT);
}

void Lcd_WriteIndex(uint8_t Index)
{
    uint8_t i=0;

    LCD_CS_CLR;
    LCD_RS_CLR;

    for(i = 8; i > 0; i--){
        if(Index & 0x80)    
            LCD_SDA_SET; 
        else 
            LCD_SDA_CLR; 

        LCD_SCL_CLR;      
        LCD_SCL_SET;
        Index <<= 1; 
    }
    LCD_CS_SET;
}

void Lcd_WriteData(uint8_t Data)
{
    uint8_t i=0;
    LCD_CS_CLR;
    LCD_RS_SET;
    for(i = 8; i > 0; i--) {
        if(Data & 0x80)    
            LCD_SDA_SET; 
        else 
            LCD_SDA_CLR;
        
        LCD_SCL_CLR;       
        LCD_SCL_SET;
        Data <<= 1; 
    }
    LCD_CS_SET; 
}

void Lcd_WriteReg(uint8_t Index, uint8_t Data)
{
    Lcd_WriteIndex(Index);
    Lcd_WriteData(Data);
}

unsigned int Lcd_ReadData()
{
    uint32_t ReadData = GRAY0;
    return ReadData;
}

void Lcd_Reset(void)
{
    LCD_RST_CLR;
    xSysCtlDelay(xSysCtlClockGet()/20);
    LCD_RST_SET;
    xSysCtlDelay(xSysCtlClockGet()/20);
}


void Lcd_Init(void)
{
    LCD_GPIO_Init();
    Lcd_Reset();
    
    Lcd_WriteIndex(0x11);//Sleep exit 
    xSysCtlDelay(xSysCtlClockGet()/10);
    
    //ST7735R Frame Rate
    Lcd_WriteIndex(0xB1); 
    Lcd_WriteData(0x01); Lcd_WriteData(0x2C); Lcd_WriteData(0x2D); 
    Lcd_WriteIndex(0xB2); 
    Lcd_WriteData(0x01); Lcd_WriteData(0x2C); Lcd_WriteData(0x2D); 
    Lcd_WriteIndex(0xB3); 
    Lcd_WriteData(0x01); Lcd_WriteData(0x2C); Lcd_WriteData(0x2D); 
    Lcd_WriteData(0x01); Lcd_WriteData(0x2C); Lcd_WriteData(0x2D); 
    
    Lcd_WriteIndex(0xB4); //Column inversion 
    Lcd_WriteData(0x07); 
    
    //ST7735R Power Sequence
    Lcd_WriteIndex(0xC0); 
    Lcd_WriteData(0xA2); Lcd_WriteData(0x02); Lcd_WriteData(0x84); 
    Lcd_WriteIndex(0xC1); Lcd_WriteData(0xC5); 
    Lcd_WriteIndex(0xC2); 
    Lcd_WriteData(0x0A); Lcd_WriteData(0x00); 
    Lcd_WriteIndex(0xC3); 
    Lcd_WriteData(0x8A); Lcd_WriteData(0x2A); 
    Lcd_WriteIndex(0xC4); 
    Lcd_WriteData(0x8A); Lcd_WriteData(0xEE); 
    
    Lcd_WriteIndex(0xC5); //VCOM 
    Lcd_WriteData(0x0E); 
    
    Lcd_WriteIndex(0x36); //MX, MY, RGB mode 
    Lcd_WriteData(0xC8); 
    
    //ST7735R Gamma Sequence
    Lcd_WriteIndex(0xe0); 
    Lcd_WriteData(0x0f); Lcd_WriteData(0x1a); 
    Lcd_WriteData(0x0f); Lcd_WriteData(0x18); 
    Lcd_WriteData(0x2f); Lcd_WriteData(0x28); 
    Lcd_WriteData(0x20); Lcd_WriteData(0x22); 
    Lcd_WriteData(0x1f); Lcd_WriteData(0x1b); 
    Lcd_WriteData(0x23); Lcd_WriteData(0x37); Lcd_WriteData(0x00); 
    
    Lcd_WriteData(0x07); 
    Lcd_WriteData(0x02); Lcd_WriteData(0x10); 
    Lcd_WriteIndex(0xe1); 
    Lcd_WriteData(0x0f); Lcd_WriteData(0x1b); 
    Lcd_WriteData(0x0f); Lcd_WriteData(0x17); 
    Lcd_WriteData(0x33); Lcd_WriteData(0x2c); 
    Lcd_WriteData(0x29); Lcd_WriteData(0x2e); 
    Lcd_WriteData(0x30); Lcd_WriteData(0x30); 
    Lcd_WriteData(0x39); Lcd_WriteData(0x3f); 
    Lcd_WriteData(0x00); Lcd_WriteData(0x07); 
    Lcd_WriteData(0x03); Lcd_WriteData(0x10);  
    
    Lcd_WriteIndex(0x2a);
    Lcd_WriteData(0x00);Lcd_WriteData(0x00);
    Lcd_WriteData(0x00);Lcd_WriteData(0x7f);
    Lcd_WriteIndex(0x2b);
    Lcd_WriteData(0x00);Lcd_WriteData(0x00);
    Lcd_WriteData(0x00);Lcd_WriteData(0x9f);
    
    Lcd_WriteIndex(0xF0); //Enable test command  
    Lcd_WriteData(0x01); 
    Lcd_WriteIndex(0xF6); //Disable ram power save mode 
    Lcd_WriteData(0x00); 
    
    Lcd_WriteIndex(0x3A); //65k mode 
    Lcd_WriteData(0x05);    
    
    Lcd_WriteIndex(0x29);//Display on
}

void Lcd_SetRegion(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end)
{        
    Lcd_WriteIndex(0x2a);
    Lcd_WriteData(0x00);
    Lcd_WriteData(x_start);
    Lcd_WriteData(0x00);
    Lcd_WriteData(x_end);

    Lcd_WriteIndex(0x2b);
    Lcd_WriteData(0x00);
    Lcd_WriteData(y_start);
    Lcd_WriteData(0x00);
    Lcd_WriteData(y_end);

    Lcd_WriteIndex(0x2c);
}

void Lcd_SetXY(uint16_t x, uint16_t y)
{
      Lcd_SetRegion(x, y, x, y);
}

void Gui_DrawPoint(uint16_t x,uint16_t y, uint16_t Data)
{
    Lcd_SetRegion(x,y,x+1,y+1);
    Lcd_WriteData(Data>>8);
    Lcd_WriteData(Data);
}    

unsigned int Lcd_ReadPoint(uint16_t x, uint16_t y)
{
    unsigned int Data;
    Lcd_SetXY(x, y);

    Lcd_ReadData();
    Data=Lcd_ReadData();
    //Lcd_WriteData(Data);
    return Data;
}

void Lcd_Clear(uint16_t Color)               
{    
    unsigned int i,m;
    Lcd_SetRegion(0, 0, X_MAX_PIXEL-1, Y_MAX_PIXEL-1);        
    Lcd_WriteIndex(0x2C);
    for(i = 0; i < X_MAX_PIXEL; i++){
        for(m = 0; m < Y_MAX_PIXEL; m++){    
            Lcd_WriteData(Color>>8);
            Lcd_WriteData(Color);
        }
    }
    LCD_CS_SET;           
}
          