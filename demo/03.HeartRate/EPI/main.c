#include <stdio.h>
#include <stdint.h>
#include "xhw_types.h"
#include "xhw_ints.h"
#include "xhw_nvic.h"
#include "xhw_memmap.h"
#include "xdebug.h"
#include "xcore.h"
#include "xhw_sysctl.h"
#include "xsysctl.h"
#include "xhw_gpio.h"
#include "xgpio.h"
#include "xhw_uart.h"
#include "xuart.h"
#include "delay.h"

#include "med03212.h"

unsigned short LCD_BUFFER[480][272];
//*****************************************************************************
//
//! \brief Init uart to print.
//!
//! \param None
//!
//! \details uart config as (115200, 8 data bit, 1 stop bit , no partiy)
//!
//! \return None.
//
//*****************************************************************************
void
TestIOInit(void)
{
    xSysCtlPeripheralEnable(xSYSCTL_PERIPH_GPIOA);
    xSysCtlPeripheralEnable(SYSCTL_PERIPH_AFIO);

    xSPinTypeUART(UART1RX, PA10);
    xSPinTypeUART(UART1TX, PA9);


    xSysCtlPeripheralReset(xSYSCTL_PERIPH_UART1);
    xSysCtlPeripheralEnable(xSYSCTL_PERIPH_UART1);

    xUARTConfigSet(xUART1_BASE, 115200, (xUART_CONFIG_WLEN_8 |
                                         xUART_CONFIG_STOP_1 |
                                         xUART_CONFIG_PAR_NONE));

    xUARTEnable(xUART1_BASE, (xUART_BLOCK_UART | xUART_BLOCK_TX | xUART_BLOCK_RX));
}

void TestIOPut(char ch)
{
    uint8_t c;
    c = ch;
    while((xHWREG(USART1_BASE + USART_SR) & (0x80))==0x00);
    xHWREG(USART1_BASE + USART_DR) = c;
}

void UART_SendStr(char *str)
{
    while(*str){
        TestIOPut(*str);
        str++;
    }
}

char *myitoa(int num,char *str,int radix)
{
    char index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    unsigned unum; /* 中间变量 */
    int i=0,j,k;
    /* 确定unum的值 */
    if(radix==10 && num<0){ /* 十进制负数 */
        unum=(unsigned)-num;
        str[i++]='-';
    }
    else unum=(unsigned)num; /* 其他情况 */
    /* 逆序 */
    do{
        str[i++]=index[unum%(unsigned)radix];
        unum/=radix;
    }while(unum);
    str[i]='\0';
    /* 转换 */
    if(str[0]=='-') k=1; /* 十进制负数 */
    else k=0;
    char temp;
    for(j=k;j<=(i-k-1)/2;j++){
        temp=str[j];
        str[j] = str[i-1+k-j];
        str[i-j-1]=temp;
    }
    return str;
}

extern unsigned long ulj;
extern unsigned char counter;
unsigned long time=0;
unsigned long min=0;
int main(void)
{
    unsigned long tmp;
    char num[20];
    /* Setup the microcontroller system. Initialize the Embedded Flash Interface,  
     initialize the PLL and update the SystemFrequency variable. */
    xSysCtlClockSet(72000000, xSYSCTL_OSC_MAIN | xSYSCTL_XTAL_8MHZ);
    xSysCtlDelay(10000);

    delay_init(72);//延时初始化，此处采用SystemTick精确延时 
    TestIOInit();
    
    Med03212_Init();
    //arrayInit();
    while(1){ 
      if(ulj >= 60) {
          ulj = 0; min++;
          myitoa(counter, num, 10);
          UART_SendStr(num);
      }
      time = (ulj+min*60)*1000 + (1000 - xHWREG(NVIC_ST_CURRENT)/9000);
    }
}


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}
#endif
