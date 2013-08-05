#include "Lcd_Driver.h"
#include "GUI.h"
#include "clock.h"
#include "delay.h"

static unsigned char Num[10]={0,1,2,3,4,5,6,7,8,9};

void Redraw_Mainmenu(void)
{
    Lcd_Clear(GRAY0);
    
    Gui_DrawFont_GBK16(16,0,BLUE,GRAY0,"EPI @ CooCox");

    Gui_DrawFont_GBK16(16,120,BLUE,GRAY0,"S1:Move.    ");
    Gui_DrawFont_GBK16(16,140,RED,GRAY0, "S2:Sellect  ");
    
    Gui_DrawFont_Num32(100,125,RED,GRAY0,Num[5]);
    delay_ms(1000);
    Gui_DrawFont_Num32(100,125,RED,GRAY0,Num[4]);
    delay_ms(1000);
    Gui_DrawFont_Num32(100,125,RED,GRAY0,Num[3]);
    delay_ms(1000);
    Gui_DrawFont_Num32(100,125,RED,GRAY0,Num[2]);
    delay_ms(1000);
    Gui_DrawFont_Num32(100,125,RED,GRAY0,Num[1]);
    delay_ms(1000);
    Gui_DrawFont_Num32(100,125,RED,GRAY0,Num[0]);    
}

void Num_Test(void)
{
    uint8_t i=0;
    Lcd_Clear(GRAY0);
    Gui_DrawFont_GBK16(16,20,RED,GRAY0,"Num Test");
    delay_ms(1000);
    Lcd_Clear(GRAY0);

    for(i=0;i<10;i++){
        Gui_DrawFont_Num32((i%3)*40,32*(i/3)+30,RED,GRAY0,Num[i+1]);
        delay_ms(100);
    }    
}

void Color_Test(void)
{
    uint8_t i = 2;
    Lcd_Clear(GRAY0);
    
    Gui_DrawFont_GBK16(20,10,BLUE,GRAY0,"Color Test");
    delay_ms(1000);

    while(i--){
        Lcd_Clear(WHITE);
        Lcd_Clear(BLACK);
        Lcd_Clear(RED);
        Lcd_Clear(GREEN);
        Lcd_Clear(BLUE);
    }        
}

void TFT_Test()
{
    Color_Test();
    Num_Test();
    Gui_DrawFont_GBK16(10,50,BLUE,GRAY0,"Test ok.");
    delay_ms(1000);
}
#define POINTERCOLOR 0xf800
void Show_Clock(void)
{
  unsigned int Hour ,Minute,Second;
  Hour    = 9;//GetRTCHOUR();
  while(Hour>=12)
  {
    Hour -= 12;
  }
  Minute  = 0;//GetRTCMIN();
  Second  = 0;//GetRTCSEC();
  Ini_Clock_Contex();
  //ShowBMP240320(BMP_CLOCK);
  Gen_Hour_Hand(Hour*30+Minute/2);
  Save_Hour_Hand_BK(); 
  Draw_Hour_Hand(POINTERCOLOR);
  Gen_Minu_Hand(Minute*6+Second/10);
  Save_Minu_Hand_BK(); 
  Draw_Minu_Hand(POINTERCOLOR);
  Gen_Second_Hand(Second*6);
  Save_Second_Hand_BK(); 
  Draw_Second_Hand(POINTERCOLOR);
  for(;;){
#if 0
    if(Second==GetRTCSEC())     continue;
    Second   = GetRTCSEC();
    Minute  = GetRTCMIN();
    Hour    = GetRTCHOUR();
#else
      delay_ms(1000); //delay 1s
      Second ++;
      if(Second >= 60){
        Second = 0; Minute ++;
        if(Minute>=60) {
            Minute = 0; Hour ++;
        }
      }
#endif
    while(Hour>=12)
    {
      Hour -= 12;
    }
    if(Minute%6==0&&Second==0)                                      
    {
      Restore_Second_Hand_BK();
      Restore_Minu_Hand_BK();
      Restore_Hour_Hand_BK();
      Gen_Hour_Hand(Hour*30+Minute/2);
      if(Second%10==0)
        Gen_Minu_Hand(Minute*6+Second/10);
      Gen_Second_Hand(Second*6);
      Save_Hour_Hand_BK(); 
      Draw_Hour_Hand(POINTERCOLOR);
      Save_Minu_Hand_BK(); 
      Draw_Minu_Hand(POINTERCOLOR);
      Save_Second_Hand_BK(); 
      Draw_Second_Hand(POINTERCOLOR);
    }
    else if(Second%10==0)                                           
    {
      Restore_Second_Hand_BK();
      Restore_Minu_Hand_BK();      
      Gen_Minu_Hand(Minute*6+Second/10);
      Gen_Second_Hand(Second*6);
      Save_Minu_Hand_BK(); 
      Draw_Minu_Hand(POINTERCOLOR);
      Save_Second_Hand_BK(); 
      Draw_Second_Hand(POINTERCOLOR);
    } else {
      Restore_Second_Hand_BK();
      Gen_Second_Hand(Second*6);
      Save_Second_Hand_BK(); 
      Draw_Second_Hand(POINTERCOLOR);
    }                                                   
  }
}
