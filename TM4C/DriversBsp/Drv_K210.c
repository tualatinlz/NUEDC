#include "Drv_K210.h"

_k210_st k210;
static uint8_t _datatemp[50];
static float check_time_ms[2];
void K210_Check_State(float dT_s)
{
	u8 tmp[2];
	//���Ӽ��
	if (check_time_ms[0] < 500)
	{
		check_time_ms[0]++;
		k210.link_sta = 1;
	}
	else
	{
		k210.link_sta = 0;
	}
	//���ݼ��1
	if (check_time_ms[1] < 500)
	{
		check_time_ms[1]++;
		tmp[0] = 1;
	}
	else
	{
		tmp[0] = 1;
	}
	//���ù���״̬
	if (tmp[0])
	{
		k210.work_sta = 1;
	}
	else
	{
		k210.work_sta = 1;
	}
}

//K210_GetOneByte�ǳ������ݽ�������������ÿ���յ�һ�ֽڹ������ݣ����ñ�����һ�Σ������������Ǵ����յ�������
//����������α����ã����ս��յ�������һ֡���ݺ󣬻��Զ��������ݽ�������K210_DataAnl
void K210_GetOneByte(uint8_t data)
{
	static u8 _data_len = 0, _data_cnt = 0;
	static u8 rxstate = 0;

	if (rxstate == 0 && data == 0xAA)
	{
		rxstate = 1;
		_datatemp[0] = data;
	}
	else if (rxstate == 1 && (data == HW_TYPE || data == HW_ALL))
	{
		rxstate = 2;
		_datatemp[1] = data;
	}
	else if (rxstate == 2)
	{
		rxstate = 3;
		_datatemp[2] = data;
	}
	else if (rxstate == 3 && data < 250)
	{
		rxstate = 4;
		_datatemp[3] = data;
		_data_len = data;
		_data_cnt = 0;
	}
	else if (rxstate == 4 && _data_len > 0)
	{
		_data_len--;
		_datatemp[4 + _data_cnt++] = data;
		if (_data_len == 0)
			rxstate = 5;
	}
	else if (rxstate == 5)
	{
		rxstate = 6;
		_datatemp[4 + _data_cnt++] = data;
	}
	else if (rxstate == 6)
	{
		rxstate = 0;
		_datatemp[4 + _data_cnt] = data;
		//		DT_data_cnt = _data_cnt+5;
		//
		K210_DataAnl(_datatemp, _data_cnt + 5); //
	}
	else
	{
		rxstate = 0;
	}
}
//K210_DataAnlΪK210���ݽ�������������ͨ���������õ�����ģ������ĸ�������
//�������ݵ����壬�������������ģ��ʹ���ֲᣬ����ϸ�Ľ���

static void K210_DataAnl(uint8_t *data, uint8_t len)
{
	u8 check_sum1 = 0, check_sum2 = 0;
	if (*(data + 3) != (len - 6)) //�ж����ݳ����Ƿ���ȷ
		return;
	for (u8 i = 0; i < len - 2; i++)
	{
		check_sum1 += *(data + i);
		check_sum2 += check_sum1;
	}
	if ((check_sum1 != *(data + len - 2)) || (check_sum2 != *(data + len - 1))) //�ж�sumУ��
		return;
	//================================================================================

	if (*(data + 2) == 0XF1)
	{
		if (*(data + 4) == 0) //Ѳ�߽Ƕ�
		{
			k210.angel = *(data + 5)<<8 | *(data + 6);
			k210.update_cnt++;
		}
		else if (*(data + 4) == 1) //ʶ������
		{
			k210.number = *(data + 5);
			k210.update_cnt++;
		}
	}
	if (*(data + 2) == 0XF2)//
	{
			
	}
	else if (*(data + 2) == 0X34) //
	{

	}
}