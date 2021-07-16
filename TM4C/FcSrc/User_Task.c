#include "User_Task.h"
#include "LX_FC_Fun.h"
//#include "LX_FC_EXT_Sensor.h"
#include "Drv_K210.h"
#include "Drv_HMI.h"

//_k210_st k210;
void UserTask_OneKeyCmd(void)
{
		static u8 counter1,counter2 = 0;
		u16 wholeLength = 3000;
		LX_Change_Mode(3);
		k210.number = 5;
		switch(hmi.mode){
			case 0x01:FC_Lock();
				break;
			case 0x02:OneKey_Land();
				break;
			case 0x03:FC_Unlock();
				break;
			case 0x04:UserTask_FollowLine(wholeLength);
				break;
			case 0x05:test(80,7);
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
		u8 velocity = 10;	//移动速度 最小10cm/s
		static u8 stage = 0;			//流程执行阶段
		u16 maxcnt = wholeLength * 5; 
		LX_Change_Mode(3);
		if(hmi.oldmode != hmi.mode){
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
			OneKey_Takeoff(80);
			stage = 2;
		}
		else if(stage == 2){
			if(k210.angel >180 && k210.angel<357)	Left_Rotate(360-k210.angel,10);
			else if(k210.angel<180 && k210.angel>3) Right_Rotate(k210.angel,10); 
			else {
				Horizontal_Move(distance,velocity,k210.angel);
				counter++;
			}
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
		u8 distance = 2;	//每次移动的距离
		u8 velocity = 10;	//移动速度 最小10cm/s
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
			if(maxcnt % 200 <100) Vertical_Target(height + dh);
			else Vertical_Target(height - dh);
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
