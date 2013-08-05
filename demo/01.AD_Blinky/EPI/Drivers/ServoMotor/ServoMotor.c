#include <stdio.h>
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
#include "ServoMotor.h"
#include "ArduinoMotorShield.h"

extern volatile StepMotorStruct StepMotor;

//*****************************************************************************
//
//! \brief Set ServoMotor speed
//!
//! \param sDegree is the target angle you want to rotate to.
//!   note: -360 <= sDegree <= 360
//!
//! \return none.
//
//*****************************************************************************
void SteerMotorRun(signed long sDegree)
{
    signed long TargetPost = 0;
    signed long delta = 0;

    xIntDisable(xINT_TIMER4);

    //1: calculate current postion and target postion
    StepMotor.CurrentPost %= AngleToStepCoeff;
    TargetPost = (sDegree%360)*AngleToStepCoeff/360;

    //2: calculate offset
    delta = TargetPost - StepMotor.CurrentPost;
    if(delta > AngleToStepCoeff/2)
    {
        delta -= AngleToStepCoeff;
    }
    else if(delta < -AngleToStepCoeff/2)
    {
        delta += AngleToStepCoeff;
    }

    //target = now, there is need to rotate
    if(abs(delta) <= 0)
    {
        xIntEnable(xINT_TIMER4);
        return ;
    }

    //3: calculate offset and decide the best route
    StepMotor.ulStepCnt = 0;
    if(delta >= 0)
    {
        StepMotor.ucDir = 0;
        StepMotor.ulTotalStep = delta;
    }
    else
    {
        StepMotor.ucDir = 1;
        StepMotor.ulTotalStep = -delta;
    }

    //Start
    xGPIOSPinWrite(sD3, 1);
    xGPIOSPinWrite(sD11, 1);
    xIntEnable(xINT_TIMER4);

}

//*****************************************************************************
//
//! \brief Set ServoMotor speed
//!
//! \param ulSpeed is the speed of stepmotor
//!  note: 
//!       1: uints ms
//!       2: this value must be < 1000
//! when ulSpeed is set to 999, that's mean one step need 1ms, when select 4 step
//! mode,stepmotor will cost 200*1ms = 200 ms to rotate 360 degree.
//!
//! \return none.
//
//*****************************************************************************
void SteerMotorSpeedSet(unsigned long ulSpeed)
{
    if(ulSpeed == 0)
    {
        AMSStepMotorStop();
        return;
    }
    if(ulSpeed < 1000)
    {
        StepMotor.ulStepInterval = (1000 -ulSpeed);
    }
}

