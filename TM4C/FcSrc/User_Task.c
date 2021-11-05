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
		static u8 counter1,counter2 = 0;
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
		static u16 counter = 0;
		counter ++;
		u16 maxcnt = 10000;
		static u8 sendflag=0;
		static u8 stage = 0;
		static u8 step = 0;
		static u8 counterLED = 0;
		u8 blockLength = 50;
		u8 velocity = 50;
		u8 vd = 90;	 
		LX_Change_Mode(3);
		
		if(hmi.mode != hmi.oldmode){
			counter = 0;
			stage = 0;
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
			if(stage==0){
				FC_Unlock();
				k210.number = 4;
				stage = 1;
				delaycnt = 200;
				delay_flag = 1;
			}
			else if(stage==1){
				OneKey_Takeoff(height);
				stage = 2;
				delaycnt = 300;
				delay_flag = 1;
			}
			else if(stage==2){
				Horizontal_Move(4*blockLength,velocity,0);
				stage = 3;
				delaycnt = 250;
				delay_flag = 1;
			}
			else if(stage == 3){
				Right_Rotate(90,90);
				step++;
				delaycnt = 70;
				delay_flag = 1;
				stage=54;
			}	
			else if(stage==54){
				Horizontal_Move(blockLength,velocity,0); 
				delaycnt = 50;
				delay_flag = 1;
				stage=50;
			}
			else if(stage==50){
				k210_cfg.mode=2;	
        dt.fun[0xf4].WTS=1; 
				delaycnt = 50;
				delay_flag = 1;
				stage=51;
			}
			else if(stage==51){
				Horizontal_Move(k210.xoffset,10,k210.xdirection*180+90);
				delaycnt = 70;
				delay_flag = 1;
				stage=52;
				//if(k210.land == 1)stage=5;
			}
			else if(stage==52){
				Horizontal_Move(k210.yoffset,10,k210.ydirection*180);
				delaycnt = 70;
				delay_flag = 1;
				stage=4;
				//if(k210.land == 1)stage=5;
			}
			
			else if(stage==4){
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
				stage = 5;
				delaycnt = 25;
				delay_flag = 1;
			}
			else if(stage == 5){
				Horizontal_Move(blockLength,velocity,90);
				step++;
				delaycnt = 50;
				delay_flag = 1;
				stage=10;
			}
			else if(stage==6){
				Horizontal_Move(5*blockLength,velocity,0);
				delaycnt = 50;
				delay_flag = 1;
				stage=7;
			}
			else if(stage==7){
				Horizontal_Move(blockLength,velocity,90);
				k210_cfg.mode=3;	
        dt.fun[0xf4].WTS=1;
				delaycnt = 50;
				delay_flag = 1;
				stage=8;
			}
			else if(stage==8){
				Horizontal_Move(k210.xoffset,50,k210.xdirection*180+90);
				delaycnt = 50;
				delay_flag = 1;
				stage=9;
				if(k210.land == 1)stage=10;
			}
			else if(stage==9){
				Horizontal_Move(k210.yoffset,50,k210.ydirection*180);
				delaycnt = 50;
				delay_flag = 1;
				stage=8;
				if(k210.land == 1)stage=10;
			}
			else if(stage==60){
				OneKey_Land();
			}
			else if(stage==10){
        k210_cfg.mode=4;
        dt.fun[0xf4].WTS=1;
				stage=11;
				delaycnt = 50;
				delay_flag = 1;
			}
			else if(stage==11){
				Vertical_Down(20,20);
				stage= 12;
				delaycnt = 50;
				delay_flag = 1;
			}
			else if(stage==12){
				blink();
				delaycnt = 25;
				delay_flag = 1;
				counterLED++;
				//2*数字
				if(counterLED >= k210.number * 2){
					OneKey_Land();
					//hmi.mode=0;
				}
			}
				
		if(counter >= maxcnt){
			OneKey_Land();
			counter = 0;
			stage = 0;
			hmi.mode = 0;
		}
		
	}
}


void spreadPU(u8 height){
		static u16 counter = 0;
		counter ++;
		u16 maxcnt = 10000;
		static u8 sendflag=0;
		static u8 stage = 0;
		static u8 step = 0;
		static u8 counterLED=0;
		u8 blockLength = 50;
		u8 velocity = 50;
		u8 vd = 90;	 
		LX_Change_Mode(3);
		
		if(hmi.mode != hmi.oldmode){
			counter = 0;
			stage = 0;
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
		}
		
		if(delay_flag){
			delay20();
		}
		else{
			if(stage==0){
				FC_Unlock();
				k210.number=4;	//IHWT
				stage = 1;
				delaycnt = 200;
				delay_flag = 1;
			}
			else if(stage==1){
				OneKey_Takeoff(height);
				stage = 2;
				delaycnt = 150;
				delay_flag = 1;
			}
			else if(stage==2){
				Horizontal_Move(4*blockLength,velocity,0);
				stage = 3;
				delaycnt = 200;
				delay_flag = 1;
			}
			else if(stage == 3){
				Right_Rotate(90,90);
				step++;
				delaycnt = 50;
				delay_flag = 1;
				stage=50;
			}	
			else if(stage==50){
				k210_cfg.mode=2;	
        dt.fun[0xf4].WTS=1; 
				delaycnt = 50;
				delay_flag = 1;
				stage=51;
			}
			else if(stage==51){
				Horizontal_Move(k210.xoffset,10,k210.xdirection*180+90);
				delaycnt = 50;
				delay_flag = 1;
				stage=52;
				//if(k210.land == 1)stage=5;
			}
			else if(stage==52){
				Horizontal_Move(k210.yoffset,10,k210.ydirection*180);
				delaycnt = 50;
				delay_flag = 1;
				stage=4;
				//if(k210.land == 1)stage=5;
			}
			
			else if(stage==4){
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
				delaycnt = 25;
				delay_flag = 1;
			}
			else if(stage == 5){
				Horizontal_Move(blockLength,velocity,0);
				step++;
				delaycnt = 50;
				delay_flag = 1;
				stage=4;
			}
			else if(stage==6){
				Horizontal_Move(blockLength,velocity,270);
				delaycnt = 50;
				delay_flag = 1;
				stage=7;
			}
			else if(stage==7){
				Horizontal_Move(blockLength,velocity,0);
				k210_cfg.mode=3;	
        dt.fun[0xf4].WTS=1;
				delaycnt = 50;
				delay_flag = 1;
				stage=8;
			}
			else if(stage==8){
				Horizontal_Move(openmv.xoffset,10,openmv.xdirection*180+90);
				delaycnt = 50;
				delay_flag = 1;
				stage=9;
				if(openmv.ready == 1)stage=10;
			}
			else if(stage==9){
				Horizontal_Move(openmv.yoffset,10,openmv.ydirection*180);
				delaycnt = 50;
				delay_flag = 1;
				stage=8;
				if(openmv.ready == 1)stage=10;
			}
			else if(stage==10){
				blink();
				delaycnt = 30;
				delay_flag = 1;
				counterLED++;
				if(counterLED >= k210.number*2) stage = 10;
			}
			else if(stage==10){
				OneKey_Land();
			}
				
		if(counter >= maxcnt){
			OneKey_Land();
			counter = 0;
			stage = 0;
			hmi.mode = 0;
		}
	}
}


void customF(){
	static u16 counter = 0;
	counter ++;
	u16 maxcnt = 10000;
	static u8 sendflag=0;
	static u8 stage = 0;
	static u8 step = 0;
	static u8 counterLED = 0;
	u8 blockLength = 50;
	u8 velocity = 50;
	u8 vd = 90;	 
	LX_Change_Mode(3);
	k210_cfg.mode=4;
  dt.fun[0xf4].WTS=1;
	stage=11;
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
