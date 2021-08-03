#ifndef __DRV_hmi_H
#define __DRV_hmi_H

//==����
#include "SysConfig.h"

//==����/����



typedef struct
{
	u8 mode;   //���г���ѡ��
	u8 oldmode;
  u8 update_cnt;  //���ݸ��¼�����
	u8 page;
} _hmi_st;

//�ɿ�״̬

//==��������
extern _hmi_st hmi;
//==��������
//static
static void HMI_DataAnl(uint8_t *data_buf, uint8_t num);
//public
void HMI_GetOneByte(uint8_t data);
#endif
