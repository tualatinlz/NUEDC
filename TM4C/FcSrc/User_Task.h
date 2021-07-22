#ifndef __USER_TASK_H
#define __USER_TASK_H

#include "SysConfig.h"

void UserTask_OneKeyCmd(void);
void UserTask_FollowLine(u8 wholeLength);
void UserTask_FollowLineN(u8 wholeLength);
void test(u16 height,u16 dh);
void delay20(void);
void rotate(u16 r,u8 direction);
#endif
