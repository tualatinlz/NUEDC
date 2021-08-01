#ifndef __DRV_K210_H
#define __DRV_K210_H

//==引用
#include "SysConfig.h"

//==定义/声明

typedef struct
{
	//
	u8 	number; 			//数字识别结果
	u16 angel;			 	//巡线角度
	u8  leftorright; 	//自转方向
	u8  xoffset;			 //左右偏移量
	u8  xdirection; 	//x轴移动方向 右为0
	u8  yoffset;			 //左右偏移量
	u8  ydirection; 	//y轴移动方向 右为0
  u8  update_cnt;  	//数据更新计数
	u16  distance;    //超声波测距输出
	u8 	mode;        	//k210工作模式选择
	//0：  1：  2：
} _k210_st;

//飞控状态

//==数据声明
extern _k210_st k210;
//==函数声明
//static
static void K210_DataAnl(uint8_t *data_buf, uint8_t num);

//public
void K210_GetOneByte(uint8_t data);
void K210_Check_State(float dT_s);
#endif
