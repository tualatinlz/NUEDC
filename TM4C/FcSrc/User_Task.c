#include "User_Task.h"
#include "LX_FC_Fun.h"
#include "Drv_K210.h"
#include "Drv_HMI.h"
#include "Drv_AnoOf.h"
#include "ANO_DT_LX.h"
static u8 maxcount;
static u8 delay_flag;
//dt.fun[0xf4].WTS = 1;

//һ���ӳٺ���
void delay20(){
	static u8 count = 0;
	if(count <= maxcount) count++;
	else{
		maxcount = 0;
		delay_flag = 0;
		count = 0;
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
			case 0x04:UserTask_FollowLine(wholeLength);
				break;
			case 0x05://test(80,10);
				break;
			case 0x06:rotate(80,10);
				break;
			case 0x07:UserTask_FollowLineN(wholeLength);
				break;
			case 0x10:Horizontal_Calibrate();
				break;
			case 0x11:Mag_Calibrate();
				break;
			case 0x12:ACC_Calibrate();
				break;
			case 0x13:GYR_Calibrate();
				break;			
		}
}

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

//����Ѳ��
void UserTask_FollowLineN(u8 wholeLength){
		static u16 counter = 0;
		u16 maxcnt = 50000; 
		static u8 count1 = 0;
		static u8 stage = 0;			//����ִ�н׶�
		u8 distance = 10;				  //ÿ���ƶ��ľ���
		u8 velocity = 25;	        //�ƶ��ٶ� ��С10cm/s
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
				count1++;
				//���ǰ�ȴ�
				if(count1>=150){
					stage = 1;
					count1=0;
				}
			}
		else if(stage == 1){
			OneKey_Takeoff(targetHeight);
			stage = 2;
			maxcount = 100;
			delay_flag = 1;
		}
		else if(stage == 2){
			//ǰ��
			Horizontal_Move(distance,velocity,90);			
			maxcount = distance * 50 / velocity;
			delay_flag = 1;
			stage = 3;
		}
		else if(stage == 3){
			if(k210.distance <= 100){
				Horizontal_Move(k210.distance-70,velocity,0);
				maxcount = (k210.distance-70) / velocity;
				delay_flag = 1;
				stage  = 4;
			}
			else stage = 2;
		}
		else if(stage == 4){
			//ǰ��
				Horizontal_Move(distance,velocity,90);			
				maxcount = distance * 50 / velocity;
				delay_flag = 1;
				//stage = 5;
		}
		else if(stage==5){
			//�߶ȵ���			
			if(k210.offset > 3){
				if(k210.leftorright == 0){
					Vertical_Up(k210.offset,10);
				}
				else if(k210.leftorright == 1){
					Vertical_Down(k210.offset,10);
				}
			}
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

void rotate(u16 r,u8 direction){
		static u16 counter = 0;
		u16 maxcnt = 50000;       //��ʱ����
		static u8 count1,count2 = 0;
		static u8 stage = 0;			//����ִ�н׶�
		u8 targetHeight = 100;    //���Ŀ��߶�
		u8 deg = 10;							//������ת�Ƕ�
		u8 degs = 10;							//��ת���ٶ� ��Ӧ����deg
		u8 distance = 10;					//�����ƶ����� �����ܳ�
		u8 velocity = 10;					//�����ƶ��ٶ�
		LX_Change_Mode(3);
	
		//�л�״̬ʱ����ֲ�����
		if(hmi.mode != hmi.oldmode){		
			count1 = 0;
			count2 = 0;	
			stage = 0;
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
				stage = 1;
				maxcount = 150;
				delay_flag = 1;
			}
			else if(stage == 1){
				OneKey_Takeoff(targetHeight);
				stage = 2;
				maxcount = 100;
				delay_flag = 1;
			}
			else if(stage == 2){
				Left_Rotate(deg,degs);
				maxcount = deg * 50 /degs;
				delay_flag = 1;
				stage = 3;
			}
			else if(stage==3){
				Horizontal_Move(distance,velocity,90);
				maxcount = distance * 50 / velocity;
				delay_flag = 1;
				stage = 4;
			}
			else if(stage==4){
				if(k210.distance<100)Horizontal_Move(10,10,180);
				maxcount = 50;
				delay_flag = 1;
				stage = 2;
			}
			//�������
			else if(stage == 5){
				OneKey_Land();
				count1 = 0;
				stage = 0;
				hmi.mode = 0;
			}
		if(counter >= maxcnt){
			stage = 5;
		}
	}
}