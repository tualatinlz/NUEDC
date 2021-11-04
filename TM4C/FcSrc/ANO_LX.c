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
 * ����    �������ɿ����롢���������
 * ����ʱ�䣺2020-01-22 
 * ����		 �������ƴ�-Jyoun
 * ����    ��www.anotc.com
 * �Ա�    ��anotc.taobao.com
 * ����QȺ ��190169595
 * ��Ŀ������18084888982��18061373080
===========================================================================*/

_rt_tar_un rt_tar;
_pwm_st pwm_to_esc;
_fc_bat_un fc_bat;

//ģʽ3���̿�        ->ң��ҡ�˲��������

//////////////////////////////////////////////////////////////////////
//����Ϊ�ɿػ������ܳ��򣬲������û��Ķ��͵��á�
//////////////////////////////////////////////////////////////////////

//��������
static inline void ESC_Output(u8 unlocked)
{
	static u8 esc_calibrated;
	static s16 pwm[4];
	//
	pwm[0] = pwm_to_esc.pwm_m1 * 0.1f;
	pwm[1] = pwm_to_esc.pwm_m2 * 0.1f;
	pwm[2] = pwm_to_esc.pwm_m3 * 0.1f;
	pwm[3] = pwm_to_esc.pwm_m4 * 0.1f;
	//

	if (esc_calibrated == 0)
	{
//ע�⣬����ESCУ׼���ܣ������ܷ�������Ԥ�ϵ��𻵻��������˺�������Ը���
//һ����ҪУ׼ʱ���������������������ⷢ�����⡣
//У׼ESC�ɹ��󣬼ǵùرմ˹��ܣ�����������⡣
#if (ESC_CALI == 1)
		//
		for (u8 i = 0; i < 8; i++)
		{
			pwm[i] = 1000; //У׼ʱ�����������ţ�ע����Σ�ա�
		}
		//
		//��ң���ź� �� ������������λʱ�����0�����ź�
		if (rc_in.no_signal == 0 && rc_in.rc_ch.st_data.ch_[ch_3_thr] < 1150)
		{
			//
			for (u8 i = 0; i < 8; i++)
			{
				pwm[i] = 0;
			}
			//���У׼��ɡ�
			esc_calibrated = 1;
		}
#else
		//û�п�У׼���ܣ�ֱ�ӱ��У׼��ɡ�
		esc_calibrated = 1;
#endif
	}
	else
	{
		//������������������0����
		if (unlocked)
		{
			for (u8 i = 0; i < 4; i++)
			{
				pwm[i] = LIMIT(pwm[i], 0, 1000);
			}
		}
		else
		{
			for (u8 i = 0; i < 4; i++)
			{
				pwm[i] = 0;
			}
		}
	}
	//���ײ�PWM��������ź�
	DrvMotorPWMSet(pwm);
}

//����ADC�����ص�ѹ
static void Bat_Voltage_Data_Handle()
{
	fc_bat.st_data.voltage_100 = Drv_AdcGetBatVot() * 100; //��λ��10mv
}

//��ʱ1ms����
void ANO_LX_Task()
{
	//static u8 mod_f[3];
	static u16 tmp_cnt[2];
	//��10ms
	tmp_cnt[0]++;
	tmp_cnt[0] %= 10;
	if (tmp_cnt[0] == 0)
	{
		//ң������
		//DrvRcInputTask(0.01f);
		//ң�����ݴ���
		//RC_Data_Task(0.01f);
		//�ɿ�״̬����
		//LX_FC_State_Task(0.01f); 
		//��������״̬���
		AnoOF_Check_State(0.01f);
		//==
		//��100ms
		tmp_cnt[1]++;
		tmp_cnt[1] %= 10;
		if (tmp_cnt[1] == 0)
		{
			//��ȡ��ص�ѹ��Ϣ
			Bat_Voltage_Data_Handle();
		}
	}
	//�������ڽ��յ�������
	DrvUartDataCheck();
	//GPS���ݴ���
	//GPS_Data_Prepare_Task(1);
	//�ⲿ���������ݴ��� 
	LX_FC_EXT_Sensor_Task(0.001f);
	//ͨ�Ž���
	ANO_LX_Data_Exchange_Task(0.001f);
	//������
	ESC_Output(1); //unlocked
	//�ƹ�����
	LED_1ms_DRV();
}
