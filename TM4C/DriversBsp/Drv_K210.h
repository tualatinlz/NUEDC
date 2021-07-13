#ifndef __DRV_K210_H
#define __DRV_K210_H

//==����
#include "SysConfig.h"

//==����/����

typedef struct
{
	u8 link_sta; //����״̬��0��δ���ӡ�1�������ӡ�
	u8 work_sta; //����״̬��0���쳣��1������
	//
	u8 	number; 
	u16 angel;
  u8 update_cnt;  //���ݸ��¼�����
} _k210_st;

//�ɿ�״̬

//==��������
extern _k210_st k210;
//==��������
//static
static void K210_DataAnl(uint8_t *data_buf, uint8_t num);

//public
void K210_GetOneByte(uint8_t data);
void K210_Check_State(float dT_s);
#endif
