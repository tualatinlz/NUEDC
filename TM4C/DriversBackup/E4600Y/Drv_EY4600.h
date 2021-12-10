#ifndef __DRV_EY4600_H
#define __DRV_EY4600_H

//==引用
#include "SysConfig.h"

//==定义/声明

typedef struct
{
	//
	uint16_t rawdata[25];
  u8 update_cnt;  //数据更新计数。
} _ey4600_st;
//飞控状态

//==数据声明
extern _ey4600_st ey4600;
//==函数声明
//static
static void EY4600_DataAnl(uint8_t *data_buf, uint8_t num);
//public
void EY4600_GetOneByte(uint8_t data);
#endif
