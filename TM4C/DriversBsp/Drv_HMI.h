#ifndef __DRV_hmi_H
#define __DRV_hmi_H

//==引用
#include "SysConfig.h"

//==定义/声明



typedef struct
{
	u8 mode;   //飞行程序选择
	u8 oldmode;
  u8 update_cnt;  //数据更新计数。
	u8 page;
} _hmi_st;

//飞控状态

//==数据声明
extern _hmi_st hmi;
//==函数声明
//static
static void HMI_DataAnl(uint8_t *data_buf, uint8_t num);
//public
void HMI_GetOneByte(uint8_t data);
#endif
