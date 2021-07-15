#ifndef __DRV_hmi_H
#define __DRV_hmi_H

//==����
#include "SysConfig.h"

//==����/����

typedef struct
{
	u8 link_sta; //����״̬��0��δ���ӡ�1�������ӡ�
	u8 work_sta; //����״̬��0���쳣��1������
	//
	u8 mode;   //���г���ѡ��
	u8 oldmode;
	u16 angel;
  u8 update_cnt;  //���ݸ��¼�����
} _hmi_st;

//�ɿ�״̬

//==��������
extern _hmi_st hmi;
//==��������
//static
static void HMI_DataAnl(uint8_t *data_buf, uint8_t num);

//public
void HMI_GetOneByte(uint8_t data);
void HMI_Check_State(float dT_s);
#endif
