//*****************************************************************************
//
//! \file HC_SR04.c
//! \brief Driver for ultrasonic distance measure module.
//! \version V2.1.1.0
//! \date 1/28/2013
//! \author CooCox
//! \copy
//!
//! Copyright (c)  2012, CooCox
//! All rights reserved.
//!
//! Redistribution and use in source and binary forms, with or without
//! modification, are permitted provided that the following conditions
//! are met:
//!
//!     * Redistributions of source code must retain the above copyright
//! notice, this list of conditions and the following disclaimer.
//!     * Redistributions in binary form must reproduce the above copyright
//! notice, this list of conditions and the following disclaimer in the
//! documentation and/or other materials provided with the distribution.
//!     * Neither the name of the <ORGANIZATION> nor the names of its
//! contributors may be used to endorse or promote products derived
//! from this software without specific prior written permission.
//!
//! THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//! AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//! IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//! ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
//! LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//! CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//! SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//! INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//! CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//! ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
//! THE POSSIBILITY OF SUCH DAMAGE.
//*****************************************************************************
#include "xhw_gpio.h"
#include "xhw_ints.h"
#include "xhw_memmap.h"
#include "xhw_nvic.h"
#include "xhw_sysctl.h"
#include "xhw_timer.h"
#include "xhw_types.h"
#include "xcore.h"
#include "xgpio.h"
#include "xsysctl.h"
#include "xtimer.h"
#include "HC_SR04.h"

#include <stdio.h>


extern unsigned long Timer4Callback(void *pvCBData,  unsigned long ulEvent,
                                       unsigned long ulMsgParam,
                                       void *pvMsgData);
static unsigned long sulSysClk = 0;
static unsigned char sucHCSR04Status = PERIPHERAL_STATUS_NOINIT;
volatile static unsigned short susDistance = 0;

//*****************************************************************************
//
//! \brief Timer capture interrupt service function
//!
//! \param pvCBData not used
//! \param ulEvent not used
//! \param ulMsgParam not used
//! \param pvMsgData not used
//!
//! This function is entered when the rising edge or falling edge of the feedback
//! signal from the module is captured by the timer. The function will record the
//! count value of each capture to calculate the time.
//!
//! \return zero.
//
//*****************************************************************************
unsigned long HCSR04TimerIntCallback(void *pvCBData,  unsigned long ulEvent,
                                       unsigned long ulMsgParam,
                                       void *pvMsgData)
{
    static unsigned char sucCaptureCnt = 0;
    static unsigned long sulCapture1, sulCapture2;

    if(sucCaptureCnt == 0)
    {
        //
        // save last count value
        //
        sulCapture1 = xTimerMatchGet(HCSR04_TIMER_BASE, HCSR04_TIMER_CHANNEL);
        //
        // switch capture edge to falling edge
        //
        xTimerCaptureEdgeSelect(HCSR04_TIMER_BASE, HCSR04_TIMER_CHANNEL, xTIMER_CAP_FALLING);
        //
        // capture times increase
        //
        sucCaptureCnt++;
    }
    else
    {
        sucCaptureCnt = 0;
        //
        // get current count value and clear interrupt flag
        //
        sulCapture2  = xTimerMatchGet(HCSR04_TIMER_BASE, HCSR04_TIMER_CHANNEL);
        //
        // first time set capture edge to rising edge to get the start count value
        //
        xTimerCaptureEdgeSelect(HCSR04_TIMER_BASE, HCSR04_TIMER_CHANNEL, xTIMER_CAP_RISING);

        //
        // calculate the time between two capture
        //
        if( sulCapture2 > sulCapture1 )
        {
            susDistance = sulCapture2 - sulCapture1;
        }
        else
        {
            susDistance = MAX_RELOAD_VAL + 1 + sulCapture2 - sulCapture1;
        }
        susDistance = susDistance / 58;
        if(sucHCSR04Status == PERIPHERAL_STATUS_STOP)
        {
            //
            // Stop the timer
            //
            xTimerStop(HCSR04_TIMER_BASE, HCSR04_TIMER_CHANNEL);
            xIntDisable( HCSR04_TIMER_INT);
        }
    }



    return 0;
}

//*****************************************************************************
//
//! \brief Get distance
//!
//! \param None
//!
//! This function is to return the distance measured by HC-SR04.
//! \note the trigger signal is defined in other files. If no trigger this
//! function will always return 0.
//!
//! \return susDistance The distance in unit of cm.
//
//*****************************************************************************
unsigned short HCSR04GetDistance(void)
{
    return susDistance;
}

//*****************************************************************************
//
//! \brief Get initialized for measuring distance
//!
//! \param None
//!
//! This function is to initialize the IO port and timer to get ready for measure
//!
//! \return None.
//
//*****************************************************************************
void HCSR04Init(void)
{

    xSysCtlPeripheralEnable(SYSCTL_PERIPH_AFIO);
    xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(HCSR04_TRIG_PIN));
    xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(HCSR04_ECHO_PIN));

    xGPIOSPinDirModeSet(HCSR04_TRIG_PIN, xGPIO_DIR_MODE_OUT);
    xGPIOSPinWrite( HCSR04_TRIG_PIN, 0 );
    xGPIOSPinDirModeSet(HCSR04_ECHO_PIN, xGPIO_DIR_MODE_IN);
    xGPIOSPinWrite( HCSR04_ECHO_PIN, 0 );

    //
    // Timer 3 input channel remap
    //
    xGPIOPinConfigure(TIM3CH2(PIN), HCSR04_ECHO_PIN);

    xSysCtlPeripheralEnable2(HCSR04_TIMER_BASE);

    sulSysClk = xSysCtlClockGet();

    //
    // Configure the timer as capture mode
    //
    xTimerInitConfig(HCSR04_TIMER_BASE, HCSR04_TIMER_CHANNEL,
            xTIMER_MODE_CAPTURE | xTIMER_COUNT_UP, 1000000);

    //
    // first time set capture edge to rising edge to get the start count value
    //
    xTimerCaptureEdgeSelect(HCSR04_TIMER_BASE, HCSR04_TIMER_CHANNEL, xTIMER_CAP_RISING);

    //
    // Capture mode: capture
    //
    xTimerCaptureModeSet(HCSR04_TIMER_BASE,
            HCSR04_TIMER_CHANNEL,
            xTIMER_CAP_MODE_CAP);

    //
    // Set reload value
    //
    xTimerLoadSet(HCSR04_TIMER_BASE, HCSR04_TIMER_CHANNEL, MAX_RELOAD_VAL);

    //
    // Set prescaler value to get 1us internal count clock
    //
    xTimerPrescaleSet(HCSR04_TIMER_BASE, HCSR04_TIMER_CHANNEL, sulSysClk / 1000000);

    //
    // Set and enable capture interrupt
    //
    xTimerIntCallbackInit(HCSR04_TIMER_BASE, HCSR04TimerIntCallback);
    xTimerIntEnable(HCSR04_TIMER_BASE, HCSR04_TIMER_CHANNEL, xTIMER_INT_CAP_EVENT);
    xIntPrioritySet(HCSR04_TIMER_INT, 2);  //a higher priority
    xIntEnable( HCSR04_TIMER_INT);
    //
    // start to count
    //
    xTimerStart(HCSR04_TIMER_BASE, HCSR04_TIMER_CHANNEL);

#if 0
    //
    // System drive timer
    //
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
#endif

    sucHCSR04Status = PERIPHERAL_STATUS_RUNNING;
}

//*****************************************************************************
//
//! \brief Stop measuring distance
//!
//! \param None
//!
//! \return None.
//
//*****************************************************************************
void HCSR04Stop()
{
    sucHCSR04Status = PERIPHERAL_STATUS_NOINIT;
}
