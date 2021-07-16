#include "Drv_EY4600.h"
#include "Ano_DT_LX.h"

_ey4600_st ey4600;
static uint8_t _datatemp[50];

//EY4600_GetOneByte�ǳ������ݽ�������������ÿ���յ�һ�ֽ����ݣ����ñ�����һ�Σ������������Ǵ����յ�������
//����������α����ã����ս��յ�������һ֡���ݺ󣬻��Զ��������ݽ�������EY4600_DataAnl
void EY4600_GetOneByte(uint8_t data)
{
	static u8 _data_len = 0, _data_cnt = 0;
	if(data != 0x0d && data != 0x0a)_datatemp[_data_cnt++] = data;
	if(ROM_UARTCharsAvail(UART4_BASE) == 0){
		EY4600_DataAnl(_datatemp,_data_cnt);
		_data_cnt = 0; 
		dt.fun[0xf2].WTS = 1;
	}
}
//EY4600_DataAnlΪEY4600���ݽ�������������ͨ���������õ�����ģ������ĸ�������
//�������ݵ����壬�������������ģ��ʹ���ֲᣬ����ϸ�Ľ���

static void EY4600_DataAnl(uint8_t *data, uint8_t len)
{
	for(int i = 0;i<len;i++){
		ey4600.rawdata[i] = _datatemp[i];//<<8 | _datatemp[i+1];
	}
}
