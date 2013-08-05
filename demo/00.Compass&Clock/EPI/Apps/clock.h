#ifndef __CLOCK_H
#define __CLOCK_H

void Ini_Clock_Contex(void);
void Gen_Hour_Hand(unsigned int angle); 
void Save_Hour_Hand_BK(void);
void Draw_Hour_Hand(unsigned int color);
void Restore_Hour_Hand_BK(void);
void Gen_Minu_Hand(unsigned int angle);
void Save_Minu_Hand_BK(void);
void Draw_Minu_Hand(unsigned int color);
void Restore_Minu_Hand_BK(void);
void Gen_Second_Hand(unsigned int angle);
void Save_Second_Hand_BK(void);
void Draw_Second_Hand(unsigned int color);
void Restore_Second_Hand_BK(void);

#endif