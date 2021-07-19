#include "User_Task.h"
#include "LX_FC_Fun.h"
#include "Drv_K210.h"
#include "Drv_HMI.h"
#include "Drv_AnoOf.h"

void UserTask_OneKeyCmd(void)
{
		static u8 counter1,counter2 = 0;
		u16 wholeLength = 30000;
		LX_Change_Mode(3);
		switch(hmi.mode){
			case 0x01:FC_Lock();
				break;
			case 0x02:OneKey_Land();
				break;
			case 0x03:FC_Unlock();
				break;
			case 0x04:UserTask_FollowLine(wholeLength);
				break;
			case 0x05:test(80,10);
				break;
			case 0x10: Horizontal_Calibrate();
				break;
			case 0x11: Mag_Calibrate();
				break;
			case 0x12: ACC_Calibrate();
				break;
			case 0x13: GYR_Calibrate();
				break;			
		}
}

void UserTask_FollowLine(u8 wholeLength){
		static u16 counter = 0;
		static u8 count1 = 0;
		u8 distance = 2;	//每次移动的距离
		u8 velocity = 15;	//移动速度 最小10cm/s
		static u8 stage = 0;			//流程执行阶段
		u8 targetHeight = 100;
		u16 maxcnt = 50000; 
		LX_Change_Mode(3);

		if(hmi.mode != hmi.oldmode){
			counter = 0;
			stage = 0;
			count1 = 0;	
			hmi.oldmode = hmi.mode;
		}
		
		if(stage == 0){
			FC_Unlock();
			count1++;
			if(count1>=150){
				stage = 1;
				count1=0;
			}
		}
		else if(stage == 1){
			OneKey_Takeoff(targetHeight);
			stage = 2;
		}
		else if(stage == 2){
				if(k210.angel >180 && k210.angel<357)Left_Rotate(360-k210.angel,30);
				else if(k210.angel<180 && k210.angel>3) Right_Rotate(k210.angel,30); 
				else {
					if(k210.offset/2 > 3)	Horizontal_Move(k210.offset/2,velocity,k210.leftorright*180+90);
					else counter++;
					Horizontal_Move(distance,velocity,0);
				}
				
				if(ano_of.of_alt_cm>targetHeight + 3) Vertical_Down(ano_of.of_alt_cm-targetHeight,10);
				else if(ano_of.of_alt_cm<targetHeight - 3) Vertical_Up(targetHeight-ano_of.of_alt_cm,10);
				if(k210.number == 3) stage = 3;
		}
		else if(stage==3){
			OneKey_Land();
			hmi.mode = 0;
			stage = 0;
			counter = 0;
		}
		if(counter >= maxcnt){
			stage=3;
		}
}

void test(u16 height,u16 dh){
		static u16 counter = 0;
		static u8 count1 = 0;
		static u8 stage = 0;			//流程执行阶段
		u16 maxcnt = 2000; 
		LX_Change_Mode(3);
		//切换状态时清除局部变量
		if(hmi.oldmode != hmi.mode){
			counter = 0;
			stage = 0;
			count1 = 0;	
			hmi.oldmode = hmi.mode;
		}			
		if(stage == 0){
			FC_Unlock();
			count1++;
			if(count1>=100){
				stage = 1;
				count1=0;
			}
		}
		else if(stage == 1){
			OneKey_Takeoff(height);
			stage = 2;
		}
		else if(stage == 2){
			if(maxcnt % 200 <100) Horizontal_Move(1,10,90);
			else Horizontal_Move(1,10,270);
			maxcnt++;
		}
		else if(stage==3){
			OneKey_Land();
			hmi.mode = 0;
			stage = 0;
			counter = 0;
		}

		if(counter >= maxcnt){
			stage=3;
		}
}

void f2021(u16 height,u16 dh){
		static u16 counter = 0;
		static u8 count1 = 0;
		static u8 stage = 0;			//流程执行阶段
		u16 maxcnt = 2000; 
		LX_Change_Mode(3);
		//切换状态时清除局部变量
		if(hmi.oldmode != hmi.mode){
			counter = 0;
			stage = 0;
			count1 = 0;	
			hmi.oldmode = hmi.mode;
		}			
		if(stage == 0){
			FC_Unlock();
			count1++;
			if(count1>=100){
				stage = 1;
				count1=0;
			}
		}
		else if(stage == 1){
			OneKey_Takeoff(100);
			stage = 2;
		}
		else if(stage == 2){
			Horizontal_Move(2,5,90);
			
		}
		else if(stage==3){
			OneKey_Land();
			hmi.mode = 0;
			stage = 0;
			counter = 0;
		}

		if(counter >= maxcnt){
			stage=3;
		}
}