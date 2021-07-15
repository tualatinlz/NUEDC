#ifndef __DRV_K210_H
#define __DRV_K210_H

//==引用
#include "SysConfig.h"

//==定义/声明

typedef struct
{
	//
	u8 	number; 
	u16 angel;
  u8 update_cnt;  //数据更新计数。
} _k210_st;

typedef union {
	u8 byte[3];
	_k210_st st_data;
} _k210_un;

//飞控状态

//==数据声明
extern _k210_st k210;
extern _k210_un ext_k210;
//==函数声明
//static
static void K210_DataAnl(uint8_t *data_buf, uint8_t num);

//public
void K210_GetOneByte(uint8_t data);
void K210_Check_State(float dT_s);
#endif
