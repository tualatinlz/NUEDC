/******************** (C) COPYRIGHT 2017 ANO Tech ********************************
 * ����    �������ƴ�
 * ����    ��www.anotc.com
 * �Ա�    ��anotc.taobao.com
 * ����QȺ ��190169595
 * ����    ���ɿس�ʼ��
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
	//��ʱ
	MyDelayMs(100);
	//LED���ܳ�ʼ��
	DvrLedInit();
	//��ʼ������������
	DrvPwmOutInit();
	MyDelayMs(100);
	//�����������ڽ�HMI
	DrvUart1Init(115200);
	//����2��ʼ�� EY4600
	DrvUart2Init(115200);
	//����3��ʼ�� K210
	DrvUart3Init(115200);
	//����������
	DrvUart4Init(115200);
	//����5��imu ���ڽ�����
	DrvUart5Init(500000);
	MyDelayMs(100);
	//SBUS����ɼ���ʼ��
	//DrvRcInputInit();
	//��ص�ѹ�ɼ���ʼ��
	DrvAdcInit();
	MyDelayMs(100);
	//����ģ���ʼ��
	ANO_DT_Init();
	MyDelayMs(800);
	//��ʼ����ʱ�ж�
	DrvTimerFcInit();
	//��ʼ����ɣ�����1
	return (1);
}
/******************* (C) COPYRIGHT 2014 ANO TECH *****END OF FILE************/
