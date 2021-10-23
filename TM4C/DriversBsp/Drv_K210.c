#include "Drv_K210.h"
#include "Ano_DT_LX.h"

_k210_st k210;
_k210_config k210_cfg;
static uint8_t _datatemp[50];

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
		K210_DataAnl(_datatemp, _data_cnt + 5); //
		dt.fun[0xf1].WTS = 1;
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
		if (*(data + 4) == 0) //Ѳ��ת�򼰽Ƕ�
		{
			k210.leftorright = *(data + 5);
			k210.angel = *(data + 6);
			k210.update_cnt++;
		}
		else if (*(data + 4) == 1) //x�᷽��ƫ����
		{
			k210.xdirection = *(data + 5);
			k210.xoffset = *(data + 6)<<8 | *(data + 7);
			k210.update_cnt++;
		}
		else if (*(data + 4) == 2) //y�᷽��ƫ����
		{
			k210.ydirection = *(data + 5);
			k210.yoffset = *(data + 6)<<8 | *(data + 7);
			k210.update_cnt++;
		}
		else if (*(data + 4) == 3) //��������ഫ��
		{
			k210.distance = *(data + 5)<<8 | *(data + 6);
			k210.distance = k210.distance/10;
			k210.update_cnt++;
		}
		else if (*(data + 4) == 4) //����ʶ��
		{
			k210.number = *(data + 5);
		}
		else if (*(data + 4) == 5) //��ǰ����ģʽ
		{
			k210_cfg.mode = *(data + 6);
		}
	}
}
