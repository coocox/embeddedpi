//*****************************************************************************
//
//! \file PortCfg.h
//! \brief communication port interface file
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


#ifndef _PORT_CFG_H_
#define _PORT_CFG_H_

#define PORT_I2C                0
#define PORT_UART               1
#define PORT_SPI                2

//Note: Please select your communication port first
#define COM_PORT                PORT_UART

#if (COM_PORT == PORT_I2C)
#include "I2C_Channel.h"

#define ComPortInit             I2C_PortInit
#define ComBlockWrite           I2C_BlockWrite
#define ComByteWrite            I2C_ByteWrite
#define ComByteRead             I2C_ByteRead
#define ComTrigSendMsgInt       I2C_TrigSendMsgInt

#elif (COM_PORT == PORT_UART)
#include "UART_Channel.h"

#define ComPortInit             UART_PortInit
#define ComBlockWrite           UART_BlockWrite
#define ComByteWrite            UART_ByteWrite
#define ComByteRead             UART_ByteRead
#define ComTrigSendMsgInt       UART_TrigSendMsgInt

#elif (COM_PORT == PORT_SPI)
#include "SPI_Channel.h"

#define ComPortInit             SPI_PortInit
#define ComBlockWrite           SPI_BlockWrite
#define ComByteWrite            SPI_ByteWrite
#define ComByteRead             SPI_ByteRead
#define ComTrigSendMsgInt       SPI_TrigSendMsgInt

#else
#error "ERROR: Please select communication port first"
#endif

#endif
