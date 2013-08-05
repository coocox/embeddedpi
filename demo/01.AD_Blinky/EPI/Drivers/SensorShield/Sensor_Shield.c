//*****************************************************************************
//
//! \file Sensor_Shield.c
//! \brief Control for Sensor Shield.
//! \version 2.1.1.0
//! \date 20/3/2013
//! \author CooCox
//! \copy
//!
//! Copyright (c)  2011, CooCox
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
//
//*****************************************************************************

#include "xhw_types.h"
#include "xcore.h"
#include "xhw_ints.h"
#include "xhw_memmap.h"
#include "xhw_nvic.h"
#include "xhw_sysctl.h"
#include "xhw_gpio.h"
#include "xdebug.h"
#include "xsysctl.h"
#include "xgpio.h"
#include "xadc.h"
#include "xuart.h"
#include "cookie.h"
#include "Sensor_shield.h"
#if(SENSOR_SHIELD_I2C_USED > 0)
#include "xhw_i2c.h"
#include "xi2c.h"
#endif



//*****************************************************************************
//
//! \brief Send a buffer to uart.
//!
//! \param ulBase is the base address of the UART port.
//! \param pucStr is the point of data will be sent.
//! \param ulLen is the lenth of data will be sent.
//!
//! Send a buffer to uart
//!
//! \return None
//
//*****************************************************************************
void
SensorShieldUARTBufferPut(unsigned long ulBase, unsigned char *pucStr, unsigned long ulLen)
{
    unsigned long i;
    for(i=0; i<ulLen; i++)
    {
        xUARTCharPut(ulBase, *pucStr++);
    }
}

//*****************************************************************************
//
//! \brief get data from UART.
//!
//! \param pucBuf is the point of data will be save.
//! \param ulLen is the lenth of data will be save.
//!
//! get data from UART.
//!
//! \return the lenth of data
//
//*****************************************************************************
unsigned long
SensorShieldUARTBufferGet(unsigned long ulBase, unsigned char *pucBuf, unsigned long  ulLen)
{
    unsigned long i = 0;
    for (i = 0; i < ulLen; i++)
    {
        pucBuf[i] = xUARTCharGet(ulBase);
    }
    return ulLen;
}

//*****************************************************************************
//
//! \brief ADC callback interrupt,it handle the data when the Moisture Sensor has
//! overflow the threshold.
//!
//! \param None
//!
//! \return none
//
//*****************************************************************************
unsigned long ADCCallback(void *pvCBData,  unsigned long ulEvent,
                                       unsigned long ulMsgParam,void *pvMsgData)
{

    return 0;
}

//*****************************************************************************
//
//! \brief Initialize the sensor shield board.
//!
//! \param None.
//!
//! Initialize the sensor shield board.
//!
//! \return None
//
//*****************************************************************************
void
SensorShieldInit(void)
{
	xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(SENSOR_SHIELD_O0));
	xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(SENSOR_SHIELD_O1));
	xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(SENSOR_SHIELD_O2));
	xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(SENSOR_SHIELD_O3));
	xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(SENSOR_SHIELD_O4));
	xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(SENSOR_SHIELD_O5));
	xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(SENSOR_SHIELD_I0));
	xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(SENSOR_SHIELD_I1));
	xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(SENSOR_SHIELD_I2));
	xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(SENSOR_SHIELD_I3));
	xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(SENSOR_SHIELD_I4));
	xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(SENSOR_SHIELD_I5));
#if(SENSOR_SHIELD_OUT_USED > 0)

    xGPIOSPinTypeGPIOOutput(SENSOR_SHIELD_O0);
    xGPIOSPinTypeGPIOOutput(SENSOR_SHIELD_O1);
    xGPIOSPinTypeGPIOOutput(SENSOR_SHIELD_O2);
    xGPIOSPinTypeGPIOOutput(SENSOR_SHIELD_O3);
    xGPIOSPinTypeGPIOOutput(SENSOR_SHIELD_O4);
    xGPIOSPinTypeGPIOOutput(SENSOR_SHIELD_O5);
    //xSysCtlPeripheralEnable2();
#endif

#if((SENSOR_SHIELD_IN0_USED == SENSOR_SHIELD_IN_ANALOG) ||                    \
    (SENSOR_SHIELD_IN1_USED == SENSOR_SHIELD_IN_ANALOG) ||                    \
    (SENSOR_SHIELD_IN2_USED == SENSOR_SHIELD_IN_ANALOG) ||                    \
    (SENSOR_SHIELD_IN3_USED == SENSOR_SHIELD_IN_ANALOG) ||                    \
    (SENSOR_SHIELD_IN4_USED == SENSOR_SHIELD_IN_ANALOG) ||                    \
    (SENSOR_SHIELD_IN5_USED == SENSOR_SHIELD_IN_ANALOG))

    xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(sD45));

    //
    // Set ADCCLK prescaler, ADCCLK=PCLK2(max 72MHz)/Div(Div:2,4,6,8)
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
    //
    xADCStepConfigure(sADC_BASE, 0, sADC_CH0);
    xADCStepConfigure(sADC_BASE, 1, sADC_CH1);
    xADCStepConfigure(sADC_BASE, 2, sADC_CH2);
    xADCStepConfigure(sADC_BASE, 3, sADC_CH3);

#endif

#if(SENSOR_SHIELD_IN0_USED == SENSOR_SHIELD_IN_ANALOG)
    sA0PinTypeADC();
#elif (SENSOR_SHIELD_IN0_USED == SENSOR_SHIELD_IN_DIGITAL)
    xGPIOSPinTypeGPIOInput(SENSOR_SHIELD_I0);
#endif

#if(SENSOR_SHIELD_IN1_USED == SENSOR_SHIELD_IN_ANALOG)
    sA1PinTypeADC();
#elif (SENSOR_SHIELD_IN1_USED == SENSOR_SHIELD_IN_DIGITAL)
    xGPIOSPinTypeGPIOInput(SENSOR_SHIELD_I1);
#endif

#if(SENSOR_SHIELD_IN2_USED == SENSOR_SHIELD_IN_ANALOG)
    sA2PinTypeADC();
#elif (SENSOR_SHIELD_IN2_USED == SENSOR_SHIELD_IN_DIGITAL)
    xGPIOSPinTypeGPIOInput(SENSOR_SHIELD_I2);
#endif

#if(SENSOR_SHIELD_IN3_USED == SENSOR_SHIELD_IN_ANALOG)
    sA3PinTypeADC();
#elif (SENSOR_SHIELD_IN3_USED == SENSOR_SHIELD_IN_DIGITAL)
    xGPIOSPinTypeGPIOInput(SENSOR_SHIELD_I3);
#endif

#if(SENSOR_SHIELD_IN4_USED == SENSOR_SHIELD_IN_ANALOG)||  \
       (SENSOR_SHIELD_IN4_USED == SENSOR_SHIELD_IN_DIGITAL)
    xGPIOSPinTypeGPIOInput(SENSOR_SHIELD_I4);
#endif

#if(SENSOR_SHIELD_IN5_USED == SENSOR_SHIELD_IN_ANALOG)||  \
	   (SENSOR_SHIELD_IN5_USED == SENSOR_SHIELD_IN_DIGITAL)
    xGPIOSPinTypeGPIOInput(SENSOR_SHIELD_I5);
#endif

#if(SENSOR_SHIELD_I2C_USED > 0)
    //
    // Enable the GPIOx port which is connected with I2C
    //
    xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(SENSOR_SHIELD_TWI_SDA));
    xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(SENSOR_SHIELD_TWI_SCK));

    //
    // Enable the I2Cx which is connected with device
    //
    xSysCtlPeripheralEnable2(sI2C_BASE);

    //
    // Set BH1750_PIN_I2C_CLK as CLK
    //
    sPinTypeI2C(sI2C_BASE);

    //
    // Configure MCU as a master device and Set Clcok Rates
    //
    xI2CMasterInit(sI2C_BASE, 100000);
    xI2CMasterEnable(sI2C_BASE);
#endif

#if(SENSOR_SHIELD_UART_USED > 0)
    xSysCtlPeripheralEnable2(sUART_BASE);
//    xSysCtlPeripheralClockSourceSet(xSYSCTL_UART1_MAIN, 1);
    sPinTypeUART(sUART_BASE);
    xUARTConfigSet(sUART_BASE, 115200, (xUART_CONFIG_WLEN_8 |
                                        xUART_CONFIG_STOP_1 |
                                        xUART_CONFIG_PAR_NONE));
#endif
}

//*****************************************************************************
//
//! \brief Get ADC value.
//!
//! \param ulInputChannel The analog input channel (0~3).
//!
//! Get one channel ADC convert value.
//!
//! \return None
//
//*****************************************************************************
unsigned long
ADCValueGet(unsigned char ucInputChannel)
{
	unsigned long ulADCBuffer[4];

	//
	// Check valid of ucInputChannel
	//
	xASSERT(ucInputChannel <= SENSOR_SHIELD_AI3);

	//
	// Get ADC value
	//
	xADCDataGet(sADC_BASE, ulADCBuffer);

	//
	// Return specified channel value
	//
	return(ulADCBuffer[ucInputChannel]);
}
