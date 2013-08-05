#include <stdint.h>
#include "xhw_types.h"
#include "xhw_ints.h"
#include "xhw_memmap.h"
#include "xhw_config.h"
#include "xhw_nvic.h"
#include "xhw_sysctl.h"
#include "xdebug.h"
#include "xsysctl.h"
#include "xcore.h"
#include "delay.h"

static unsigned char  fac_us=0;//us延时倍乘数
static unsigned short fac_ms=0;//ms延时倍乘数

//SYSTICK的时钟固定为HCLK时钟的1/8
//SYSCLK:系统时钟
void delay_init(unsigned char SYSCLK)
{
    xHWREG(NVIC_ST_CTRL) &= 0xfffffffb;//  HCLK/8
    fac_us = SYSCLK/8;		    
    fac_ms = (uint16_t)fac_us*1000;
}

void delay_ms(unsigned short nms)
{
    uint32_t temp;		   
    xHWREG(NVIC_ST_RELOAD) = (uint32_t)nms*fac_ms;
    xHWREG(NVIC_ST_CURRENT) = 0x00;               
    xHWREG(NVIC_ST_CTRL) = 0x01;                   
    do{
        temp = xHWREG(NVIC_ST_CTRL);
    }while(temp&0x01&&!(temp&(1<<16)));            
    
    xHWREG(NVIC_ST_CTRL) = 0x00;                  
    xHWREG(NVIC_ST_CURRENT) = 0x00;               
}

void delay_us(unsigned int nus)
{
    uint32_t temp;
    
    xHWREG(NVIC_ST_RELOAD) = nus*fac_us;          	  		 
    xHWREG(NVIC_ST_CURRENT) = 0x00;               
    xHWREG(NVIC_ST_CTRL) = 0x01;                  
    
    do{
        temp = xHWREG(NVIC_ST_CTRL);
    }while(temp&0x01&&!(temp&(1<<16)));           
    
    xHWREG(NVIC_ST_CTRL) = 0x00;                  
    xHWREG(NVIC_ST_CURRENT) = 0x00;               	
}