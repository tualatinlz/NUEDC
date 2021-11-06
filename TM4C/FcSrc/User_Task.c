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
			case 0x03:customF();
			//hmi.mode = 0;
				break;
			case 0x04:spreadP(150);
				break;
			case 0x05:spreadPU(150);
				break;
			case 0x06:customF();
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
		u8 velocity = 50;						//前进速度
		static u16 direction = 90;	//前进方向 
		//u8 vd = 45;	 							//转向角速度
		//u8 degcnt = 110;					//转向等待时间
		LX_Change_Mode(3);					//切换到程控模式
		
		if(hmi.mode != hmi.oldmode){
			counter = 0;
			stage = 0;
			sendflag = 0;
			step = 1;
			counterLED = 0;
			hmi.oldmode = hmi.mode;
		}
		switch(step){
			case 4:direction = 180;
				break;
			case 8:direction = 90;
				break;
			case 11:direction = 0;
				break;
			case 12:direction = 270;
				break;
			case 14:direction = 0;
				break;
			case 15:direction = 90;
				break;
			case 17:direction = 0;
				break;
			case 18:direction = 270;
				break;
			case 20:direction = 0;
				break;
			case 21:direction = 90;
				break;
			case 23:direction = 0;
				break;
			case 24:direction = 270;
				break;
			case 30:Right_Rotate(180,90);
							step++;
							delaycnt=300;
							delay_flag=1;
							stage = 10;
				break;
				default:
					break;
		}
		
		//根据平移方向确定距离
		switch(direction){
			case 90:
			case 270:
				blockLength = 50;
			break;
			case 0:
			case 180:
				blockLength = 46;
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
					Horizontal_Move(4*46,velocity,0);
					stage = 3;
					delaycnt = 250;
					delay_flag = 1;
				break;
				case 3:
					Horizontal_Move(blockLength,velocity,90); 
					delaycnt = 100;
					delay_flag = 1;
					stage=5;
				break;	
				//经验调整
				//case 4:
				//	Horizontal_Move(blockLength,velocity,0); 
				//	delaycnt = 70;
				//	delay_flag = 1;
				//	stage=5;
				//break;
				//K210开启字母识别，根据A位置调整
				case 5:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 70;
					delay_flag = 1;
					stage=6;
				break;
				case 6:
					Horizontal_Move(k210.xoffset,20,k210.xdirection*180+90);
					delaycnt = 70;
					delay_flag = 1;
					stage=7;
				break;
				case 7:
					Horizontal_Move(k210.yoffset,20,k210.ydirection*180);
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
					Horizontal_Move(blockLength,velocity,direction);
					step = step + 1;
					delaycnt = 100;
					delay_flag = 1;
					stage=8;
				break;
				//惯导返航
				case 10:
					Horizontal_Move(5*blockLength,velocity,180);
					delaycnt = 300;
					delay_flag = 1;
					stage=11;
				break;
				case 11:
					Horizontal_Move(blockLength,velocity,270);
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
					//k210.number = 4;
					stage = 1;
					delaycnt = 200;
					delay_flag = 1;
				break;
				case 1:
					targetHeight = height;
					OneKey_Takeoff(height);
					stage = 20;
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
					Right_Rotate(180,90);
					stage = 70;
					delaycnt = 300;
					delay_flag = 1;
				break;
				case 70:
					openmv_cfg.mode = 1;
					dt.fun[0xf6].WTS= 1;
					delaycnt = 100;
					delay_flag = 1;
					stage = 21;
					break;
				case 21:
					if(openmv.xoffset !=0 | openmv.yoffset != 0) stage = 22;
					else{
						Horizontal_Move(50,velocity,270);
						openmv.xtotal += 50;
						delaycnt = 300;
						delay_flag = 1;
					}
				case 22:
					Horizontal_Move(openmv.xoffset,20,openmv.xdirection*180+90);
					if(openmv.xdirection == 1)	openmv.xtotal -= openmv.xoffset;
					else openmv.xtotal += openmv.xoffset;
					openmv.xoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=23;
					if(openmv.ready == 1) stage = 24;
				break;
				case 23:
					Horizontal_Move(openmv.yoffset,20,openmv.ydirection*180);
					openmv.ytotal += openmv.yoffset;
					openmv.yoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=22;
					if(openmv.ready == 1) stage = 24;
				break;
				//开始识别条码
				case 24:
					openmv_cfg.mode=0;
					dt.fun[0xf6].WTS=1;
					targetHeight = 115;
					Vertical_Down(35,10);
					k210_cfg.mode=4;
					dt.fun[0xf4].WTS=1;
					delaycnt = 250;
					delay_flag = 1;
					stage = 25;
				break;
				case 25:
					if(k210.number != 0){
						delaycnt = 100;
						delay_flag = 1;
						stage = 27;
					}
				break;
				//下降一次未识别到的异常情况
				case 26:
					OneKey_Land();
					break;
				//识别到数字后闪烁
				case 27:
						blink();
						delaycnt = 30;
						delay_flag = 1;
						counterLED++;
						//2*数字
						if(counterLED >= 2*k210.number){
							delaycnt = 150;
							delay_flag = 1;
							stage = 28;
							counterLED = 0;
						}
					break;
					case 28:
						blink();
						delaycnt = 30;
						delay_flag = 1;
						counterLED++;
						//2*数字
						if(counterLED >= 2*k210.number){
							delaycnt = 150;
							delay_flag = 1;
							stage = 29;
							counterLED = 0;
						}
					break;
					case 29:	
						targetHeight = 150;
						Vertical_Target(150);
						delaycnt = 100;
						delay_flag = 1;
						stage = 30;
						break;
					//回到十字
					case 30:
						Horizontal_Move(openmv.xtotal+50,50,90);
						delaycnt = 200;
						delay_flag = 1;
						stage = 31;
					break;
					case 31:
						Horizontal_Move(200-openmv.ytotal,50,0);
						k210_cfg.mode=3;
						dt.fun[0xf4].WTS=1;
						delaycnt = 200;
						delay_flag = 1;
						stage = 32;
					break;
					//十字直接降落，待完善
					case 32:
						OneKey_Land();
					break;
				case 41:
					Vertical_Down(20,20);
					stage= 12;
					delaycnt = 50;
					delay_flag = 1;
				break;
				//LED依据识别到的数字闪烁
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
	static u8 counterLED = 0;
	static u8 flag = 0;
	static u8 stage = 0;
	if(flag == 0){
		k210_cfg.mode=4;
		dt.fun[0xf4].WTS=1;
		flag = 1;
	}
	if(delay_flag){
			delay20();
	}
	else if(k210.number != 0 && stage == 0){
		blink();
		delaycnt = 30;
		delay_flag = 1;
		counterLED++;
		//2*数字
		if(counterLED >= 2*k210.number){
			delaycnt = 100;
			delay_flag = 1;
			stage = 1;
			counterLED = 0;
		}
	}
	else if(stage == 1){
		blink();
		delaycnt = 30;
		delay_flag = 1;
		counterLED++;
		//2*数字
		if(counterLED >= 2*k210.number){
			hmi.mode = 0;
		}
	}
	//hmi.mode = 0;
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


