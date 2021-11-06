#ifndef __USER_TASK_H
#define __USER_TASK_H

#include "SysConfig.h"

void delay20(void);
void rotate(u16 r,u8 direction);
void UserTask_OneKeyCmd(void);
void spreadP(u8 height);
void spreadPU(u8 height);
void customF(void);
void blink(void);
void test(u16 height,u16 dh);
//void Take_Photos(void);
#endif
