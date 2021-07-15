/******************** (C) COPYRIGHT 2017 ANO Tech ********************************
 * 作者    ：匿名科创
 * 官网    ：www.anotc.com
 * 淘宝    ：anotc.taobao.com
 * 技术Q群 ：190169595
 * 描述    ：飞控初始化
**********************************************************************************/
#include "Drv_BSP.h"
#include "Drv_PwmOut.h"
#include "Drv_led.h"
#include "Drv_adc.h"
#include "Drv_Timer.h"
#include "ANO_DT_LX.h"
#include "Drv_Uart.h"
#include "Drv_Timer.h"

u8 All_Init()
{
	DrvSysInit();
	//延时
	MyDelayMs(100);
	//LED功能初始化
	DvrLedInit();
	//初始化电调输出功能
	DrvPwmOutInit();
	MyDelayMs(100);
	DrvUart1Init(500000);
	//串口2初始化，函数参数为波特率
	DrvUart2Init(500000);
	//串口3初始化
	DrvUart3Init(115200);
	//接匿名光流
	DrvUart4Init(115200);
	//串口5接imu
	DrvUart5Init(500000);
	MyDelayMs(100);
	//SBUS输入采集初始化
	//DrvRcInputInit();
	//电池电压采集初始化
	DrvAdcInit();
	MyDelayMs(100);
	//数传模块初始化
	ANO_DT_Init();
	MyDelayMs(800);
	//初始化定时中断
	DrvTimerFcInit();
	//初始化完成，返回1
	return (1);
}
/******************* (C) COPYRIGHT 2014 ANO TECH *****END OF FILE************/
