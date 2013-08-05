#include "clock.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "Lcd_Driver.h"

#define CENTER_X   64
#define CENTER_Y   80


int HOUR_HAND_BUF[400][2];                                  
int HOUR_HAND_BK_BUF[400];                                 
int MINU_HAND_BUF[600][2];                                
int MINU_HAND_BK_BUF[600];                                  
int SEC_HAND_BUF[100][2];                                   
int SEC_HAND_BK_BUF[100];                                   

int minu_comp = 0;                                          
int hour_comp = 0;                                         

void Gen_Second_Hand(unsigned int angle)                    
{
    int i;
    double Angle,SINA,COSA; 
    double xtemp,ytemp;
    Angle  =(360-angle)*3.1415926/180;
    SINA   = sin(Angle);
    COSA   = cos(Angle);
    for(i=0;i<100;i++){
        xtemp  = CENTER_X-i*SINA;
        ytemp  = CENTER_Y-i*COSA;  
        SEC_HAND_BUF[i][0] = (int)xtemp;
        SEC_HAND_BUF[i][1] = (int)ytemp;
        if((xtemp-SEC_HAND_BUF[i][0])>0.5)
            SEC_HAND_BUF[i][0]++;
        if((ytemp-SEC_HAND_BUF[i][1])>0.5)
            SEC_HAND_BUF[i][1]++;
    }
}

void Save_Second_Hand_BK(void)
{
    unsigned int i;
    for(i=0;i<100;i++)
        SEC_HAND_BK_BUF[i]= Lcd_ReadPoint(SEC_HAND_BUF[i][0], SEC_HAND_BUF[i][1]);  
}

void Draw_Second_Hand(unsigned int color)                   
{
    unsigned int i;
    for(i=0;i<100;i++)
        Gui_DrawPoint(SEC_HAND_BUF[i][0],SEC_HAND_BUF[i][1],color);
}

void Restore_Second_Hand_BK(void)
{
    unsigned int i;
    for(i=0;i<100;i++)
        Gui_DrawPoint(SEC_HAND_BUF[i][0],SEC_HAND_BUF[i][1], SEC_HAND_BK_BUF[i]);  
}

void Gen_Minu_Hand(unsigned int angle)                      
{
    int   i,j,ORD;
    double Angle,SINA,COSA; 
    double xtemp,ytemp;
    Angle  =(360-angle)*3.1415926/180;
    SINA   = sin(Angle);
    COSA   = cos(Angle);
    for(i=-2; i<3; i++){
        for(j=0; j<80; j++)
        {
            xtemp  = CENTER_X+i*COSA-j*SINA;
            ytemp  = CENTER_Y-i*SINA-j*COSA;  
            ORD    =(i+2)*80+j;
            MINU_HAND_BUF[ORD][0] = (int)xtemp;
            MINU_HAND_BUF[ORD][1] = (int)ytemp;
            if((xtemp-MINU_HAND_BUF[ORD][0])>0.5)
                MINU_HAND_BUF[ORD][0]++;
            if((ytemp-MINU_HAND_BUF[ORD][1])>0.5)
                MINU_HAND_BUF[ORD][1]++;
            if(i>-2&&j>0){                                             
                if(  (MINU_HAND_BUF[ORD][0]   -MINU_HAND_BUF[ORD-1 ][0]!=0)
                    &&(MINU_HAND_BUF[ORD][1]   -MINU_HAND_BUF[ORD-1 ][1]!=0)
                    &&(MINU_HAND_BUF[ORD-80][0]-MINU_HAND_BUF[ORD-81][0]!=0)
                    &&(MINU_HAND_BUF[ORD-80][1]-MINU_HAND_BUF[ORD-81][1]!=0)
                    &&(MINU_HAND_BUF[ORD-1 ][1]-MINU_HAND_BUF[ORD-81][1]!=0))
                {
                    if(angle>0&&angle<90){
                        MINU_HAND_BUF[minu_comp+400][0] = MINU_HAND_BUF[ORD-81][0]+1;
                        MINU_HAND_BUF[minu_comp+400][1] = MINU_HAND_BUF[ORD-81][1];
                    }
                    if(angle>90&&angle<180){
                        MINU_HAND_BUF[minu_comp+400][0] = MINU_HAND_BUF[ORD-1][0]+1;
                        MINU_HAND_BUF[minu_comp+400][1] = MINU_HAND_BUF[ORD-1][1];
                    }
                    if(angle>180&&angle<270){
                        MINU_HAND_BUF[minu_comp+400][0] = MINU_HAND_BUF[ORD-1][0];
                        MINU_HAND_BUF[minu_comp+400][1] = MINU_HAND_BUF[ORD-1][1]+1;
                    }
                    if(angle>270&&angle<360){
                        MINU_HAND_BUF[minu_comp+400][0] = MINU_HAND_BUF[ORD-81][0];
                        MINU_HAND_BUF[minu_comp+400][1] = MINU_HAND_BUF[ORD-81][1]-1;
                    }
                    minu_comp++;
                }
            }        
        }
    }
}

void Save_Minu_Hand_BK(void)
{
    unsigned int i;
    for(i=0;i<400+minu_comp;i++)
        MINU_HAND_BK_BUF[i]= Lcd_ReadPoint(MINU_HAND_BUF[i][0], MINU_HAND_BUF[i][1]);  
}

void Draw_Minu_Hand(unsigned int color)                       
{
    unsigned int i;
    for(i=0;i<400;i++)
        Gui_DrawPoint(MINU_HAND_BUF[i][0], MINU_HAND_BUF[i][1], color);
    for(i=0;i<minu_comp;i++)
        Gui_DrawPoint(MINU_HAND_BUF[400+i][0], MINU_HAND_BUF[400+i][1], color);
}

void Restore_Minu_Hand_BK(void)
{
    unsigned int i;
    for(i=0;i<400+minu_comp;i++)
        Gui_DrawPoint(MINU_HAND_BUF[i][0],MINU_HAND_BUF[i][1], MINU_HAND_BK_BUF[i]);  
    minu_comp = 0;
}


void Gen_Hour_Hand(unsigned int angle)                        
{
    int i,j,ORD;
    double Angle,SINA,COSA; 
    double xtemp,ytemp;
    Angle  =(360-angle)*3.1415926/180;
    SINA   = sin(Angle);
    COSA   = cos(Angle);
    for(i=-2;i<3;i++){
        for(j=0;j<60;j++){
            xtemp  = CENTER_X+i*COSA-j*SINA;
            ytemp  = CENTER_Y-i*SINA-j*COSA;  
            ORD    =(i+2)*60+j;
            HOUR_HAND_BUF[ORD][0] = (int)xtemp;
            HOUR_HAND_BUF[ORD][1] = (int)ytemp;
            if((xtemp-HOUR_HAND_BUF[ORD][0])>0.5)
                HOUR_HAND_BUF[ORD][0]++;
            if((ytemp-HOUR_HAND_BUF[ORD][1])>0.5)
                HOUR_HAND_BUF[ORD][1]++;
            if(i>-2&&j>0){                                            
                if(  (HOUR_HAND_BUF[ORD][0]   -HOUR_HAND_BUF[ORD-1 ][0]!=0)
                    &&(HOUR_HAND_BUF[ORD][1]   -HOUR_HAND_BUF[ORD-1 ][1]!=0)
                    &&(HOUR_HAND_BUF[ORD-60][0]-HOUR_HAND_BUF[ORD-61][0]!=0)
                    &&(HOUR_HAND_BUF[ORD-60][1]-HOUR_HAND_BUF[ORD-61][1]!=0)
                    &&(HOUR_HAND_BUF[ORD-1 ][1]-HOUR_HAND_BUF[ORD-61][1]!=0))
                {
                    if(angle>0&&angle<90){
                        HOUR_HAND_BUF[hour_comp+300][0] = HOUR_HAND_BUF[ORD-61][0]+1;
                        HOUR_HAND_BUF[hour_comp+300][1] = HOUR_HAND_BUF[ORD-61][1];
                    }
                    if(angle>90&&angle<180){
                        HOUR_HAND_BUF[hour_comp+300][0] = HOUR_HAND_BUF[ORD-1][0]+1;
                        HOUR_HAND_BUF[hour_comp+300][1] = HOUR_HAND_BUF[ORD-1][1];
                    }
                    if(angle>180&&angle<270){
                        HOUR_HAND_BUF[hour_comp+300][0] = HOUR_HAND_BUF[ORD-1][0];
                        HOUR_HAND_BUF[hour_comp+300][1] = HOUR_HAND_BUF[ORD-1][1]+1;
                    }
                    if(angle>270&&angle<360){
                        HOUR_HAND_BUF[hour_comp+300][0] = HOUR_HAND_BUF[ORD-61][0];
                        HOUR_HAND_BUF[hour_comp+300][1] = HOUR_HAND_BUF[ORD-61][1]-1;
                    }
                    hour_comp++;
                }
            }        
        }
    }
}

void Save_Hour_Hand_BK(void)
{
    unsigned int i;
    for(i=0;i<300+hour_comp;i++)
        HOUR_HAND_BK_BUF[i]= Lcd_ReadPoint(HOUR_HAND_BUF[i][0], HOUR_HAND_BUF[i][1]);  
}

void Draw_Hour_Hand(unsigned int color)                     
{
    unsigned int i;
    for(i=0; i<300; i++)
        Gui_DrawPoint(HOUR_HAND_BUF[i][0], HOUR_HAND_BUF[i][1], color);
    for(i=0; i<hour_comp; i++)
        Gui_DrawPoint(HOUR_HAND_BUF[300+i][0], HOUR_HAND_BUF[300+i][1], color);
}

void Restore_Hour_Hand_BK(void)
{
    unsigned int i;
    for(i=0;i<300+hour_comp;i++)
        Gui_DrawPoint(HOUR_HAND_BUF[i][0],HOUR_HAND_BUF[i][1], HOUR_HAND_BK_BUF[i]);  
    hour_comp = 0;
}

void Ini_Clock_Contex(void)
{
    hour_comp = 0;
    minu_comp = 0;  
}
