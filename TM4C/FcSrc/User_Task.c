#include "User_Task.h"
#include "LX_FC_Fun.h"
#include "Drv_K210.h"
#include "Drv_HMI.h"
#include "Drv_AnoOf.h"
#include "ANO_DT_LX.h"
#include "Drv_PwmOut.h"
static u16 maxcount;
static u8 delay_flag;

//一级延迟函数 一次延迟20ms
void delay20(){
	static u16 count = 0;
	if(count <= maxcount) count++;
	else{
		maxcount = 0;
		delay_flag = 0;
		count = 0;
		fc_sta.rotating = 0;
	}
}
//主函数 包括不同模式
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
			case 0x04:solveMaze(80);
				break;
			case 0x05:test(100,0);
				break;
			case 0x06:
				break;
			case 0x07:
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
/*/地面线循迹
void UserTask_FollowLine(u8 wholeLength){
		static u16 counter = 0;
		static u8 count1 = 0;
		static u8 stage = 0;			//流程执行阶段
		u8 distance = 2;					//每次移动的距离
		u8 velocity = 15;					//移动速度 最小10cm/s
		u8 targetHeight = 100;
		u16 maxcnt = 50000; 
		LX_Change_Mode(3);
		
		//切换状态时清除局部变量
		if(hmi.mode != hmi.oldmode){
			counter = 0;
			stage = 0;
			count1 = 0;	
			hmi.oldmode = hmi.mode;
		}
		//非阻塞延迟 一次20ms
		if(delay_flag){
			delay20();
		}
		else{
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
			//高度调节
			if(ano_of.of_alt_cm>targetHeight + 3) Vertical_Down(ano_of.of_alt_cm-targetHeight,10);
			else if(ano_of.of_alt_cm<targetHeight - 3) Vertical_Up(targetHeight-ano_of.of_alt_cm,10);
			else{	
				//拐弯角度调整
				if(k210.angel >180 && k210.angel<357)Left_Rotate(360-k210.angel,30);
				else if(k210.angel<180 && k210.angel>3) Right_Rotate(k210.angel,30); 
				//左右位置调整
				if(k210.offset/2 > 3)	Horizontal_Move(k210.offset/2,velocity,k210.leftorright*180+90);
				else counter++;
				//前进
				Horizontal_Move(distance,velocity,0);
				//识别指定数字降落
				if(k210.number == 3) stage = 3;
			}
		}
		else if(stage==3){
			OneKey_Land();
			counter = 0;
			stage = 0;
			hmi.mode = 0;
		}
		if(counter >= maxcnt){
			stage=3;
		}
	}
}
*/
//电线巡迹
void UserTask_FollowLineN(u8 wholeLength){
		static u16 counter = 0;
		u16 maxcnt = 50000; 
		static u8 count1 = 0;
		static u8 stage = 0;			//流程执行阶段
		u8 distance = 20;				  //每次移动的距离
		u8 velocity = 10;	        //移动速度 最小10cm/s
		u8 targetHeight = 100;
		LX_Change_Mode(3);
		
		//切换状态时清除局部变量
		if(hmi.mode != hmi.oldmode){
			counter = 0;
			stage = 0;
			count1 = 0;	
			hmi.oldmode = hmi.mode;
		}
		//非阻塞延迟 一次20ms
		if(delay_flag){
			delay20();
		}
		else{
			if(stage == 0){
				FC_Unlock();
				stage = 1;
				//起飞前等待
				maxcount = 200;
				delay_flag = 1;
			}
			else if(stage == 1){
				OneKey_Takeoff(targetHeight);
				stage = 2;
				maxcount = 100;
				delay_flag = 1;
			}
			else if(stage == 2){
				//前后位置
				Horizontal_Move(k210.yoffset,velocity,k210.ydirection*180);			
				maxcount = distance * 50 / velocity;
				delay_flag = 1;
				stage = 3;
			}
			else if(stage == 3){
				//左右位置
				Horizontal_Move(k210.xoffset,velocity,k210.xdirection*180 + 90);			
				maxcount = distance * 50 / velocity;
				delay_flag = 1;
				stage = 2;
				counter ++;
			}
			else if(stage == 4){
				//前进
				Horizontal_Move(distance,velocity,90);			
				maxcount = distance * 50 / velocity;
				delay_flag = 1;
				stage = 5;
				//if(k210.barcode == 1) stage = 6; //识别到二维码			
			}
			else if(stage==5){	//根据电线调整飞机高度
				//YAW方向调整
				Rotate(k210.leftorright,k210.angel);
				stage = 4;
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



void solveMaze(u8 height){
		static u16 counter = 0;
		u16 maxcnt = 3000;        //60秒降落 防止意外
		static u8 count1 = 0;
		static u8 stage = 0;			//流程执行阶段
		u8 distance = 20;				  //每次移动的距离
		u8 velocity = 10;	        //移动速度 最小10cm/s
		u8 targetHeight = height;
		LX_Change_Mode(3);
		counter ++;
		//切换状态时清除局部变量
		if(hmi.mode != hmi.oldmode){
			counter = 0;
			stage = 0;
			count1 = 0;	
			hmi.oldmode = hmi.mode;
		}
		//非阻塞延迟 一次20ms
		if(delay_flag){
			delay20();
		}
		else{
			if(stage == 0){
				FC_Unlock();
				stage = 1;
				//起飞前等待4秒
				maxcount = 200;
				delay_flag = 1;
			}
			else if(stage == 1){
				OneKey_Takeoff(targetHeight);
				stage = 2;
				maxcount = 150;
				delay_flag = 1;
			}
			else if(stage == 2){
				//稳定悬停
				OneKey_Hang();
				maxcount = 50;
				delay_flag = 1;
				stage = 3;
			}
			else if(stage == 3){

			}
			else if(stage == 4){	

			}
			else if(stage==5){	

			}
			else if(stage==6){	

			}
			else if(stage==7){	

			}
			else if(stage==8){	

			}
			else if(stage==9){
				OneKey_Land();
				counter = 0;
				stage = 0;
				hmi.mode = 0;
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

void test(u16 height,u16 dh){
		static u16 counter = 0;
		u16 maxcnt = 3000;        //60秒降落 防止意外
		static u8 count1 = 0;
		static u8 stage = 0;			//流程执行阶段
		u8 distance = 20;				  //每次移动的距离
		u8 velocity = 10;	        //移动速度 最小10cm/s
		u8 targetHeight = height;
		LX_Change_Mode(3);
		counter ++;
		//切换状态时清除局部变量
		if(hmi.mode != hmi.oldmode){
			counter = 0;
			stage = 0;
			count1 = 0;	
			hmi.oldmode = hmi.mode;
		}
		//非阻塞延迟 一次20ms
		if(delay_flag){
			delay20();
		}
		else{
			if(stage == 0){
				FC_Unlock();
				stage = 1;
				//起飞前等待
				maxcount = 150;
				delay_flag = 1;
			}
			else if(stage == 1){
				OneKey_Takeoff(targetHeight);
				stage = 2;
				maxcount = 300;
				delay_flag = 1;
			}
			else if(stage == 2){
				//悬停
				OneKey_Hang();
				maxcount = 200;
				delay_flag = 1;
				stage = 3;
			}
			else if(stage == 3){
				Horizontal_Move(50,15,0);
				maxcount = 100;
				delay_flag = 1;
				stage = 2;
			}
			else if(stage == 4){	//距离确定后向右巡线

			}
			else if(stage==5){	//根据电线调整飞机高度

			}
			else if(stage==6){	//识别到条形码后

			}
			else if(stage==7){	//距离确定后向右巡线

			}
			else if(stage==8){	//根据电线调整飞机高度

			}
			else if(stage==9){	//绕杆
				OneKey_Land();
				counter = 0;
				stage = 0;
				hmi.mode = 0;
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

//原地自转
void rotate(u16 r,u8 direction){
		static u16 counter = 0;
		static u8 stage = 1;				//流程执行阶段
		u8 deg = 20;								//单次旋转角度
		u8 degs = 10;								//旋转角速度 不应大于deg
		u8 distance = 2*0.1736*r;		//单次移动距离 决定周长
		u8 velocity = distance;			//单次移动速度
	
		//非阻塞延迟 一次20ms
		if(delay_flag){
			delay20();
		}
		else{
			if(stage==1){
				if(direction == 0)Left_Rotate(deg,degs);
				else Right_Rotate(deg,degs);
				maxcount = deg * 50 /degs;
				delay_flag = 1;
				stage = 2;
			}
			else if(stage==2){
				if(direction == 0) Horizontal_Move(distance,velocity,90);
				else Horizontal_Move(distance,velocity,270);
				maxcount = distance * 50 / velocity;
				delay_flag = 1;
				//stage = 3;
				stage = 1;
			}
			else if(stage==3){
				if(k210.distance<100 && k210.distance>70)Horizontal_Move(k210.distance - 70,10,180);
				maxcount = 50;
				delay_flag = 1;
				stage = 1;
			}
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
