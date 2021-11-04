#ifndef __DRV_EY4600_H
#define __DRV_EY4600_H

//==����
#include "SysConfig.h"

//==����/����

typedef struct
{
	//
	uint16_t rawdata[25];
  u8 update_cnt;  //���ݸ��¼�����
} _ey4600_st;
//�ɿ�״̬

//==��������
extern _ey4600_st ey4600;
//==��������
//static
static void EY4600_DataAnl(uint8_t *data_buf, uint8_t num);
//public
void EY4600_GetOneByte(uint8_t data);
#endif
