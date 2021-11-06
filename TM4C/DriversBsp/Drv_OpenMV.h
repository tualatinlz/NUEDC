#ifndef __DRV_OpenMV_H
#define __DRV_OpenMV_H

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
	int  xtotal;
	u8  xdirection; 	//x轴移动方向 右为0
	u8  ytotal;
	u8  yoffset;			 //左右偏移量
	u8  ydirection; 	//y轴移动方向 右为0
  u8  update_cnt;  	//数据更新计数
	u8  ready;				//到杆子指定位置了
	u8  distance;			//超声波距离
} _openmv_st;

typedef struct
{
	u8 	mode;        	//openmv工作模式选择
	u8  go;
	u8  l1;
	u8  l2;
	u8 	a1;
	u8 	a2;
	u8 	b1;
	u8 	b2;
} _openmv_config;
//状态

//==数据声明
extern _openmv_st openmv;
extern _openmv_config openmv_cfg;
//==函数声明
//static
static void OpenMV_DataAnl(uint8_t *data_buf, uint8_t num);

//public
void OpenMV_GetOneByte(uint8_t data);
void OpenMV_Check_State(float dT_s);
#endif
