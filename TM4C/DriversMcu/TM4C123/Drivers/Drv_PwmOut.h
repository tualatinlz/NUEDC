#ifndef _DRV_PWMOUT_H_
#define _DRV_PWMOUT_H_

#include "sysconfig.h"

void DrvPwmOutInit(void);
void DrvMotorPWMSet(int16_t pwm[4]);
void DrvServoPWMSet(int16_t pwm[4]);
void DrvHeatSet(u16 val);

#endif
