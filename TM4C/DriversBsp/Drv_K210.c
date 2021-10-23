#include "Drv_K210.h"
#include "Ano_DT_LX.h"

_k210_st k210;
_k210_config k210_cfg;
static uint8_t _datatemp[50];

//K210_GetOneByte是初级数据解析函数，串口每接收到一字节光流数据，调用本函数一次，函数参数就是串口收到的数据
//当本函数多次被调用，最终接收到完整的一帧数据后，会自动调用数据解析函数K210_DataAnl
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
//K210_DataAnl为K210数据解析函数，可以通过本函数得到光流模块输出的各项数据
//具体数据的意义，请参照匿名光流模块使用手册，有详细的介绍

static void K210_DataAnl(uint8_t *data, uint8_t len)
{
	u8 check_sum1 = 0, check_sum2 = 0;
	if (*(data + 3) != (len - 6)) //判断数据长度是否正确
		return;
	for (u8 i = 0; i < len - 2; i++)
	{
		check_sum1 += *(data + i);
		check_sum2 += check_sum1;
	}
	if ((check_sum1 != *(data + len - 2)) || (check_sum2 != *(data + len - 1))) //判断sum校验
		return;
	//================================================================================

	if (*(data + 2) == 0XF1)
	{
		if (*(data + 4) == 0) //巡线转向及角度
		{
			k210.leftorright = *(data + 5);
			k210.angel = *(data + 6);
			k210.update_cnt++;
		}
		else if (*(data + 4) == 1) //x轴方向及偏移量
		{
			k210.xdirection = *(data + 5);
			k210.xoffset = *(data + 6)<<8 | *(data + 7);
			k210.update_cnt++;
		}
		else if (*(data + 4) == 2) //y轴方向及偏移量
		{
			k210.ydirection = *(data + 5);
			k210.yoffset = *(data + 6)<<8 | *(data + 7);
			k210.update_cnt++;
		}
		else if (*(data + 4) == 3) //超声波测距传输
		{
			k210.distance = *(data + 5)<<8 | *(data + 6);
			k210.distance = k210.distance/10;
			k210.update_cnt++;
		}
		else if (*(data + 4) == 4) //数字识别
		{
			k210.number = *(data + 5);
		}
		else if (*(data + 4) == 5) //当前工作模式
		{
			k210_cfg.mode = *(data + 6);
		}
	}
}
