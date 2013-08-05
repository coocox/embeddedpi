#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "xhw_types.h"
#include "xcore.h"
#include "xhw_ints.h"
#include "xhw_memmap.h"
#include "xhw_nvic.h"
#include "xhw_sysctl.h"
#include "xhw_gpio.h"
#include "xhw_timer.h"
#include "xdebug.h"
#include "xsysctl.h"
#include "xgpio.h"
#include "cookie.h"
#include "xadc.h"
#include "xtimer.h"
#include "DriverCfg.h"

#define STAT_MOTOR_UNINIT    0
#define STAT_MOTOR_INIT      1
#define STAT_MOTOR_OVER      5

//
// Motor Control parameters
//
//typedef struct
//{
//  unsigned long ulStepCnt;                               // Current running step
//  unsigned long ulTotalStep;                             // Total Steps need to run
//  unsigned char ucDir;                                   // Direction of running
//  unsigned long ulStepInterval;                          // The interval of every step (unit is ms)
//  unsigned long ulCurrentPosition;
//}StepMotorStruct;


extern volatile StepMotorStruct StepMotor;

#if (MOTOR_STEP == 8)

// Break A : 0 1 0 0 0 1 0 1
// Dir   A : 1 0 0 0 0 0 1 0
// Break B : 0 0 0 1 0 0 0 1
// Dir   B : 1 1 1 0 0 0 0 0
static const unsigned char g_ul42BYGH_8_Step[8] =
{
    0x05, 0x09, 0x01, 0x02, 0x00, 0x08, 0x04, 0x06
};

#elif (MOTOR_STEP == 4)

static const unsigned long g_ul42BYGH_4_Step[4] =
{
    0x03, 0x01, 0x00, 0x02
};

#else
#error Please select stepmotor parameter
#endif

//
// Timer4 interrupt callback function, to deal with step motor
//
unsigned long Timer4Callback(void *pvCBData,  unsigned long ulEvent,
                                       unsigned long ulMsgParam,
                                       void *pvMsgData)
{
    static unsigned long sulIntCntForStepMotor=0;
    static unsigned short susIntCntForSenSor = 0;
    static long int index = 0;
    //
    // Clear the status
    //
    TimerFlagStatusClear(xTIMER4_BASE, TIMER_FLAG_UPDATE);

    //Motor Speed
    if(++sulIntCntForStepMotor >= StepMotor.ulStepInterval)
    {
        sulIntCntForStepMotor = 0;

        //
        // If run over
        //
        if(StepMotor.ulStepCnt++ >= StepMotor.ulTotalStep)
        {
            StepMotor.ulStepCnt = 0;
            StepMotor.ulTotalStep = 0;

            //NOTE: Here you must not disable stepmotor control channel by this code:
            //  !!WRONG!! xGPIOSPinWrite(sD3, 0);
            //  !!WRONG!! xGPIOSPinWrite(sD11, 0);
            //  others, motor can not work correctly when rotate step by step
            xGPIOSPinWrite(sD9, 1);
            xGPIOSPinWrite(sD8, 1);
        }
        else
        {

#if  (MOTOR_STEP == 8)
            if(StepMotor.ucDir == AMS_RUN_FORWARD)
            {
                if(++index >= 8)
                {
                    index  = 0;
                }
                StepMotor.CurrentPost++;
            }
            else
            {
                StepMotor.CurrentPost--;
                if(--index < 0)
                {
                    index = 7;
                }
            }

            //Motor control Channel A
            xGPIOSPinWrite(sD9,  (g_ul42BYGH_8_Step[index] >> 3) & 0x01);
            xGPIOSPinWrite(sD12, (g_ul42BYGH_8_Step[index] >> 2) & 0x01);
            //Motor control Channel B
            xGPIOSPinWrite(sD8,  (g_ul42BYGH_8_Step[index] >> 1) & 0x01);
            xGPIOSPinWrite(sD13, (g_ul42BYGH_8_Step[index] >> 0) & 0x01);
#elif (MOTOR_STEP == 4)
            if(StepMotor.ucDir == AMS_RUN_FORWARD)
            {
                if(++index >= 4)
                {
                    index  = 0;
                }
                StepMotor.CurrentPost++;
            }
            else
            {
                StepMotor.CurrentPost--;
                if(--index < 0)
                {
                    index = 3;
                }
            }

            xGPIOSPinWrite(sD9, 0);
            xGPIOSPinWrite(sD8, 0);
            xGPIOSPinWrite(sD12, ((g_ul42BYGH_4_Step[index] >> 1) & 0x1));
            xGPIOSPinWrite(sD13, ((g_ul42BYGH_4_Step[index] >> 0) & 0x1));
#else
#error Please select stepmotor parameter
#endif
        }

    }

    if(susIntCntForSenSor++ >= 50)
    {
        //
        // send a triger pulse to HC-SR04 to start measure distance.
        // The ultrasonic module is triggered every 50ms
        //
        susIntCntForSenSor = 0;
        xGPIOSPinWrite( HCSR04_TRIG_PIN, 1 );
    }
    else
    {
        xGPIOSPinWrite( HCSR04_TRIG_PIN, 0 );
    }

    return 0;
}

unsigned long MotorShieldOperation(CMDSTRUCT *pCmdStruct)
{
    //
    // MotoShield initialization status.
    //
    static unsigned char ucStatus = STAT_MOTOR_UNINIT;
    unsigned long ulTmp = 0;
    unsigned char i;

    if((pCmdStruct->cmd_1 == 0x40)&&(pCmdStruct->cmd_2 == 0x0)) //Èç¹ûÊÇmotorshield
    {
        switch(pCmdStruct->cmd_3)
        {
        case 0x01: // DC motor initialize
            ulTmp = *(unsigned long *)&pCmdStruct->rx_buf[1];
            AMSDCMotorPWMInit(pCmdStruct->rx_buf[0], ulTmp, pCmdStruct->rx_buf[5]);
            pCmdStruct->ret = TRUE;
            pCmdStruct->tx_len = 0;
            ucStatus = STAT_MOTOR_INIT;
            break;

        case 0x02:  // DC motor set speed
            if(ucStatus != STAT_MOTOR_UNINIT)
            {
                ulTmp = *(unsigned long *)&pCmdStruct->rx_buf[1];
                AMSDCMotorSpeedSet(pCmdStruct->rx_buf[0], ulTmp);
                pCmdStruct->ret = TRUE;
                pCmdStruct->tx_len = 0;
            }
            else
            {
                pCmdStruct->ret = FALSE;
                //pCmdStruct->tx_buf[0] = 0;  //can return error message.
                pCmdStruct->tx_len = 0;
            }
            break;

        case 0x03: // DC motor start
            if(ucStatus != STAT_MOTOR_UNINIT)
            {
                AMSDCMotorRun(pCmdStruct->rx_buf[0], pCmdStruct->rx_buf[1]);
                pCmdStruct->ret = TRUE;
                pCmdStruct->tx_len = 0;
            }
            else
            {
                pCmdStruct->ret = FALSE;
                //pCmdStruct->tx_buf[0] = 0;  //can return error message.
                pCmdStruct->tx_len = 0;
            }
            break;

        case 0x04:  // DC motor stop
            AMSDCMotorStop(pCmdStruct->rx_buf[0]);
            pCmdStruct->ret = TRUE;
            pCmdStruct->tx_len = 0;
            break;

        case 0x05:  // Step motor initialize
            AMSStepMotorInit();
            xSysCtlPeripheralEnable(xSYSCTL_PERIPH_TIMER4);

            //
            // Clear the flag first
            //
            TimerIntClear(xTIMER4_BASE, TIMER_INT_UEV);

            //
            // Config timer4 as periodic mode, 1000Hz interrupt frequency
            // This timer as a trigger source for step motor and other peripheral components
            //
            xTimerInitConfig(xTIMER4_BASE, xTIMER_CHANNEL0, xTIMER_MODE_PERIODIC | xTIMER_COUNT_UP, 1000);
            xTimerIntCallbackInit(xTIMER4_BASE, Timer4Callback);
            xTimerIntEnable(xTIMER4_BASE, xTIMER_CHANNEL0, xTIMER_INT_MATCH);
            xIntPrioritySet(xINT_TIMER4, 20);  //a low priority
            xIntEnable(xINT_TIMER4);
            xTimerStart(xTIMER4_BASE, xTIMER_CHANNEL0);

            //
            // Motor disable
            //
            AMSStepMotorStop();
            ucStatus = STAT_MOTOR_INIT;
            pCmdStruct->ret = TRUE;
            pCmdStruct->tx_len = 0;
            break;

        case 0x06:  // Step motor speed set
            ulTmp = *(unsigned long *)&pCmdStruct->rx_buf[0];

            //
            // ulTmp > 0 update ulStepInterval; ulTmp == 0 stop
            //
            if(ulTmp)
            {
                StepMotor.ulStepInterval = ulTmp;
            }
            else
            {
                // If speed == 0, stop
                AMSStepMotorStop();
            }
            pCmdStruct->ret = TRUE;
            pCmdStruct->tx_len = 0;
            break;

        case 0x07:  // Step motor start
            if(ucStatus != STAT_MOTOR_UNINIT)
            {

                StepMotor.ucDir = pCmdStruct->rx_buf[0];
                StepMotor.ulTotalStep = *(unsigned long *)&pCmdStruct->rx_buf[1];
                if(StepMotor.ulTotalStep && StepMotor.ulStepInterval)
                {
                    //
                    // Enable motor output
                    //
                    xGPIOSPinWrite(sD3,1);
                    xGPIOSPinWrite(sD11,1);
                }
                pCmdStruct->ret = TRUE;
                pCmdStruct->tx_len = 0;
            }
            else
            {
                pCmdStruct->ret = FALSE;
                pCmdStruct->tx_len = 0;
            }
            break;

        case 0x08:  // Step motor stop
            AMSStepMotorStop();

            pCmdStruct->ret = TRUE;
            pCmdStruct->tx_len = 0;
            break;

        case 0x09:  // Get current
            AMSMotorADCInit();
            if(pCmdStruct->rx_buf[0] < 3)
            {
                //
                // Single channel
                //
                ulTmp = AMSMotorCurrentGet(pCmdStruct->rx_buf[0]);
                for(i=0;i<4;i++)
                    pCmdStruct->tx_buf[i] = (unsigned char)(ulTmp>>(i*8));
                pCmdStruct->ret = TRUE;
                pCmdStruct->tx_len = 4;
            }
            else
            {
                //
                // Double channel
                //
                ulTmp = AMSMotorCurrentGet(AMS_MOTOR_A);
                for(i=0;i<4;i++)
                    pCmdStruct->tx_buf[i] = (unsigned char)(ulTmp>>(i*8));
                ulTmp = AMSMotorCurrentGet(AMS_MOTOR_B);
                for(i=4;i<8;i++)
                    pCmdStruct->tx_buf[i] = (unsigned char)(ulTmp>>((i-4)*8));
                pCmdStruct->ret = TRUE;
                pCmdStruct->tx_len = 8;
            }
            break;

        case 0x0A: //Servo Angle
            {
                signed long _Angle = *(signed long *) pCmdStruct->rx_buf;
                SteerMotorRun(_Angle);
                pCmdStruct->ret = TRUE;
                pCmdStruct->tx_len = 0;
                break;
            }

        case 0x0B: //Servo Speed
            {
                ulTmp = *(unsigned long *) pCmdStruct->rx_buf;
                SteerMotorSpeedSet(ulTmp);
                pCmdStruct->ret = TRUE;
                pCmdStruct->tx_len = 0;
                break;
            }
        default :
            pCmdStruct->ret = FALSE;
            pCmdStruct->tx_len = 0;
            break;
        }
    }
    return 0;
}

unsigned long UltrasonicSensorOperation(CMDSTRUCT *pCmdStruct)
{
    unsigned short tmp = 0;
    unsigned char *p = (unsigned char *)&tmp;
    if((pCmdStruct->cmd_1 == 0x40)&&(pCmdStruct->cmd_2 == 0x02))  //Ultrasonic
    {
        switch(pCmdStruct->cmd_3)
        {
            case 0x01:
                HCSR04Init();
                pCmdStruct->ret = TRUE;
                pCmdStruct->tx_len = 0;
                break;
            case 0x02:
                //*(unsigned short *)pCmdStruct->tx_buf = HCSR04GetDistance();
                //*(unsigned short *)pCmdStruct->tx_buf = HCSR04GetDistance();
                tmp = HCSR04GetDistance();
                //printf("%d\r\n", tmp);
                pCmdStruct->tx_buf[0] = *p++;
                pCmdStruct->tx_buf[1] = *p++;

                pCmdStruct->ret = TRUE;
                pCmdStruct->tx_len = 2;
                break;
            case 0x03:
                HCSR04Stop();
                pCmdStruct->ret = TRUE;
                pCmdStruct->tx_len = 0;
                break;
            default:
                pCmdStruct->ret = FALSE;
                pCmdStruct->tx_len = 0;
                break;
        }
    }

    return (0);
}

//*****************************************************************************
//
//! \brief TinkerKit operation
//!
//! \param pCmdStruct the command struct to pass parameter and get result
//!
//! This function is to initialize the IO port and timer to get ready for measure
//!
//! \return None.
//
//*****************************************************************************
unsigned long TinkerKitOperation(CMDSTRUCT *pCmdStruct)
{
    unsigned long ulADCTmp[4];
    unsigned char i;

    if((pCmdStruct->cmd_1 == 0x40)&&(pCmdStruct->cmd_2 == 0x1))  //tinkerkit
    {
        switch(pCmdStruct->cmd_3)
        {
        case 0x01:
            switch(pCmdStruct->rx_buf[0])
            {
            case 0: // O0 -> sD11
                xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(sD11));
                xGPIOSPinTypeGPIOOutput(sD11);
                xGPIOSPinWrite(sD11, pCmdStruct->rx_buf[1]);
                pCmdStruct->ret = TRUE;
                break;
            case 1: // O1 -> sD10
                xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(sD10));
                xGPIOSPinTypeGPIOOutput(sD10);
                xGPIOSPinWrite(sD10, pCmdStruct->rx_buf[1]);
                pCmdStruct->ret = TRUE;
                break;
            case 2: // O2 -> sD9
                xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(sD9));
                xGPIOSPinTypeGPIOOutput(sD9);
                xGPIOSPinWrite(sD9, pCmdStruct->rx_buf[1]);
                pCmdStruct->ret = TRUE;
                break;
            case 3: // O3 -> sD6
                xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(sD6));
                xGPIOSPinTypeGPIOOutput(sD6);
                xGPIOSPinWrite(sD6, pCmdStruct->rx_buf[1]);
                pCmdStruct->ret = TRUE;
                break;
            case 4: // O4 -> sD5
                xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(sD5));
                xGPIOSPinTypeGPIOOutput(sD5);
                xGPIOSPinWrite(sD5, pCmdStruct->rx_buf[1]);
                pCmdStruct->ret = TRUE;
                break;
            case 5: // O5 -> sD3
                xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(sD3));
                xGPIOSPinTypeGPIOOutput(sD3);
                xGPIOSPinWrite(sD3, pCmdStruct->rx_buf[1]);
                pCmdStruct->ret = TRUE;
                break;
            default:
                pCmdStruct->ret = FALSE;
                break;
            }
            pCmdStruct->tx_len = 0;
            break;
        case 0x02:
            switch(pCmdStruct->rx_buf[0])
            {
            case 0: // I0 -> sA0
                xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(sA0));
                xGPIOSPinTypeGPIOInput(sA0);
                pCmdStruct->tx_buf[0] =
                        xGPIOSPinRead(sA0);
                pCmdStruct->ret = TRUE;
                pCmdStruct->tx_len = 1;
                break;
            case 1: // I1 -> sA1
                xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(sA1));
                xGPIOSPinTypeGPIOInput(sA1);
                pCmdStruct->tx_buf[0] =
                        xGPIOSPinRead(sA1);
                pCmdStruct->ret = TRUE;
                pCmdStruct->tx_len = 1;
                break;
            case 2: // I2 -> sA2
                xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(sA2));
                xGPIOSPinTypeGPIOInput(sA2);
                pCmdStruct->tx_buf[0] =
                        xGPIOSPinRead(sA3);
                pCmdStruct->ret = TRUE;
                pCmdStruct->tx_len = 1;
                break;
            case 3: // I3 -> sA3
                xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(sA3));
                xGPIOSPinTypeGPIOInput(sA3);
                pCmdStruct->tx_buf[0] =
                        xGPIOSPinRead(sA3);
                pCmdStruct->ret = TRUE;
                pCmdStruct->tx_len = 1;
                break;
            case 4: // I4 -> sA4
                xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(sA4));
                xGPIOSPinTypeGPIOInput(sA4);
                pCmdStruct->tx_buf[0] =
                        xGPIOSPinRead(sA4);
                pCmdStruct->ret = TRUE;
                pCmdStruct->tx_len = 1;
                break;
            case 5: // I5 -> sA5
                xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(sA5));
                xGPIOSPinTypeGPIOInput(sA5);
                pCmdStruct->tx_buf[0] =
                        xGPIOSPinRead(sA5);
                pCmdStruct->ret = TRUE;
                pCmdStruct->tx_len = 1;
                break;
            default:
                pCmdStruct->ret = FALSE;
                pCmdStruct->tx_len = 0;
                break;
            }
            break;
        case 0x03:

            //
            // Set ADCCLK prescaler, ADCCLK=PCLK2(max 72MHz)/DIV(DIV:2,4,6,8)
            // You should set ADCCLK < 14MHz to ensure the accuracy of ADC
            //
            xSysCtlPeripheralClockSourceSet(xSYSCTL_ADC0_MAIN, 8);
            //
            // Enable Peripheral ADC clock
            //
            xSysCtlPeripheralEnable(xSYSCTL_PERIPH_ADC1);

            //
            // Enable the ADC conversion
            //
            xADCEnable(sADC_BASE);

            //
            // The two sentences below configure ADC to scan mode, continuous convert, software trigger.
            //
            xADCConfigure(sADC_BASE, xADC_MODE_SCAN_SINGLE_CYCLE, ADC_TRIGGER_PROCESSOR);
            xADCConfigure(sADC_BASE, xADC_MODE_SCAN_CONTINUOUS, ADC_TRIGGER_PROCESSOR);

            //
            // Configure channel step by step.(Max 4 steps, the 2nd parameter start from 0, max is 3)
            // Must not jump over a step, or the ADC result may be in wrong position.
            // Here we enable 4 channels of the I0~I3 which work in continuous conversion.
            // You can read one channel a time or multi-channel a time.
            //
            xADCStepConfigure(sADC_BASE, 0, ADC_CHANNEL_10);
            xADCStepConfigure(sADC_BASE, 1, ADC_CHANNEL_11);
            xADCStepConfigure(sADC_BASE, 2, ADC_CHANNEL_12);
            xADCStepConfigure(sADC_BASE, 3, ADC_CHANNEL_13);

            switch(pCmdStruct->rx_buf[0])
            {
            case 0:       // I0 -> sA0
                // Enable IO clock
                xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(sA0));
                // Set IO direction and type.
                sA0PinTypeADC();
                // Read ADC value
                xADCDataGet(sADC_BASE, ulADCTmp);
                for(i=0;i<4;i++)
                    pCmdStruct->tx_buf[i] = ulADCTmp[0]>>(i*8);
                pCmdStruct->ret = TRUE;
                pCmdStruct->tx_len = 4;
                break;

            case 1:       // I1 -> sA1
                // Enable IO clock
                xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(sA1));
                // Set IO direction and type.
                sA1PinTypeADC();
                // Read ADC value
                xADCDataGet(sADC_BASE, ulADCTmp);
                for(i=0;i<4;i++)
                    pCmdStruct->tx_buf[i] = ulADCTmp[1]>>(i*8);
                pCmdStruct->ret = TRUE;
                pCmdStruct->tx_len = 4;
                break;
            case 2:       // I2 -> sA2
                // Enable IO clock
                xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(sA2));
                // Set IO direction and type.
                sA2PinTypeADC();
                // Read ADC value
                xADCDataGet(sADC_BASE, ulADCTmp);
                for(i=0;i<4;i++)
                    pCmdStruct->tx_buf[i] = ulADCTmp[2]>>(i*8);
                pCmdStruct->ret = TRUE;
                pCmdStruct->tx_len = 4;
                break;
            case 3:       // I3 -> sA3
                // Enable IO clock
                xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(sA3));
                // Set IO direction and type.
                sA3PinTypeADC();
                // Read ADC value
                xADCDataGet(sADC_BASE, ulADCTmp);
                for(i=0;i<4;i++)
                    pCmdStruct->tx_buf[i] = ulADCTmp[3]>>(i*8);
                pCmdStruct->ret = TRUE;
                pCmdStruct->tx_len = 4;
                break;
            default:
                pCmdStruct->ret = FALSE;
                pCmdStruct->tx_len = 0;
                break;
            }
            break;
        default:
            //
            // If new commands are added
            //
            pCmdStruct->ret = FALSE;
            pCmdStruct->tx_len = 0;
            break;
        }
    }
    return 0;
}

unsigned long IOOperation(CMDSTRUCT *pCmdStruct)
{
    return 0;
}

unsigned long SystemCtl(CMDSTRUCT *pCmdStruct)
{
    unsigned char *FwVer = "STM32 Firmware V1.0.0.0";
    unsigned char *ErrorInfo = "This Command is not supported by this firmware";
    unsigned char index = 0;

    if(pCmdStruct->cmd_3 == 0x02)
    {
        while(*FwVer)
        {
            pCmdStruct->tx_buf[index++] = *FwVer++;
        }
        pCmdStruct->ret = TRUE;
        pCmdStruct->tx_len = index;
    }
    else if(pCmdStruct->cmd_3 == 0x03)
    {
        xSysCtlReset();
    }
    else
    {
        while(*ErrorInfo)
        {
            pCmdStruct->tx_buf[index++] = *ErrorInfo++;
        }
        pCmdStruct->ret = FALSE;
        pCmdStruct->tx_len = index;
    }
    return 0;
}



unsigned long DigitalCompass(CMDSTRUCT *pCmdStruct)
{
    unsigned long angle = 0;
    unsigned char * p = (unsigned char *) &angle;

    int16_t Com_Data[3] = {0};
    Result  Res         = SUCCESS;

    if((pCmdStruct->cmd_1 == 0x40)&&(pCmdStruct->cmd_2 == 0x04))
    {
        switch(pCmdStruct->cmd_3)
        {
            case 0x01: // Init

                //Init Compass
                xSysCtlPeripheralReset2(HMC5883L_PIN_I2C_PORT);
                xSysCtlDelay(0xFFF);
                Res = HMC5883L_Init();
                if(Res == FAILURE)
                {
                    pCmdStruct->ret = FALSE;
                    pCmdStruct->tx_len = 0;
                    break;
                }

                /*
                //Configure Digital Compass
                Res = HMC5883L_Cfg(MODE_CONT | GAIN_1090 | SAMPLE_8 | DATA_RATE_15);
                if(Res == FAILURE)
                {
                    pCmdStruct->ret = FALSE;
                    pCmdStruct->tx_len = 0;
                    break;
                }

                SysCtlDelay(FFF);
                */
                pCmdStruct->ret = TRUE;
                pCmdStruct->tx_len = 0;
                break;

            case 0x02: // Read
/*
                //Configure Digital Compass
                Res = HMC5883L_Cfg(MODE_SIG | GAIN_1090 | SAMPLE_8 | DATA_RATE_15);
                if(Res == FAILURE)
                {
                    pCmdStruct->ret = FALSE;
                    pCmdStruct->tx_len = 0;
                    break;
                }

                Res = HMC5883L_DataGet(&Com_Data[0], &Com_Data[1], &Com_Data[2]);
                if(Res == FAILURE)
                {
                    pCmdStruct->ret = FALSE;
                    pCmdStruct->tx_len = 0;
                    break;
                }

                angle = (unsigned long) (atan2((double)Com_Data[0],(double)Com_Data[1])*(180/3.14159265)+180);

                //printf("%f\r\n", angle);
                pCmdStruct->tx_buf[0] = *p++;
                pCmdStruct->tx_buf[1] = *p++;
                pCmdStruct->tx_buf[2] = *p++;
                pCmdStruct->tx_buf[3] = *p++;

                pCmdStruct->ret = TRUE;
                pCmdStruct->tx_len = 4;
                break;
*/
            case 0x03: // Stop
                pCmdStruct->ret = TRUE;
                pCmdStruct->tx_len = 0;
                break;

            default:
                pCmdStruct->ret = FALSE;
                pCmdStruct->tx_len = 0;
                break;
        }
    }

    return (0);
}
