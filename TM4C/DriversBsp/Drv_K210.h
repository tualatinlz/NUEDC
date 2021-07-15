#ifndef __DRV_K210_H
#define __DRV_K210_H

//==����
#include "SysConfig.h"

//==����/����

typedef struct
{
	//
	u8 	number; 
	u16 angel;
  u8 update_cnt;  //���ݸ��¼�����
} _k210_st;

typedef union {
	u8 byte[3];
	_k210_st st_data;
} _k210_un;

//�ɿ�״̬

//==��������
extern _k210_st k210;
extern _k210_un ext_k210;
//==��������
//static
static void K210_DataAnl(uint8_t *data_buf, uint8_t num);

//public
void K210_GetOneByte(uint8_t data);
void K210_Check_State(float dT_s);
#endif
