#ifndef _LED_H_
#define _LED_H_

#include "xhw_types.h"
#include "xhw_ints.h"
#include "xhw_nvic.h"
#include "xhw_memmap.h"
#include "xdebug.h"
#include "xcore.h"
#include "xhw_sysctl.h"
#include "xsysctl.h"
#include "xhw_gpio.h"
#include "xgpio.h"
#include "xhw_timer.h"
#include "xpwm.h"
#include "cookie_cfg.h"
#include "cookie.h"

#define R_PWM   sPWMB_BASE
#define R_Pin	sD3
#define G_PWM   sPWMA_BASE
#define G_Pin   sD9
#define B_PWM   xPWME_BASE
#define B_Pin   PB9

extern void RGB_Init(void);
extern void RGBColorSet(unsigned long ulColor, unsigned long ulVal);

#endif