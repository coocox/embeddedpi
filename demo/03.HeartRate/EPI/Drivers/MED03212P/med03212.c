#include <stdio.h>
#include "med03212.h"

unsigned long temp[21];
xtBoolean data_effect = xtrue;
unsigned int heart_rate;//the measurement result of heart rate
unsigned long sub;
unsigned char counter=0;
unsigned long last_time = 0;
unsigned long cur_time = 0;

const int max_heartpluse_duty = 2000; //you can change it follow your system's request.
                                      //2000 meams 2 seconds. System return error
                                      //if the duty overtrip 2 second.

unsigned char num[20];
extern void UART_SendStr(char *str);
extern char *myitoa(int num, char *str, int radix);

extern unsigned long time;
unsigned long sD12Callback(void *pvCBData,  unsigned long ulEvent,
                              unsigned long ulMsgParam,
                              void *pvMsgData)
{
    cur_time = time; //xSysTickValueGet();
    if((cur_time - last_time) >= 180)
      counter++;
    last_time = cur_time;
    return 0;
}

/*Function: Initialization for the array(temp)*/
void arrayInit(void)
{
    unsigned char i;
    for(i=0; i < 20; i ++){
        temp[i] = 0;
    }
    temp[20] = xHWREG(NVIC_ST_CURRENT)/9000;
}

void sum(void)
{
    if(data_effect){
        heart_rate=1200000/(temp[20]-temp[0]);//60*20*1000/20_total_time
        UART_SendStr("Heart_rate_is:\t");
        myitoa(heart_rate, num, 10);
        UART_SendStr(num);
    }
    data_effect=1;//sign bit
}

unsigned long ulj=0;
void SysTickIntHandler()
{
    ulj++;
    xSysTickPeriodSet(9000000);
    //myitoa(ulj, num, 10); UART_SendStr(num);
}

void Med03212_Init(void)
{
    xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(MED03212P_IN_Pin));
    xGPIOSPinTypeGPIOInput(MED03212P_IN_Pin);
    xGPIOSPinIntEnable(MED03212P_IN_Pin, GPIO_FALLING_EDGE);
    xGPIOPinIntCallbackInit(GPIOC_BASE, xGPIO_PIN_9, sD12Callback);
    xIntEnable(xINT_GPIOC);
    
    xSysTickPeriodSet(9000000);
    xSysTickEnable();
    xHWREG(NVIC_ST_CTRL) &= 0xfffffffb;
    xSysTickIntEnable();
}
