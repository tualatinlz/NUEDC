#include "ANO_LX.h"
#include "Drv_RcIn.h"
#include "ANO_DT_LX.h"
#include "ANO_Math.h"
#include "Drv_PwmOut.h"
#include "LX_FC_EXT_Sensor.h"
#include "Drv_AnoOf.h"
#include "Drv_adc.h"
#include "Drv_led.h"
#include "LX_FC_Fun.h"
#include "Drv_Uart.h"
#include "Drv_K210.h"
#include "Drv_HMI.h"

/*==========================================================================
 * 描述    ：凌霄飞控输入、输出主程序
 * 更新时间：2020-01-22 
 * 作者		 ：匿名科创-Jyoun
 * 官网    ：www.anotc.com
 * 淘宝    ：anotc.taobao.com
 * 技术Q群 ：190169595
 * 项目合作：18084888982，18061373080
============================================================================
 * 匿名科创团队感谢大家的支持，欢迎大家进群互相交流、讨论、学习。
 * 若您觉得匿名有不好的地方，欢迎您拍砖提意见。
 * 若您觉得匿名好，请多多帮我们推荐，支持我们。
 * 匿名开源程序代码欢迎您的引用、延伸和拓展，不过在希望您在使用时能注明出处。
 * 君子坦荡荡，小人常戚戚，匿名坚决不会请水军、请喷子，也从未有过抹黑同行的行为。  
 * 开源不易，生活更不容易，希望大家互相尊重、互帮互助，共同进步。
 * 只有您的支持，匿名才能做得更好。  
===========================================================================*/

_rt_tar_un rt_tar;
_pwm_st pwm_to_esc;
_fc_bat_un fc_bat;

//遥控CH5(AUX1)通道值(1000-1500-2000)设置模式1-2-3，模式0需要通过单独发送指令设置
//模式0：姿态自稳    ->遥控CH1-CH4直接控制姿态和油门。
//模式1：自稳+定高   ->遥控CH1/CH2/CH3控制姿态，但是遥控CH3(油门摇杆)控制垂直方向速度。
//模式2：定点        ->遥控CH1/CH2控制水平方向速度，并且遥控CH3(油门摇杆)控制垂直方向速度,遥控CH4控制YAW姿态。
//模式3：程控        ->遥控摇杆不参与控制

//////////////////////////////////////////////////////////////////////
//以下为飞控基础功能程序，不建议用户改动和调用。
//////////////////////////////////////////////////////////////////////

//输出给电调
static inline void ESC_Output(u8 unlocked)
{
	static u8 esc_calibrated;
	static s16 pwm[8];
	//
	pwm[0] = pwm_to_esc.pwm_m1 * 0.1f;
	pwm[1] = pwm_to_esc.pwm_m2 * 0.1f;
	pwm[2] = pwm_to_esc.pwm_m3 * 0.1f;
	pwm[3] = pwm_to_esc.pwm_m4 * 0.1f;
	pwm[4] = pwm_to_esc.pwm_m5 * 0.1f;
	pwm[5] = pwm_to_esc.pwm_m6 * 0.1f;
	pwm[6] = pwm_to_esc.pwm_m7 * 0.1f;
	pwm[7] = pwm_to_esc.pwm_m8 * 0.1f;
	//

	if (esc_calibrated == 0)
	{
//注意，若打开ESC校准功能，将可能发生不可预料的损坏或者人身伤害，后果自负。
//一定需要校准时，请拆掉螺旋桨，尽量避免发生意外。
//校准ESC成功后，记得关闭此功能，避免出现意外。
#if (ESC_CALI == 1)
		//
		for (u8 i = 0; i < 8; i++)
		{
			pwm[i] = 1000; //校准时先输出最大油门，注意有危险。
		}
		//
		//有遥控信号 且 油门下拉到低位时，输出0油门信号
		if (rc_in.no_signal == 0 && rc_in.rc_ch.st_data.ch_[ch_3_thr] < 1150)
		{
			//
			for (u8 i = 0; i < 8; i++)
			{
				pwm[i] = 0;
			}
			//标记校准完成。
			esc_calibrated = 1;
		}
#else
		//没有开校准功能，直接标记校准完成。
		esc_calibrated = 1;
#endif
	}
	else
	{
		//解锁才输出，否则输出0油门
		if (unlocked)
		{
			for (u8 i = 0; i < 8; i++)
			{
				pwm[i] = LIMIT(pwm[i], 0, 1000);
			}
		}
		else
		{
			for (u8 i = 0; i < 8; i++)
			{
				pwm[i] = 0;
			}
		}
	}
	//给底层PWM驱动输出信号
	DrvMotorPWMSet(pwm);
}

//根据ADC计算电池电压
static void Bat_Voltage_Data_Handle()
{
	fc_bat.st_data.voltage_100 = Drv_AdcGetBatVot() * 100; //单位：10mv
}

//定时1ms调用
void ANO_LX_Task()
{
	//static u8 mod_f[3];
	static u16 tmp_cnt[2];
	//计10ms
	tmp_cnt[0]++;
	tmp_cnt[0] %= 10;
	if (tmp_cnt[0] == 0)
	{
		//遥控输入
		//DrvRcInputTask(0.01f);
		//遥控数据处理
		//RC_Data_Task(0.01f);
		//飞控状态处理
		//LX_FC_State_Task(0.01f); 
		//匿名光流状态检测
		AnoOF_Check_State(0.01f);
		//==
		//计100ms
		tmp_cnt[1]++;
		tmp_cnt[1] %= 10;
		if (tmp_cnt[1] == 0)
		{
			//读取电池电压信息
			Bat_Voltage_Data_Handle();
		}
	}
	//解析串口接收到的数据
	DrvUartDataCheck();
	//GPS数据处理
	//GPS_Data_Prepare_Task(1);
	//外部传感器数据处理 
	LX_FC_EXT_Sensor_Task(0.001f);
	//通信交换
	ANO_LX_Data_Exchange_Task(0.001f);
	//电调输出
	ESC_Output(1); //unlocked
	//灯光驱动
	LED_1ms_DRV();
}
