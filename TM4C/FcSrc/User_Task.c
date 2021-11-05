#include "User_Task.h"
#include "LX_FC_Fun.h"
#include "Drv_OpenMV.h"
#include "Drv_K210.h"
#include "Drv_HMI.h"
#include "Drv_AnoOf.h"
#include "ANO_DT_LX.h"
#include "Drv_PwmOut.h"
#include "Drv_Led.h"
static u16 delaycnt;
static u8 delay_flag;
extern int targetHeight;

//一级延迟函数 一次延迟20ms
void delay20(){
	static u16 count = 0;
	if(count <= delaycnt) count++;
	else{
		delaycnt = 0;
		delay_flag = 0;
		count = 0;
		fc_sta.rotating = 0;
	}
}
//主函数 包括不同模式
void UserTask_OneKeyCmd(void)
{
		//static u8 counter1,counter2 = 0;
		//u16 wholeLength = 30000;
		LX_Change_Mode(3);
		switch(hmi.mode){
			case 0x01:FC_Lock();
			hmi.mode = 0;
				break;
			case 0x02:OneKey_Land();
			hmi.mode = 0;
				break;
			case 0x03:FC_Unlock();
			hmi.mode = 0;
				break;
			case 0x04:spreadP(150);
				break;
			case 0x05:customF();
				break;
			case 0x10:Horizontal_Calibrate();
								hmi.mode = 0;
				break;
			case 0x11:Mag_Calibrate();
								hmi.mode = 0;
				break;
			case 0x12:ACC_Calibrate();
								hmi.mode = 0;
				break;
			case 0x13:GYR_Calibrate();
								hmi.mode = 0;
				break;
		}
}
void spreadP(u8 height){
		static u16 counter = 0;		//飞行时间计数
		counter ++;
		u16 maxcnt = 18000;				//最大飞行时间
		static u8 stage = 0;			//流程执行阶段
		static u8 sendflag=0;			//发送标志位
		static u8 step = 1;				//格子走到第几步了
		static u8 counterLED = 0;	//LED闪烁次数
		u8 blockLength = 50;			//格子长度
		u8 velocity = 50;					//前进速度
		u8 vd = 45;	 							//转向角速度
		u8 degcnt = 110;					//转向等待时间
		LX_Change_Mode(3);				//切换到程控模式
		
		if(hmi.mode != hmi.oldmode){
			counter = 0;
			stage = 0;
			sendflag = 0;
			step = 0;
			counterLED = 0;
			hmi.oldmode = hmi.mode;
		}
		switch(step){
			case 4:Right_Rotate(90,vd);
					step++;
					delaycnt=degcnt;
					delay_flag=1;
				break;
			case 8:Left_Rotate(90,vd);
			step++;
					delaycnt=degcnt;
					delay_flag=1;
				break;
			case 11:Left_Rotate(90,vd);
			step++;
					delaycnt=degcnt;
					delay_flag=1;
				break;
			case 12:Left_Rotate(90,vd);
			step++;
					delaycnt=degcnt;
					delay_flag=1;
				break;
			case 14:Right_Rotate(90,vd);
			step++;
					delaycnt=degcnt;
					delay_flag=1;
				break;
			case 15:Right_Rotate(90,vd);
			step++;
					delaycnt=degcnt;
					delay_flag=1;
				break;
			case 17:Left_Rotate(90,vd);
			step++;
					delaycnt=degcnt;
					delay_flag=1;
				break;
			case 18:Left_Rotate(90,vd);
			step++;
					delaycnt=degcnt;
					delay_flag=1;
				break;
			case 20:Right_Rotate(90,vd);
			step++;
					delaycnt=degcnt;
					delay_flag=1;
				break;
			case 21:Right_Rotate(90,vd);
			step++;
					delaycnt=degcnt;
					delay_flag=1;
				break;
			case 23:Left_Rotate(90,vd);
			step++;
					delaycnt=degcnt;
					delay_flag=1;
				break;
			case 24:Left_Rotate(90,vd);
			step++;
					delaycnt=degcnt;
					delay_flag=1;
				break;
			case 30:Left_Rotate(90,vd);
			step++;
					delaycnt=degcnt;
					delay_flag=1;
					stage = 10;
				break;
				default:
					break;
		}
		
		if(delay_flag){
			delay20();
		}
		else{
			switch(stage){
				case 0:
					FC_Unlock();
					k210.number = 4;
					stage = 1;
					delaycnt = 200;
					delay_flag = 1;
				break;
				case 1:
					targetHeight = height;
					OneKey_Takeoff(height);
					stage = 2;
					delaycnt = 250;
					delay_flag = 1;
				break;
				case 2:
					Horizontal_Move(4*blockLength,velocity,0);
					stage = 3;
					delaycnt = 250;
					delay_flag = 1;
				break;
				case 3:
					Right_Rotate(90,90);
					step++;
					delaycnt = 100;
					delay_flag = 1;
					stage=4;
				break;	
				//经验调整
				case 4:
					Horizontal_Move(blockLength,velocity,0); 
					delaycnt = 70;
					delay_flag = 1;
					stage=5;
				break;
				//K210开启字母识别，根据A位置调整
				case 5:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 60;
					delay_flag = 1;
					stage=6;
				break;
				case 6:
					Horizontal_Move(k210.xoffset,10,k210.xdirection*180+90);
					delaycnt = 70;
					delay_flag = 1;
					stage=7;
				break;
				case 7:
					Horizontal_Move(k210.yoffset,10,k210.ydirection*180);
					delaycnt = 70;
					delay_flag = 1;
					stage=32;
				break;
				case 32:
					Horizontal_Move(blockLength,velocity,0);
					delaycnt = 70;
					delay_flag = 1;
					stage=8;
				break;
				//请求K210判断是否绿色，接到判断完毕指令后前进一格
				case 8:
					if(sendflag==0){
						k210_cfg.mode=1;
						k210_cfg.go=1;
						dt.fun[0xf4].WTS=1;                   
						sendflag=1;
					}
					if(k210.next==1){
						stage = 9;
						k210_cfg.go=0;
						k210.next=0;
						sendflag=0;
					}
					delaycnt = 25;
					delay_flag = 1;
				break;
				case 9:
					Horizontal_Move(blockLength,velocity,0);
					step++;
					delaycnt = 80;
					delay_flag = 1;
					stage=8;
				break;
				//惯导返航
				case 10:
					Horizontal_Move(5*blockLength,velocity,0);
					delaycnt = 300;
					delay_flag = 1;
					stage=11;
				break;
				case 11:
					Horizontal_Move(blockLength,velocity,90);
					k210_cfg.mode=3;	
					dt.fun[0xf4].WTS=1;
					delaycnt = 50;
					delay_flag = 1;
					stage=12;
				break;
				//根据十字位置判断飞机位置，K210控制飞机调整降落点
				case 12:
					Horizontal_Move(k210.xoffset,50,k210.xdirection*180+90);
					delaycnt = 50;
					delay_flag = 1;
					stage=13;
					if(k210.land == 1)stage=60;
				break;
				case 13:
					Horizontal_Move(k210.yoffset,50,k210.ydirection*180);
					delaycnt = 50;
					delay_flag = 1;
					stage=12;
					if(k210.land == 1)stage=60;
				break;
				case 60:
					OneKey_Land();
				break;
				}
		//超时降落			
		if(counter >= maxcnt){
			OneKey_Land();
			counter = 0;
			stage = 0;
			hmi.mode = 0;
		}
	}
}


void spreadPU(u8 height){
		static u16 counter = 0;		//飞行时间计数
		counter ++;
		u16 maxcnt = 18000;				//最大飞行时间
		static u8 stage = 0;			//流程执行阶段
		static u8 sendflag=0;			//发送标志位
		static u8 step = 1;				//格子走到第几步了
		static u8 counterLED = 0;	//LED闪烁次数
		u8 blockLength = 50;			//格子长度
		u8 velocity = 50;					//前进速度
		u8 vd = 90;	 							//转向角速度
		LX_Change_Mode(3);				//切换到程控模式
		
		if(hmi.mode != hmi.oldmode){
			counter = 0;
			stage = 0;
			sendflag = 0;
			step = 0;
			counterLED = 0;
			hmi.oldmode = hmi.mode;
		}
		switch(step){
			case 4:Right_Rotate(90,vd);
					delaycnt=50;
					delay_flag=1;
				break;
			case 8:Left_Rotate(90,vd);
					delaycnt=50;
					delay_flag=1;
				break;
			case 11:Left_Rotate(90,vd);
					delaycnt=50;
					delay_flag=1;
				break;
			case 12:Left_Rotate(90,vd);
					delaycnt=50;
					delay_flag=1;
				break;
			case 14:Right_Rotate(90,vd);
					delaycnt=50;
					delay_flag=1;
				break;
			case 15:Right_Rotate(90,vd);
					delaycnt=50;
					delay_flag=1;
				break;
			case 17:Left_Rotate(90,vd);
					delaycnt=50;
					delay_flag=1;
				break;
			case 18:Left_Rotate(90,vd);
					delaycnt=50;
					delay_flag=1;
				break;
			case 20:Right_Rotate(90,vd);
					delaycnt=50;
					delay_flag=1;
				break;
			case 21:Right_Rotate(90,vd);
					delaycnt=50;
					delay_flag=1;
				break;
			case 23:Left_Rotate(90,vd);
					delaycnt=50;
					delay_flag=1;
				break;
			case 24:Left_Rotate(90,vd);
					delaycnt=50;
					delay_flag=1;
				break;
			case 30:Left_Rotate(90,vd);
					delaycnt=50;
					delay_flag=1;
					stage = 6;
				break;
				default:
					break;
		}
		
		if(delay_flag){
			delay20();
		}
		else{
			switch(stage){
				case 0:
					FC_Unlock();
					k210.number = 4;
					stage = 1;
					delaycnt = 200;
					delay_flag = 1;
				break;
				case 1:
					targetHeight = height;
					OneKey_Takeoff(height);
					stage = 2;
					delaycnt = 250;
					delay_flag = 1;
				break;
				case 2:
					Horizontal_Move(4*blockLength,velocity,0);
					stage = 3;
					delaycnt = 250;
					delay_flag = 1;
				break;
				case 3:
					Right_Rotate(90,90);
					step++;
					delaycnt = 70;
					delay_flag = 1;
					stage=4;
				break;	
				//经验调整
				case 4:
					Horizontal_Move(blockLength,velocity,0); 
					delaycnt = 50;
					delay_flag = 1;
					stage=5;
				break;
				//K210开启字母识别，根据A位置调整
				case 5:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 50;
					delay_flag = 1;
					stage=6;
				break;
				case 6:
					Horizontal_Move(k210.xoffset,10,k210.xdirection*180+90);
					delaycnt = 70;
					delay_flag = 1;
					stage=7;
				break;
				case 7:
					Horizontal_Move(k210.yoffset,10,k210.ydirection*180);
					delaycnt = 70;
					delay_flag = 1;
					stage=8;
				break;
				//请求K210判断是否绿色，接到判断完毕指令后前进一格
				case 8:
					if(sendflag==0){
						k210_cfg.mode=1;
						k210_cfg.go=1;
						dt.fun[0xf4].WTS=1;                   
						sendflag=1;
					}
					if(k210.next==1){
						stage = 5;
						k210_cfg.go=0;
						k210.next=0;
						sendflag=0;
					}
					stage = 9;
					delaycnt = 25;
					delay_flag = 1;
				break;
				case 9:
					Horizontal_Move(blockLength,velocity,0);
					step++;
					delaycnt = 50;
					delay_flag = 1;
					stage=8;
				break;
				//惯导返航
				case 10:
					Horizontal_Move(5*blockLength,velocity,0);
					delaycnt = 50;
					delay_flag = 1;
					stage=11;
				break;
				case 11:
					Horizontal_Move(blockLength,velocity,90);
					k210_cfg.mode=3;	
					dt.fun[0xf4].WTS=1;
					delaycnt = 50;
					delay_flag = 1;
					stage=12;
				break;
				//根据十字位置判断飞机位置，K210控制飞机调整降落点
				case 12:
					Horizontal_Move(k210.xoffset,50,k210.xdirection*180+90);
					delaycnt = 50;
					delay_flag = 1;
					stage=13;
					if(k210.land == 1)stage=60;
				break;
				case 13:
					Horizontal_Move(k210.yoffset,50,k210.ydirection*180);
					delaycnt = 50;
					delay_flag = 1;
					stage=12;
					if(k210.land == 1)stage=60;
				break;
				//寻找杆子和二维码位置
				case 20:
					k210_cfg.mode=4;
					dt.fun[0xf4].WTS=1;
					stage=21;
					delaycnt = 50;
					delay_flag = 1;
				break;
				case 21:
					openmv_cfg.mode = 1;
					dt.fun[0xf5].WTS=1;
					stage=21;
					delaycnt = 50;
					delay_flag = 1;
				break;
				case 41:
					Vertical_Down(20,20);
					stage= 12;
					delaycnt = 50;
					delay_flag = 1;
				break;
				//LED依据识别到的数字闪烁
				case 31:
					blink();
					delaycnt = 25;
					delay_flag = 1;
					counterLED++;
					if(counterLED >= k210.number * 2){
						//下一阶段
					}
				break;
				case 60:
					OneKey_Land();
				break;
				//2*数字
				break;
				}
		//超时降落			
		if(counter >= maxcnt){
			OneKey_Land();
			counter = 0;
			stage = 0;
			hmi.mode = 0;
		}
	}
}


void customF(){
	LX_Change_Mode(3);
	k210_cfg.mode=4;
  dt.fun[0xf4].WTS=1;
	delaycnt = 50;
	delay_flag = 1;
	/*if(delay_flag){
			delay20();
	}
	else{
		blink();
		delaycnt = 25;
		delay_flag = 1;
		counterLED++;
		//2*数字
		if(counterLED >= 8) hmi.mode = 0;
	}*/
	hmi.mode = 0;
}
//控制LED闪烁，调用一次切换一次状态
void blink(void){
	u8 color = 2;
	static u8 clicked = 0;
	if(clicked){
		clicked=0;
		DrvLedOnOff(0);
	}
	else{
		clicked=1;
		DrvLedOnOff(color);
	}
}

//控制舵机拍照 拍一张需要调用两次
void Take_Photos(void){
	static u8 clicked = 0;
	static s16 pwm[4];
	if(clicked){
		pwm[0]  =125;
		clicked = 0;
	}
	else{
		pwm[0] = 135;
		clicked = 1;
	}
	DrvServoPWMSet(pwm);
}
