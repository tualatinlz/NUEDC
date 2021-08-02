#ifndef __DRV_K210_H
#define __DRV_K210_H

//==����
#include "SysConfig.h"

//==����/����

typedef struct
{
	//
	u8 	number; 			//����ʶ����
	u16 angel;			 	//Ѳ�߽Ƕ�
	u8  leftorright; 	//��ת����
	u8  xoffset;			 //����ƫ����
	u8  xdirection; 	//x���ƶ����� ��Ϊ0
	u8  yoffset;			 //����ƫ����
	u8  ydirection; 	//y���ƶ����� ��Ϊ0
  u8  update_cnt;  	//���ݸ��¼���
	u16  distance;    //������������
	u8 	mode;        	//k210����ģʽѡ��
	//0��  1��  2��
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
