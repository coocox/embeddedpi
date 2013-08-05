//*****************************************************************************
//
//! \file Frame.c
//! \brief protocal coder/decoder framework
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
#include "cookie.h"

#include "PortCfg.h"
#include "DriverCfg.h"
#include "Interface.h"

/*
typedef unsigned char uint8_t;
typedef unsigned int  uint32_t;
typedef          int   int32_t;
*/

#define CMD_START               0xA5
#define CMD_END                 0x5A
#define CMD_BYTE_1_MASK         0x3F

#define STATUS_IDLE             0
#define STATUS_R_CMD_1          1
#define STATUS_R_CMD_2          2
#define STATUS_R_CMD_3          3
#define STATUS_R_LEN            4
#define STATUS_R_PARAM          5
#define STATUS_R_END            6
#define STATUS_WAIT             7
#define STATUS_T_END            8
#define STATUS_T_LEN            9
#define STATUS_T_PARAM          10

#define DRV_IDLE                0
#define DRV_BUSY                1

#define FALSE                   0
#define TRUE                    1
#define BUF_MAX_SIZE            300

static uint8_t R_Msg[BUF_MAX_SIZE];
static uint8_t T_Msg[BUF_MAX_SIZE];

/*
typedef struct CmdStruct
{
    uint8_t ret;

    uint8_t cmd_1;
    uint8_t cmd_2;

    uint8_t cmd_3;
    uint8_t rx_len;
    uint8_t *rx_buf;

    uint8_t tx_len;
    uint8_t *tx_buf;

    //reserved for expand
}CMDSTRUCT;
*/
static CMDSTRUCT G_Cmd;

static void SetCmdDefault(void)
{
    G_Cmd.ret    = 0;

    G_Cmd.cmd_1  = 0;
    G_Cmd.cmd_2  = 0;

    G_Cmd.cmd_3  = 0;
    G_Cmd.rx_len = 0;
    G_Cmd.rx_buf = R_Msg;

    G_Cmd.tx_len = 0;
    G_Cmd.tx_buf = T_Msg;
};

//typedef uint32_t (*REG_FUN)(CMDSTRUCT *pCmdStruct);
typedef unsigned long (*REG_FUN)(CMDSTRUCT *pCmdStruct);

typedef struct RegStruct
{
    uint8_t status;
    REG_FUN pFun;
}REGSTRUCT;

static REGSTRUCT CMD_TBL[4][256];
static volatile uint32_t DrvStatus = DRV_IDLE;
static volatile uint32_t NewCmd    = FALSE;

static uint8_t  ACK_CODE[]  = {0xC9, 0xA3};
static uint8_t  NACK_CODE[] = {0xB6, 0xDC};

int32_t CmdRegister(uint8_t cmd_1, uint8_t cmd_2, REG_FUN pFun)
{
    uint8_t Index = (cmd_1 & 0xC0) >> 6;
    if(CMD_TBL[Index][cmd_2].status)
    {
        return (0); //Failure
    }
    else
    {
        CMD_TBL[Index][cmd_2].status = 1;
        CMD_TBL[Index][cmd_2].pFun = pFun;
        return (1); //Success
    }
}

int32_t CmdUnRegister(uint8_t cmd_1, uint8_t cmd_2)
{
    uint8_t Index = (cmd_1 & 0xC0) >> 6;
    if(!CMD_TBL[Index][cmd_2].status)
    {
        return (0); //Failure
    }
    else
    {
        CMD_TBL[Index][cmd_2].status = 0;
        CMD_TBL[Index][cmd_2].pFun = (REG_FUN)(0);
        return (1); //Success
    }
}

static void SysInit(void)
{
    xSysCtlClockSet(72000000, xSYSCTL_OSC_MAIN | xSYSCTL_XTAL_8MHZ);
    xSysCtlPeripheralEnable(SYSCTL_PERIPH_AFIO);
    xSysCtlDelay(10000);
}

static void SendResetAck()
{
    ComBlockWrite(&ACK_CODE[0], sizeof(ACK_CODE));
    ComTrigSendMsgInt();
}

static void Action(void)
{
    uint8_t Index = (G_Cmd.cmd_1 & 0xC0) >> 6;

    CMD_TBL[Index][G_Cmd.cmd_2].pFun(&G_Cmd);
}

static void Acknowledge()
{
    //Send Ack/Nack Code
    if(G_Cmd.ret) //Execute Success
    {
        ComBlockWrite( &ACK_CODE[0], sizeof(ACK_CODE));
    }
    else          //Execute Failure
    {
        ComBlockWrite( &NACK_CODE[0], sizeof(NACK_CODE));
    }

    //Send Respond Information
    if(G_Cmd.tx_len)
    {
        ComByteWrite(G_Cmd.tx_len);
        ComBlockWrite( G_Cmd.tx_buf, G_Cmd.tx_len);
    }

    //Triggle Interrupt
    ComTrigSendMsgInt();
}

//
// ----------------------------------------------------------------
// |                                                              |
// |          Protocal Format                                     |
// |                                                              |
// |--------------------------------------------------------------|
// |      |       |       |       |           |           |       |
// | HEAD | CMD_1 | CMD_2 | CMD_3 | PARAM_LEN | PARAM ... | TAIL  |
// |      |       |       |       |           |           |       |
// ----------------------------------------------------------------
//

static void WaitCmd(void)
{
    static uint8_t FsmStatus = STATUS_IDLE;
    static uint8_t index = 0;
    static uint8_t Ch = 0;

    while(1)
    {

        Ch = ComByteRead();

        switch(FsmStatus)
        {
            case STATUS_IDLE:
                {
                    SetCmdDefault();
                    if(Ch == CMD_START)
                    {
                        G_Cmd.cmd_1 = Ch;
                        FsmStatus = STATUS_R_CMD_1;
                    }
                    break;
                }

            case STATUS_R_CMD_1:
                {
                    if(Ch & CMD_BYTE_1_MASK)
                    {
                        FsmStatus = STATUS_IDLE;
                    }
                    else
                    {
                        G_Cmd.cmd_1 = Ch;
                        FsmStatus = STATUS_R_CMD_2;
                    }
                    break;
                }

            case STATUS_R_CMD_2:
                {
                    index = G_Cmd.cmd_1>>6;
                    if(CMD_TBL[index][Ch].status)
                    {
                        G_Cmd.cmd_2  = Ch;
                        FsmStatus = STATUS_R_CMD_3;
                    }
                    else
                    {
                        FsmStatus = STATUS_IDLE;
                    }
                    break;
                }

            case STATUS_R_CMD_3:
                {
                    G_Cmd.cmd_3 = Ch;
                    FsmStatus =  STATUS_R_LEN;
                    break;
                }

            case STATUS_R_LEN:
                {
                    G_Cmd.rx_len = Ch;
                    if(Ch)
                    {
                        index = 0;
                        FsmStatus =  STATUS_R_PARAM;
                    }
                    else
                    {
                        FsmStatus =  STATUS_R_END;
                    }
                    break;
                }

            case STATUS_R_PARAM:
                {
                    G_Cmd.rx_buf[index++] = Ch;
                    if(index == G_Cmd.rx_len)
                    {
                        index = 0;
                        FsmStatus =  STATUS_R_END;
                    }
                    break;
                }

            case STATUS_R_END:
                {
                    if(Ch != CMD_END)
                    {
                        FsmStatus =  STATUS_IDLE;
                    }
                    else
                    {
                        NewCmd    = TRUE;
                        DrvStatus = DRV_BUSY;
                        //FsmStatus = STATUS_WAIT;
                        FsmStatus =  STATUS_IDLE;

                        return;  //Find New Cmd
                    }
                    //break;
                }

          //case STATUS_WAIT:
          //    {
          //        if(DrvStatus == DRV_IDLE)
          //        {
          //            index = 0;
          //            FsmStatus = STATUS_T_END;
          //        }
          //        break;
          //    }

          //case STATUS_T_END:
          //    {
          //        if(G_Cmd.ret)  //success
          //        {
          //            SendByte(ACK_CODE[index++]);
          //        }
          //        else           //failure
          //        {
          //            SendByte(NACK_CODE[index++]);
          //        }

          //        if(index == 2)
          //        {
          //            if(G_Cmd.tx_len)
          //            {
          //                index = 0;
          //                FsmStatus = STATUS_T_LEN;
          //            }
          //            else
          //            {

          //                FsmStatus = STATUS_IDLE;
          //            }
          //        }
          //        break;
          //    }

          //case STATUS_T_LEN:
          //    {
          //        SendByte(G_Cmd.tx_len);
          //        FsmStatus = STATUS_T_PARAM;
          //        break;
          //    }

          //case STATUS_T_PARAM:
          //    {
          //        SendByte(G_Cmd.tx_buf[index++]);
          //        if(index == G_Cmd.tx_len)
          //        {
          //            index = 0;
          //            FsmStatus = STATUS_IDLE;
          //        }
          //        break;
          //    }
        }

    }
}

static void DebugInterface(void);

void main(void)
{

    SysInit();

    //Add your Module Function here
    CmdRegister(0x00, 0x00, SystemCtl);
    CmdRegister(0x40, 0x00, MotorShieldOperation);
    CmdRegister(0x40, 0x01, TinkerKitOperation);
    CmdRegister(0x40, 0x02, UltrasonicSensorOperation);
    CmdRegister(0x40, 0x04, DigitalCompass);

    ComPortInit();
    SendResetAck();

    //Debug entry
    //DebugInterface();

    while(1)
    {
        //Wait until a new command
        WaitCmd();

        //Dispatch new message
        Action();

        //Send Acknowledge information to RPI
        Acknowledge();
    }
}

static void DebugInterface(void)
{
    static unsigned long v = 0;
    static unsigned long last_v = 0;
    unsigned long ADValue = 0;

    SetCmdDefault();
    G_Cmd.cmd_1  = 0x40;
    G_Cmd.cmd_2  = 0x00;
    G_Cmd.cmd_3  = 0x05;
    Action();

    SteerMotorSpeedSet(999);

    SteerMotorRun(100);
    xSysCtlDelay(0xFFFFFF);
    SteerMotorRun(0);
    xSysCtlDelay(0xFFFFFF);


    while(1)
    {
        G_Cmd.cmd_1  = 0x40;
        G_Cmd.cmd_2  = 0x01;

        G_Cmd.cmd_3  = 0x03;
        G_Cmd.rx_len = 0x01;
        G_Cmd.rx_buf[0] = 0x03;
        Action();

        ADValue = *(unsigned long *)G_Cmd.tx_buf;
        v = ADValue/16;
        if((abs(v - last_v) >= 2) && (abs(v - last_v) <= 100))
        {
            SteerMotorRun(v);
        }
        last_v = v;
        xSysCtlDelay(0xFFFFF);
    }

    while(1);
}

