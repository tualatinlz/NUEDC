#include "Drv_Timer.h"
#include "timer.h"
#include "hw_ints.h"

volatile float duty;
volatile uint32_t Ticks[2];
volatile uint16_t i = 0;

void Timer0Irq(void)
{
	TimerIntClear(TIMER0_BASE,TIMER_TIMA_TIMEOUT); //��Aͨ���жϱ�־λ���
	ANO_LX_Task();
}

void Timer1Irq(void)
{
	TimerIntClear(TIMER1_BASE, TIMER_CAPA_EVENT);
    switch(i){
    case 0:
        TimerControlEvent(TIMER1_BASE, TIMER_A, TIMER_EVENT_NEG_EDGE);//��һ�β���������֮�󽫲����Ե���
        Ticks[i] = TimerValueGet(TIMER1_BASE,TIMER_A);
        i++;
    break;
    case 1:
        TimerControlEvent(TIMER1_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);//�ڶ��β����½���֮�󽫼��Ե���
        Ticks[i] = TimerValueGet(TIMER1_BASE,TIMER_A);
        i++;
    break;
    case 2:
			duty = Ticks[1] > Ticks[0] ? 1000*(Ticks[1] - Ticks[0])/(float)SysCtlClockGet():1000*(Ticks[1] - Ticks[0]+0xFFFFFF)/(float)SysCtlClockGet();//��������ʱ��
      i = 0;
    break;
    }
}

void DrvTimerFcInit(void)
{
	//ʹ��TIME0����
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	//��Timer0A����Ϊ32λ���ڼ�ʱ����
  ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
  //���ö�ʱ��װ��ֵ
  ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() / 1000);
	//���� TIMER0A �ж��¼�Ϊ��ʱ����ʱ
	ROM_IntPrioritySet( INT_TIMER0A , USER_INT7);
  ROM_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	TimerIntRegister(TIMER0_BASE,TIMER_A,Timer0Irq);
	//ʹ��Timer0A.
  ROM_TimerEnable(TIMER0_BASE, TIMER_A);
	
	
	/*
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinConfigure(GPIO_PB6_T0CCP0);
    GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_6);
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_6, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	*/
	//ʹ��TIME1����
  //ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
	//��Timer0A����Ϊ32λ���ڼ�ʱ����
  //ROM_TimerConfigure(TIMER1_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_TIME_UP);
  //���ö�ʱ��װ��ֵ
	//ROM_TimerControlEvent(TIMER1_BASE, TIMER_A, TIMER_EVENT_NEG_EDGE);
  //ROM_TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet() / 1000);
	//���� TIMER0A �ж��¼�Ϊ��ʱ����ʱ
	//ROM_IntPrioritySet( INT_TIMER1A , USER_INT7);
  //ROM_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	//TimerIntRegister(TIMER1_BASE,TIMER_A,Timer1Irq);
	//ʹ��Timer0A.
	//ROM_IntMasterEnable();
  //ROM_TimerIntEnable(TIMER1_BASE, TIMER_CAPA_EVENT);
  //ROM_IntEnable(INT_TIMER1A);
  //ROM_TimerEnable(TIMER1_BASE, TIMER_A);
}
