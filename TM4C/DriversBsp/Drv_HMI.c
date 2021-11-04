#include "Drv_hmi.h"
#include "LX_FC_EXT_Sensor.h"
#include "ANO_LX.h"
#include "ANO_DT_LX.h"

_hmi_st hmi;
static uint8_t _datatemp[50];

//hmi_GetOneByte是初级数据解析函数，串口每接收到一字节数据，调用本函数一次，函数参数就是串口收到的数据
//当本函数多次被调用，最终接收到完整的一帧数据后，会自动调用数据解析函数hmi_DataAnl
void HMI_GetOneByte(uint8_t data)
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
		HMI_DataAnl(_datatemp, _data_cnt + 5);
	}
	else
	{
		rxstate = 0;
	}
}
//hmi_DataAnl为hmi数据解析函数，可以通过本函数得到MCU输出的各项数据

static void HMI_DataAnl(uint8_t *data, uint8_t len)
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

	if (*(data + 2) == 0XF3) 
	{
		if(*(data + 4) == 0){ //串口屏指令
			if(*(data + 5) != hmi.mode)	hmi.oldmode = hmi.mode;
			hmi.mode = *(data + 5);
		}
		else if(*(data + 4) == 1){ //阈值调整
			k210_cfg.l1 = *(data + 5);
			k210_cfg.l2 = *(data + 6);
			k210_cfg.a1 = *(data + 7);
			k210_cfg.a2 = *(data + 8);
			k210_cfg.b1 = *(data + 9);
			k210_cfg.b2 = *(data + 10);
			dt.fun[0xf5].WTS = 1; //标记CMD等待发送
		}
		else if(*(data + 4) == 2){ //页面号
			hmi.page = *(data + 5);
		}
	}
}


