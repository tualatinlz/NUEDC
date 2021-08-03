#include "Drv_HWT101.h"
#include "Ano_DT_LX.h"

_hwt101_st hwt101;
static uint8_t _datatemp[50];

//HWT101_GetOneByte�ǳ������ݽ�������������ÿ���յ�һ�ֽ����ݣ����ñ�����һ�Σ������������Ǵ����յ�������
//����������α����ã����ս��յ�������һ֡���ݺ󣬻��Զ��������ݽ�������HWT101_DataAnl
void HWT101_GetOneByte(uint8_t data)
{
	static u8 _data_len = 0, _data_cnt = 0;
	static u8 rxstate = 0;
	_datatemp[_data_cnt++] = data;
	if(ROM_UARTCharsAvail(UART4_BASE) == 0){
		HWT101_DataAnl(_datatemp,_data_cnt);
		_data_cnt = 0; 
	}
}
//HWT101_DataAnlΪHWT101���ݽ�������������ͨ���������õ�����ģ������ĸ�������
//�������ݵ����壬�������������ģ��ʹ���ֲᣬ����ϸ�Ľ���

static void HWT101_DataAnl(uint8_t *data, uint8_t len)
{
	u8 sum = 0;
	if (*(data) == 0X55 && *(data+1) == 0X53)
	{
		for(int i = 0;i < 10;i++)	sum += *(data+i); 
		if(sum == *(data+10))	hwt101.angel = (*(data+7)<<8)|*(data+6)/32768*180;
	}
}
