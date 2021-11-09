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
	u8  green;        //ʶ����ɫɫ��Ϊ1
	u8  next;
	u8  land;
} _k210_st;

typedef struct
{
	u8 	mode;        	//k210����ģʽѡ��
	u8  go;						//��һ����ɫ����
	u8  l1;
	u8  l2;
	u8 	a1;
	u8 	a2;
	u8 	b1;
	u8 	b2;
	u8  map;
} _k210_config;
//״̬

//==��������
extern _k210_st k210;
extern _k210_config k210_cfg;
//==��������
//static
static void K210_DataAnl(uint8_t *data_buf, uint8_t num);

//public
void K210_GetOneByte(uint8_t data);
void K210_Check_State(float dT_s);
#endif
