#include "User_Task.h"
#include "LX_FC_Fun.h"
#include "Drv_K210.h"
#include "Drv_HMI.h"
#include "Drv_AnoOf.h"
#include "ANO_DT_LX.h"
#include "Drv_PwmOut.h"
static u16 maxcount;
static u8 delay_flag;

//һ���ӳٺ��� һ���ӳ�20ms
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
//������ ������ͬģʽ
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
/*/������ѭ��
void UserTask_FollowLine(u8 wholeLength){
		static u16 counter = 0;
		static u8 count1 = 0;
		static u8 stage = 0;			//����ִ�н׶�
		u8 distance = 2;					//ÿ���ƶ��ľ���
		u8 velocity = 15;					//�ƶ��ٶ� ��С10cm/s
		u8 targetHeight = 100;
		u16 maxcnt = 50000; 
		LX_Change_Mode(3);
		
		//�л�״̬ʱ����ֲ�����
		if(hmi.mode != hmi.oldmode){
			counter = 0;
			stage = 0;
			count1 = 0;	
			hmi.oldmode = hmi.mode;
		}
		//�������ӳ� һ��20ms
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
			//�߶ȵ���
			if(ano_of.of_alt_cm>targetHeight + 3) Vertical_Down(ano_of.of_alt_cm-targetHeight,10);
			else if(ano_of.of_alt_cm<targetHeight - 3) Vertical_Up(targetHeight-ano_of.of_alt_cm,10);
			else{	
				//����Ƕȵ���
				if(k210.angel >180 && k210.angel<357)Left_Rotate(360-k210.angel,30);
				else if(k210.angel<180 && k210.angel>3) Right_Rotate(k210.angel,30); 
				//����λ�õ���
				if(k210.offset/2 > 3)	Horizontal_Move(k210.offset/2,velocity,k210.leftorright*180+90);
				else counter++;
				//ǰ��
				Horizontal_Move(distance,velocity,0);
				//ʶ��ָ�����ֽ���
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
//����Ѳ��
void UserTask_FollowLineN(u8 wholeLength){
		static u16 counter = 0;
		u16 maxcnt = 50000; 
		static u8 count1 = 0;
		static u8 stage = 0;			//����ִ�н׶�
		u8 distance = 20;				  //ÿ���ƶ��ľ���
		u8 velocity = 10;	        //�ƶ��ٶ� ��С10cm/s
		u8 targetHeight = 100;
		LX_Change_Mode(3);
		
		//�л�״̬ʱ����ֲ�����
		if(hmi.mode != hmi.oldmode){
			counter = 0;
			stage = 0;
			count1 = 0;	
			hmi.oldmode = hmi.mode;
		}
		//�������ӳ� һ��20ms
		if(delay_flag){
			delay20();
		}
		else{
			if(stage == 0){
				FC_Unlock();
				stage = 1;
				//���ǰ�ȴ�
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
				//ǰ��λ��
				Horizontal_Move(k210.yoffset,velocity,k210.ydirection*180);			
				maxcount = distance * 50 / velocity;
				delay_flag = 1;
				stage = 3;
			}
			else if(stage == 3){
				//����λ��
				Horizontal_Move(k210.xoffset,velocity,k210.xdirection*180 + 90);			
				maxcount = distance * 50 / velocity;
				delay_flag = 1;
				stage = 2;
				counter ++;
			}
			else if(stage == 4){
				//ǰ��
				Horizontal_Move(distance,velocity,90);			
				maxcount = distance * 50 / velocity;
				delay_flag = 1;
				stage = 5;
				//if(k210.barcode == 1) stage = 6; //ʶ�𵽶�ά��			
			}
			else if(stage==5){	//���ݵ��ߵ����ɻ��߶�
				//YAW�������
				Rotate(k210.leftorright,k210.angel);
				stage = 4;
			}

		//��ʱ����
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
		u16 maxcnt = 3000;        //60�뽵�� ��ֹ����
		static u8 count1 = 0;
		static u8 stage = 0;			//����ִ�н׶�
		u8 distance = 20;				  //ÿ���ƶ��ľ���
		u8 velocity = 10;	        //�ƶ��ٶ� ��С10cm/s
		u8 targetHeight = height;
		LX_Change_Mode(3);
		counter ++;
		//�л�״̬ʱ����ֲ�����
		if(hmi.mode != hmi.oldmode){
			counter = 0;
			stage = 0;
			count1 = 0;	
			hmi.oldmode = hmi.mode;
		}
		//�������ӳ� һ��20ms
		if(delay_flag){
			delay20();
		}
		else{
			if(stage == 0){
				FC_Unlock();
				stage = 1;
				//���ǰ�ȴ�4��
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
				//�ȶ���ͣ
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
			 
		//��ʱ����
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
		u16 maxcnt = 3000;        //60�뽵�� ��ֹ����
		static u8 count1 = 0;
		static u8 stage = 0;			//����ִ�н׶�
		u8 distance = 20;				  //ÿ���ƶ��ľ���
		u8 velocity = 10;	        //�ƶ��ٶ� ��С10cm/s
		u8 targetHeight = height;
		LX_Change_Mode(3);
		counter ++;
		//�л�״̬ʱ����ֲ�����
		if(hmi.mode != hmi.oldmode){
			counter = 0;
			stage = 0;
			count1 = 0;	
			hmi.oldmode = hmi.mode;
		}
		//�������ӳ� һ��20ms
		if(delay_flag){
			delay20();
		}
		else{
			if(stage == 0){
				FC_Unlock();
				stage = 1;
				//���ǰ�ȴ�
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
				//��ͣ
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
			else if(stage == 4){	//����ȷ��������Ѳ��

			}
			else if(stage==5){	//���ݵ��ߵ����ɻ��߶�

			}
			else if(stage==6){	//ʶ���������

			}
			else if(stage==7){	//����ȷ��������Ѳ��

			}
			else if(stage==8){	//���ݵ��ߵ����ɻ��߶�

			}
			else if(stage==9){	//�Ƹ�
				OneKey_Land();
				counter = 0;
				stage = 0;
				hmi.mode = 0;
			}
			 
		//��ʱ����
		if(counter >= maxcnt){
			OneKey_Land();
			counter = 0;
			stage = 0;
			hmi.mode = 0;
		}
	}
}

//ԭ����ת
void rotate(u16 r,u8 direction){
		static u16 counter = 0;
		static u8 stage = 1;				//����ִ�н׶�
		u8 deg = 20;								//������ת�Ƕ�
		u8 degs = 10;								//��ת���ٶ� ��Ӧ����deg
		u8 distance = 2*0.1736*r;		//�����ƶ����� �����ܳ�
		u8 velocity = distance;			//�����ƶ��ٶ�
	
		//�������ӳ� һ��20ms
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

//���ƶ������ ��һ����Ҫ��������
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
