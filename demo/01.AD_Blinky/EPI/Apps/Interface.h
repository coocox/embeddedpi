#ifndef __GPIOTEST_H__
#define __GPIOTEST_H__

#define FALSE                   0
#define TRUE                    1

typedef struct CmdStruct
{
    unsigned char ret;

    unsigned char cmd_1;
    unsigned char cmd_2;

    unsigned char cmd_3;
    unsigned char rx_len;
    unsigned char *rx_buf;

    unsigned char tx_len;
    unsigned char *tx_buf;

    //reserved for expand
}CMDSTRUCT;


unsigned long MotorShieldOperation(CMDSTRUCT *pCmdStruct);
unsigned long UltrasonicSensorOperation(CMDSTRUCT *pCmdStruct);
unsigned long TinkerKitOperation(CMDSTRUCT *pCmdStruct);
unsigned long SystemCtl(CMDSTRUCT *pCmdStruct);
unsigned long DigitalCompass(CMDSTRUCT *pCmdStruct);


#endif
