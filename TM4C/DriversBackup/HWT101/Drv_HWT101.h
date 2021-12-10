#ifndef __DRV_hwt101_H
#define __DRV_hwt101_H

//==����
#include "SysConfig.h"

//==����/����

typedef struct
{
	u16 angel;			 	//�Ƕ�
	u16  distance;    //������������
} _hwt101_st;

//�ɿ�״̬

//==��������
extern _hwt101_st hwt101;
//==��������
//static
static void HWT101_DataAnl(uint8_t *data_buf, uint8_t num);

//public
void HWT101_GetOneByte(uint8_t data);
void HWT101_Check_State(float dT_s);
#endif
