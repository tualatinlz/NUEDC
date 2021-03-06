#include "Drv_EY4600.h"
#include "Ano_DT_LX.h"

_ey4600_st ey4600;
static uint8_t _datatemp[50];

//EY4600_GetOneByte是初级数据解析函数，串口每接收到一字节数据，调用本函数一次，函数参数就是串口收到的数据
//当本函数多次被调用，最终接收到完整的一帧数据后，会自动调用数据解析函数EY4600_DataAnl
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
//EY4600_DataAnl为EY4600数据解析函数，可以通过本函数得到光流模块输出的各项数据
//具体数据的意义，请参照匿名光流模块使用手册，有详细的介绍

static void EY4600_DataAnl(uint8_t *data, uint8_t len)
{
	for(int i = 0;i<len;i++){
		ey4600.rawdata[i] = _datatemp[i];//<<8 | _datatemp[i+1];
	}
}
