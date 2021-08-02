#include "Drv_Timer.h"
#include "timer.h"
#include "hw_ints.h"

volatile float duty;
volatile uint32_t Ticks[2];
volatile uint16_t i = 0;

void Timer0Irq(void)
{
	TimerIntClear(TIMER0_BASE,TIMER_TIMA_TIMEOUT); //将A通道中断标志位清除
	ANO_LX_Task();
}

void Timer1Irq(void)
{
	TimerIntClear(TIMER1_BASE, TIMER_CAPA_EVENT);
    switch(i){
    case 0:
        TimerControlEvent(TIMER1_BASE, TIMER_A, TIMER_EVENT_NEG_EDGE);//第一次捕获到上升沿之后将捕获极性倒置
        Ticks[i] = TimerValueGet(TIMER1_BASE,TIMER_A);
        i++;
    break;
    case 1:
        TimerControlEvent(TIMER1_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);//第二次捕获下降沿之后将极性倒置
        Ticks[i] = TimerValueGet(TIMER1_BASE,TIMER_A);
        i++;
    break;
    case 2:
			duty = Ticks[1] > Ticks[0] ? 1000*(Ticks[1] - Ticks[0])/(float)SysCtlClockGet():1000*(Ticks[1] - Ticks[0]+0xFFFFFF)/(float)SysCtlClockGet();//计算脉冲时间
      i = 0;
    break;
    }
}

void DrvTimerFcInit(void)
{
	//使能TIME0外设
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	//将Timer0A配置为32位定期计时器。
  ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
  //配置定时器装载值
  ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() / 1000);
	//配置 TIMER0A 中断事件为定时器超时
	ROM_IntPrioritySet( INT_TIMER0A , USER_INT7);
  ROM_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	TimerIntRegister(TIMER0_BASE,TIMER_A,Timer0Irq);
	//使能Timer0A.
  ROM_TimerEnable(TIMER0_BASE, TIMER_A);
	
	
	/*
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinConfigure(GPIO_PB6_T0CCP0);
    GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_6);
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_6, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	*/
	//使能TIME1外设
  //ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
	//将Timer0A配置为32位定期计时器。
  //ROM_TimerConfigure(TIMER1_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_TIME_UP);
  //配置定时器装载值
	//ROM_TimerControlEvent(TIMER1_BASE, TIMER_A, TIMER_EVENT_NEG_EDGE);
  //ROM_TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet() / 1000);
	//配置 TIMER0A 中断事件为定时器超时
	//ROM_IntPrioritySet( INT_TIMER1A , USER_INT7);
  //ROM_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	//TimerIntRegister(TIMER1_BASE,TIMER_A,Timer1Irq);
	//使能Timer0A.
	//ROM_IntMasterEnable();
  //ROM_TimerIntEnable(TIMER1_BASE, TIMER_CAPA_EVENT);
  //ROM_IntEnable(INT_TIMER1A);
  //ROM_TimerEnable(TIMER1_BASE, TIMER_A);
}
