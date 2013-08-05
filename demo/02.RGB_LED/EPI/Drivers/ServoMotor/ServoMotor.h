#ifndef __STEPSTEERMOTOR_H__
#define __STEPSTEERMOTOR_H__

#ifndef FALSE
#define FALSE                   0
#endif

#ifndef TURE
#define TRUE                    1
#endif

//StepMotor Step select, this value can be 8/4
//when set to 8, 0.9 degree one clock and 8 clock a period
//when set to 4, 1.8 degree one clock and 4 clock a period

#define MOTOR_STEP              8

//
//! 360 degrees stands for how many steps?
//
#if (MOTOR_STEP == 8)
#define AngleToStepCoeff        400
#elif (MOTOR_STEP == 4)
#define AngleToStepCoeff        200
#else
#error Please select stepmotor parameter
#endif

//extern unsigned long MotorCurrentGet(unsigned char ucMotorNumber);
extern void SteerMotorRun(signed long sDegree);
extern void SteerMotorSpeedSet(unsigned long ulSpeed);

#endif
