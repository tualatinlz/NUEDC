#ifndef __DRV_hwt101_H
#define __DRV_hwt101_H

//==引用
#include "SysConfig.h"

//==定义/声明

typedef struct
{
	u16 angel;			 	//角度
	u16  distance;    //超声波测距输出
} _hwt101_st;

//飞控状态

//==数据声明
extern _hwt101_st hwt101;
//==函数声明
//static
static void HWT101_DataAnl(uint8_t *data_buf, uint8_t num);

//public
void HWT101_GetOneByte(uint8_t data);
void HWT101_Check_State(float dT_s);
#endif
