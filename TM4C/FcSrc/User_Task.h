#ifndef __USER_TASK_H
#define __USER_TASK_H

#include "SysConfig.h"

void delay20(void);
void rotate(u16 r,u8 direction);
void UserTask_OneKeyCmd(void);
void solveMaze(u8 height);
void goMaze(u8 height);
void test(u16 height,u16 dh);
void Take_Photos(void);
#endif
