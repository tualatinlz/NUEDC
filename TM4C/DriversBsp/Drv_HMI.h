#ifndef __DRV_hmi_H
#define __DRV_hmi_H

//==引用
#include "SysConfig.h"

//==定义/声明

typedef struct
{
	u8 link_sta; //连接状态：0，未连接。1，已连接。
	u8 work_sta; //工作状态：0，异常。1，正常
	//
	u8 mode;   //飞行程序选择
	u8 oldmode;
	u16 angel;
  u8 update_cnt;  //数据更新计数。
} _hmi_st;

//飞控状态

//==数据声明
extern _hmi_st hmi;
//==函数声明
//static
static void HMI_DataAnl(uint8_t *data_buf, uint8_t num);

//public
void HMI_GetOneByte(uint8_t data);
void HMI_Check_State(float dT_s);
#endif
