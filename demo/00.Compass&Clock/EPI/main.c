#include <math.h>
#include "xhw_types.h"
#include "xhw_ints.h"
#include "xhw_nvic.h"
#include "xhw_memmap.h"
#include "xdebug.h"
#include "xcore.h"
#include "xsysctl.h"
#include "xgpio.h"
#include "Lcd_Driver.h"
#include "GUI.h"
#include "HMC5883L.h"
#include "delay.h"

extern void Redraw_Mainmenu(void);
extern void TFT_Test(void);
extern void Show_Clock(void);

#if 0
// Digital Compass
int main(void)
{
    uint8_t Res;
    int16_t Com_Data[3];
    uint16_t x0, y0;
    unsigned long angle = 0;
    double radian;
    
    /* Setup the microcontroller system. Initialize the Embedded Flash Interface,  
     initialize the PLL and update the SystemFrequency variable. */
    xSysCtlClockSet(72000000, xSYSCTL_OSC_MAIN | xSYSCTL_XTAL_8MHZ);
    xSysCtlDelay(10000);

    delay_init(72);
    Lcd_Init();
    HMC5883L_Init();
    HMC5883L_Cfg(MODE_SIG | GAIN_1090 | SAMPLE_8 | DATA_RATE_15);
    
    Lcd_Clear(GRAY0);
    //
    Gui_Circle(64, 80, 50, BLUE);
    Gui_DrawFont_GBK16(64,14,BLUE,GRAY0,"N"); 
    Gui_DrawFont_GBK16(64,132,BLUE,GRAY0,"S"); 
    Gui_DrawFont_GBK16(5,80,BLUE,GRAY0,"W"); 
    Gui_DrawFont_GBK16(116,80,BLUE,GRAY0,"E"); 
    while(1){
        Res = HMC5883L_DataGet(&Com_Data[0], &Com_Data[1], &Com_Data[2]);
        if(Res) break;
        angle = (unsigned long) (atan2((double)Com_Data[0],(double)Com_Data[1])*(180/3.14159265)+180);
        //angle = rand()%360;
    	radian = angle * 3.1415926 / 180;
        
        if(angle <= 180){
            x0 = 64 + (int16_t)(40 * sin(radian));
            y0 = 80 - (int16_t)(40 * cos(radian));
        } else {
            x0 = 64 + (int16_t)(40 * sin(radian));
            y0 = 80 - (int16_t)(40 * cos(radian));
        }
        Gui_DrawLine(64, 80, x0, y0, BLUE);   
        delay_ms(500);
        Gui_DrawLine(64, 80, x0, y0, GRAY0); 
    }
    return 0;
}

#else
//show clock
int main()
{
	/* Setup the microcontroller system. Initialize the Embedded Flash Interface,
	 initialize the PLL and update the SystemFrequency variable. */
	xSysCtlClockSet(72000000, xSYSCTL_OSC_MAIN | xSYSCTL_XTAL_8MHZ);
	xSysCtlDelay(10000);

	delay_init(72);
	Lcd_Init();
	Lcd_Clear(GRAY0);

	Gui_DrawFont_GBK16(30,50,BLUE,GRAY0,"Show Clock");
	Gui_DrawFont_GBK16(36,70,BLUE,GRAY0,"@ CooCox");
	delay_ms(2000);
	Lcd_Clear(GRAY0);

	while(1){
		Show_Clock();
	}
}
#endif
