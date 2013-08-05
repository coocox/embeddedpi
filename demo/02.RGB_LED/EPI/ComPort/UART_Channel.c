//*****************************************************************************
//
//! \file UART_Channle.c
//! \brief UART communication port interface
//! \version 1.0
//! \date 04/7/2013
//! \author CooCox
//! \copy
//!
//! Copyright (c)  2013, CooCox
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "xhw_types.h"
#include "xhw_memmap.h"
#include "xhw_ints.h"
#include "xhw_nvic.h"
#include "xcore.h"
#include "xsysctl.h"
#include "xhw_gpio.h"
#include "xgpio.h"
#include "xhw_uart.h"
#include "xuart.h"
#include "xhw_i2c.h"
#include "xhw_sysctl.h"
#include "xspi.h"
//#include "cookie.h"

#include "UART_Channel.h"
#include "ringbuf.h"

#define BUF_MAX_SIZE            300

static tRingBufObject Tx_FIFO;
static tRingBufObject Rx_FIFO;

static uint8_t RecvBuf[BUF_MAX_SIZE];
static uint8_t SendBuf[BUF_MAX_SIZE];

static void _FIFOInit(void)
{
    unsigned int i;
    for(i = 0; i < BUF_MAX_SIZE; i++){
        RecvBuf[i] = 0; SendBuf[i] = 0;
    }

    RingBufInit(&Tx_FIFO, SendBuf, BUF_MAX_SIZE);
    RingBufInit(&Rx_FIFO, RecvBuf, BUF_MAX_SIZE);
}

static unsigned long IntCallback(void *pvCBData,
                          unsigned long ulEvent,
                          unsigned long ulMsgParam,
                          void *pvMsgData)
{

    if(ulMsgParam & 0x40) //Tranmission Finish
    {
        /*
        xHWREG(USART1_BASE + USART_DR) = (uint32_t) TxBuf[tx_index++];
        if(tx_index == 14)
        {
            UARTIntDisable(USART1_BASE, UART_INT_TXE);
            tx_index = 0;
        }
        */

        if(!RingBufEmpty(&Tx_FIFO))
        {
            xHWREG(USART1_BASE + USART_DR) = RingBufReadOne(&Tx_FIFO);
        }
        else
        {
            UARTIntDisable(USART1_BASE, UART_INT_TC);
        }
    }

    if(ulMsgParam & 0x20) //RXNE
    {
        unsigned char tmp = (unsigned char)xHWREG(USART1_BASE + USART_DR);
        /*RxBuf[rx_index++] = (uint8_t) tmp;*/

        if(!RingBufFull(&Rx_FIFO))
        {
            RingBufWriteOne(&Rx_FIFO, tmp);
        }
    }

    return (0);
}

static void _UARTInit(void)
{
    //
    // Configure UART Pin
    // PA9-->TX   PA10-->RX
    //
    xSysCtlPeripheralEnable(xSYSCTL_PERIPH_GPIOA);
    xSPinTypeUART(UART1TX, PA9);
    xSPinTypeUART(UART1RX, PA10);

    //
    // Configure UART1 115200 8-N-1
    //
    xSysCtlPeripheralEnable(xSYSCTL_PERIPH_UART1);
    xUARTConfigSet(USART1_BASE, 115200, xUART_CONFIG_WLEN_8 |
    		                            xUART_CONFIG_STOP_1 |
    		                            xUART_CONFIG_PAR_EVEN/*xUART_CONFIG_PAR_NONE*/);
    UARTIntCallbackInit(USART1_BASE, IntCallback);
    xIntEnable(xINT_UART1);
    UARTIntEnable(USART1_BASE, UART_INT_RXNE);

    xSysCtlDelay(5000);
    xUARTEnable(USART1_BASE, xUART_BLOCK_UART | xUART_BLOCK_TX | xUART_BLOCK_RX);

    //Triggle Interrupt
    //xUARTCharPut(USART1_BASE, 0);
    //UARTIntEnable(USART1_BASE, UART_INT_TC);

}

void UART_PortInit(void)
{
    _FIFOInit();
    _UARTInit();
}


void UART_BlockWrite(uint8_t * Addr, uint32_t Size)
{
    RingBufWrite(&Tx_FIFO, Addr, Size);
}

void UART_ByteWrite(uint8_t Ch)
{
    while(RingBufFull(&Tx_FIFO));
    RingBufWriteOne(&Tx_FIFO, Ch);
}

uint8_t UART_ByteRead(void)
{
    uint8_t Ch = 0;

    while(RingBufEmpty(&Rx_FIFO));
    Ch = RingBufReadOne(&Rx_FIFO);

    return(Ch);
}

void UART_TrigSendMsgInt(void)
{
    //Triggle Interrupt
    xUARTCharPut(USART1_BASE, 0x00);
    UARTIntEnable(USART1_BASE, UART_INT_TC);
}
