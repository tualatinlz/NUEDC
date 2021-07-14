#include "User_Task.h"
#include "Drv_K210.h"
#include "Drv_HMI.h"
#include "LX_FC_Fun.h"

void UserTask_OneKeyCmd(void)
{
		static u8 counter1,counter2 = 0;
		u16 wholeLength = 3000;
		LX_Change_Mode(3);
		switch(hmi.mode){
			case 1:FC_Lock();
				break;
			case 2:OneKey_Land();
				break;
			case 3:FC_Unlock();
				break;
			case 4:	UserTask_FollowLine(wholeLength);
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
			if(k210.angel>180 && k210.angel<357)	Left_Rotate(360-k210.angel,10);
			else if(k210.angel<180 && k210.angel>3) Right_Rotate(k210.angel,10); 
			else {
				Horizontal_Move(distance,velocity,k210.angel);
				counter++;
			}
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
