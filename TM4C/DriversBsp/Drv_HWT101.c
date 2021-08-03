#include "Drv_HWT101.h"
#include "Ano_DT_LX.h"

_hwt101_st hwt101;
static uint8_t _datatemp[50];

//HWT101_GetOneByte是初级数据解析函数，串口每接收到一字节数据，调用本函数一次，函数参数就是串口收到的数据
//当本函数多次被调用，最终接收到完整的一帧数据后，会自动调用数据解析函数HWT101_DataAnl
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
//HWT101_DataAnl为HWT101数据解析函数，可以通过本函数得到光流模块输出的各项数据
//具体数据的意义，请参照匿名光流模块使用手册，有详细的介绍

static void HWT101_DataAnl(uint8_t *data, uint8_t len)
{
	u8 sum = 0;
	if (*(data) == 0X55 && *(data+1) == 0X53)
	{
		for(int i = 0;i < 10;i++)	sum += *(data+i); 
		if(sum == *(data+10))	hwt101.angel = (*(data+7)<<8)|*(data+6)/32768*180;
	}
}
