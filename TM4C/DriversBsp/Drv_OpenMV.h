#ifndef __DRV_OpenMV_H
#define __DRV_OpenMV_H

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
	int  xtotal;
	u8  xdirection; 	//x���ƶ����� ��Ϊ0
	u8  ytotal;
	u8  yoffset;			 //����ƫ����
	u8  ydirection; 	//y���ƶ����� ��Ϊ0
  u8  update_cnt;  	//���ݸ��¼���
	u8  ready;				//������ָ��λ����
	u8  distance;			//����������
} _openmv_st;

typedef struct
{
	u8 	mode;        	//openmv����ģʽѡ��
	u8  go;
	u8  l1;
	u8  l2;
	u8 	a1;
	u8 	a2;
	u8 	b1;
	u8 	b2;
} _openmv_config;
//״̬

//==��������
extern _openmv_st openmv;
extern _openmv_config openmv_cfg;
//==��������
//static
static void OpenMV_DataAnl(uint8_t *data_buf, uint8_t num);

//public
void OpenMV_GetOneByte(uint8_t data);
void OpenMV_Check_State(float dT_s);
#endif
