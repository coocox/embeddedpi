#include "rgb.h"

void RGB_Init(void)
{	
    xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(R_Pin));
    xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(G_Pin));
    xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(B_Pin));

    xSysCtlPeripheralEnable(SYSCTL_PERIPH_AFIO);
    xSysCtlPeripheralEnable2(R_PWM);
    xSysCtlPeripheralEnable2(G_PWM);
    xSysCtlPeripheralEnable2(B_PWM);
    
    sD3PinTypePWM();
    sD9PinTypePWM();
    xSPinTypeTimer(TIM4CH4(APP), B_Pin);
    
    xGPIOSPinDirModeSet(R_Pin, GPIO_DIR_MODE_HWSTD);
    xGPIOSPinDirModeSet(G_Pin, GPIO_DIR_MODE_HWSTD);
    xGPIOSPinDirModeSet(B_Pin, GPIO_DIR_MODE_HWSTD);

    //
    // Set invert, dead zone and mode
    //
    xPWMInitConfigure(R_PWM, xPWM_CHANNEL0, xPWM_TOGGLE_MODE |
                                            xPWM_OUTPUT_INVERTER_DIS |
                                            xPWM_DEAD_ZONE_DIS);
    xPWMInitConfigure(G_PWM, xPWM_CHANNEL0, xPWM_TOGGLE_MODE |
                                            xPWM_OUTPUT_INVERTER_EN |
                                            xPWM_DEAD_ZONE_DIS);
    xPWMInitConfigure(B_PWM, xPWM_CHANNEL3, xPWM_TOGGLE_MODE |
                                            xPWM_OUTPUT_INVERTER_EN |
                                            xPWM_DEAD_ZONE_DIS);
												 
    //
    // Set CNR, Prescale and Divider
    //
    xPWMFrequencySet(R_PWM, xPWM_CHANNEL0, 50);
    xPWMFrequencySet(G_PWM, xPWM_CHANNEL0, 100);
    xPWMFrequencySet(B_PWM, xPWM_CHANNEL3, 50);
	
    //
    // Set CMR
    //
    xPWMDutySet(R_PWM, xPWM_CHANNEL0, 0);
    xPWMDutySet(G_PWM, xPWM_CHANNEL0, 0);
    xPWMDutySet(B_PWM, xPWM_CHANNEL3, 0);
	
    //
    // Set output enable
    //
    xPWMOutputEnable(R_PWM, xPWM_CHANNEL0);
    xPWMOutputEnable(G_PWM, xPWM_CHANNEL0);
    xPWMOutputEnable(B_PWM, xPWM_CHANNEL3);
	
    //
    // start PWM
    //
    xPWMStart(R_PWM, xPWM_CHANNEL0);
    xPWMStart(G_PWM, xPWM_CHANNEL0);
    xPWMStart(B_PWM, xPWM_CHANNEL3);
}

void
RGBColorSet(unsigned long ulColor, unsigned long ulVal)
{
    //
    // Check the arguments.
    //
    xASSERT((ulColor == R_PWM) || (ulColor == G_PWM) || (ulColor == B_PWM));

    if(ulColor != B_PWM)
    {
        //
        // Disable the pwm channel
        //
        xPWMStop(ulColor, xPWM_CHANNEL0);

        //
        // Set the pwm duty as 50% .
        //
        xPWMDutySet(ulColor, xPWM_CHANNEL0, ulVal);

        //
        // Enable the pwm channel
        //
        xPWMStart(ulColor, xPWM_CHANNEL0);
    } else {
        //
        // Disable the pwm channel
        //
        xPWMStop(ulColor, xPWM_CHANNEL3);

        //
        // Set the pwm duty as 50% .
        //
        xPWMDutySet(ulColor, xPWM_CHANNEL3, ulVal);

        //
        // Enable the pwm channel
        //
        xPWMStart(ulColor, xPWM_CHANNEL3);
    }

}
