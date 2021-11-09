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
//��һ��˫0 �±��25
u8 map[4][44] ={{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
								{1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,1,1,1,0,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
								{1,1,1,1,1,0,0,0,1,1,1,1,0,0,0,1,1,1,1,0,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
								{1,1,1,1,1,1,0,0,0,1,1,0,0,0,1,1,1,1,1,0,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}};

//һ���ӳٺ��� һ���ӳ�20ms
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
//������ ������ͬģʽ
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
			case 0x04:spreadP(145);
				break;
			case 0x05:spreadPU(145);
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
		static u16 counter = 0;			//����ʱ�����
		counter ++;
		u16 maxcnt = 18000;					//������ʱ��
		static u8 stage = 19;				//����ִ�н׶�
		static u8 sendflag=0;				//���ͱ�־λ
		static u8 step = 1;					//�����ߵ��ڼ�����
		static u8 counterLED = 0;		//LED��˸����
		u8 blockLength = 50;				//���ӳ���
		u8 velocity = 50;						//ǰ���ٶ�
		static u16 direction = 90;	//ǰ������ 
		LX_Change_Mode(3);					//�л����̿�ģʽ
		static u8 xlilun = 38;
		static u8 ylilun = 30;
		static u8 xlilunl = 95;
		static u8 ylilunu = 35; //72
		
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
			case 6:direction = 180;
						step++;
						stage = 74;
				break;
			case 9:direction = 90;
				break;
			case 10:direction = 0;
						//stage = 70;
				break;
			case 12:
						stage = 72;
						step ++;
				break;
			case 15:direction = 90;
				break;
			case 16:direction = 180;
							step ++;
						  stage = 70;
				break;
			case 22:
							Horizontal_Move(40,40,90);
							delaycnt = 60;
							delay_flag = 1;
							step++;
				break;
			case 23:direction = 0;
				break;
			case 25:step ++;
							xlilun = 57;
						  stage = 70;			
				break;
			case 27:stage = 72;
							ylilun = 32;
							step ++;
				break;
			case 31:direction = 270;
							step ++;
							stage = 76;
				break;
			case 39:Horizontal_Move(blockLength,velocity,180);
							step++;
							delaycnt=70;
							delay_flag=1;
							stage = 10;
				break;
				default:
					break;
		}
		
		//����ƽ�Ʒ���ȷ������
		switch(direction){
			case 90:
				blockLength = 47;
				velocity = 47;
			break;
			case 270:
				blockLength = 52;
				velocity = 52;
			break;
			case 0:
				blockLength = 46;
				velocity = 46;
			break;
			case 180:
				blockLength = 47;
				velocity = 47;
			break;
		}
		if(delay_flag){
			delay20();
		}
		else{
			switch(stage){
				case 0:
					FC_Unlock();
					stage = 1;
					delaycnt = 200;
					delay_flag = 1;
				break;
				case 1:
					targetHeight = height;
					OneKey_Takeoff(height);
					stage = 2;
					delaycnt = 200;
					delay_flag = 1;
				break;
				case 2:
					Horizontal_Move(4*40,velocity,0);
					stage = 3;
					delaycnt = 250;
					delay_flag = 1;
				break;
				case 3:
					Horizontal_Move(20,velocity,90); 
					delaycnt = 100;
					delay_flag = 1;
					stage=4;
				break;	
				//K210������ĸʶ�𣬸���Aλ�õ���
				case 4:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 130;
					delay_flag = 1;
					stage=5;
				break;
				case 5:
					Horizontal_Move(k210.yoffset,20,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=6;
				break;
				case 6:
					Horizontal_Move(k210.xoffset,20,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=104;
				break;
				//K210������ĸʶ�𣬸���Aλ�õ���
				case 104:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 130;
					delay_flag = 1;
					stage=105;
				break;
				case 105:
					Horizontal_Move(k210.yoffset,20,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=106;
				break;
				case 106:
					Horizontal_Move(k210.xoffset,20,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=7;
				break;
				//����K210�ж��Ƿ���ɫ���ӵ��ж����ָ���ǰ��һ��
				case 7:
					OneKey_Hang();
				if(map[0][step]== 1){
					if(sendflag==0){
							k210_cfg.mode=5;
							//k210_cfg.go=1;
							dt.fun[0xf4].WTS=1;                   
							sendflag=1;
					}
					if(k210.next==1){
						stage = 8;
						k210_cfg.go=0;
						k210.next=0;
						sendflag=0;
					}
				}
				else stage = 8;
					delaycnt = 25;
					delay_flag = 1;
				break;
				case 8:
					if(step != 26) Horizontal_Move(blockLength,velocity,direction);
					step = step + 1;
					delaycnt = blockLength * 50 / velocity + 70;
					delay_flag = 1;
					stage=7;
				break;
				//�ߵ�����
				case 10:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 200;
					delay_flag = 1;
					stage=11;
				break;
				case 11:
					Horizontal_Move(k210.yoffset,20,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=12;
				break;
				case 12:
					Horizontal_Move(k210.xoffset,20,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=13;
				break;
				//ԭ·����
				case 13:
					Horizontal_Move(40,velocity,270);
					delaycnt = 150;
					delay_flag = 1;
					stage=14;
				break;
				case 14:
					Horizontal_Move(175,velocity,180);
					delaycnt = 230;
					delay_flag = 1;
					stage=114;
				break;
				case 114:
					k210_cfg.mode=3;
					dt.fun[0xf4].WTS=1;
					delaycnt = 200;
					delay_flag = 1;
					stage = 15;
				break;
				//����ʮ��λ���жϷɻ�λ�ã�K210���Ʒɻ����������
				case 15:
					Horizontal_Move(k210.xoffset,50,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=16;
					if(k210.land == 1) stage = 60;
				break;
				case 16:
					Horizontal_Move(k210.yoffset,50,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=15;
					if(k210.land == 1) stage = 60;
				break;
				case 60:
					OneKey_Land();
				break;
				//xУ��
				case 70:
					openmv_cfg.mode = 2;
					dt.fun[0xf6].WTS = 1;
					delaycnt = 200;
					delay_flag = 1;
					stage = 71;
				break;
				case 71:
					//if(xlilun > openmv.xdistance && xlilun - openmv.xdistance > 5) 
						//Horizontal_Move((xlilun-openmv.xdistance)*2,10,90);
					//else 
						if(xlilun < openmv.xdistance && openmv.xdistance - xlilun > 2)
						Horizontal_Move((openmv.xdistance - xlilun)*3,10,270);
					openmv.xdistance = xlilun;
					delaycnt = 150;
					delay_flag = 1;
					sendflag = 0;
					stage = 7;
				break;
				//y����
				case 72:
					openmv_cfg.mode = 2;
					dt.fun[0xf6].WTS = 1;
					delaycnt = 200;
					delay_flag = 1;
					stage = 73;
				break;
				case 73:
					if(ylilun > openmv.ydistance && ylilun - openmv.ydistance > 5) 
						Horizontal_Move(ylilun-openmv.ydistance,10,0);
					else if(ylilun < openmv.ydistance && openmv.ydistance - ylilun > 3)
						Horizontal_Move((openmv.ydistance - ylilun)*3,10,180);
					openmv.ydistance = ylilun;
					delaycnt = 150;
					delay_flag = 1;
					sendflag = 0;
					stage = 7;
				break;
					//xУ��
				case 74:
					openmv_cfg.mode = 2;
					dt.fun[0xf6].WTS = 1;
					delaycnt = 200;
					delay_flag = 1;
					stage = 75;
				break;
				case 75:
					if(xlilunl > openmv.xdistancel && xlilunl - openmv.xdistancel > 7) 
						Horizontal_Move((xlilunl-openmv.xdistancel)*1,10,270);
					else if(xlilunl < openmv.xdistancel && openmv.xdistancel - xlilunl > 5)
						Horizontal_Move((openmv.xdistancel - xlilunl)*3,10,90);
					openmv.xdistancel = xlilunl;
					delaycnt = 150;
					delay_flag = 1;
					sendflag = 0;
					stage = 7;
				break;
					//y����
				case 76:
					openmv_cfg.mode = 3;
					dt.fun[0xf6].WTS = 1;
					delaycnt = 250;
					delay_flag = 1;
					stage = 77;
				break;
				case 77:
					if(ylilunu > openmv.ydistanceu && ylilunu - openmv.ydistanceu > 3) 
						Horizontal_Move((ylilunu-openmv.ydistanceu)*2,10,180);
					else if(ylilunu < openmv.ydistanceu && openmv.ydistanceu - ylilunu > 5)
						Horizontal_Move(openmv.ydistanceu - ylilunu,10,0);
					openmv.ydistance = ylilun;
					delaycnt = 200;
					delay_flag = 1;
					sendflag = 0;
					stage = 7;
				break;
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

void spreadPU(u8 height){
		static u16 counter = 0;			//����ʱ�����
		counter ++;
		u16 maxcnt = 18000;					//������ʱ��
		static u8 stage = 19;				//����ִ�н׶�
		static u8 sendflag=0;				//���ͱ�־λ
		static u8 step = 1;					//�����ߵ��ڼ�����
		static u8 counterLED = 0;		//LED��˸����
		u8 blockLength = 50;				//���ӳ���
		u8 velocity = 50;						//ǰ���ٶ�
		static u16 direction = 90;	//ǰ������ 
		LX_Change_Mode(3);					//�л����̿�ģʽ
		static u8 xlilun = 38;
		static u8 ylilun = 30;
		static u8 xlilunl = 95;
		static u8 ylilunu = 35; //72
		
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
			case 6:direction = 180;
						step++;
						stage = 74;
				break;
			case 9:direction = 90;
				break;
			case 10:direction = 0;
						//stage = 70;
				break;
			case 12:
						stage = 72;
						step ++;
				break;
			case 15:direction = 90;
				break;
			case 16:direction = 180;
							step ++;
						  stage = 70;
				break;
			case 22:
							Horizontal_Move(40,40,90);
							delaycnt = 60;
							delay_flag = 1;
							step++;
				break;
			case 23:direction = 0;
				break;
			case 25:step ++;
							xlilun = 57;
						  stage = 70;			
				break;
			case 27:stage = 72;
							ylilun = 32;
							step ++;
				break;
			case 31:direction = 270;
							step ++;
							stage = 76;
				break;
			case 39:Horizontal_Move(blockLength,velocity,180);
							step++;
							delaycnt=70;
							delay_flag=1;
							stage = 50;
				break;
				default:
					break;
		}
		
		//����ƽ�Ʒ���ȷ������
		switch(direction){
			case 90:
				blockLength = 47;
				velocity = 47;
			break;
			case 270:
				blockLength = 52;
				velocity = 52;
			break;
			case 0:
				blockLength = 46;
				velocity = 46;
			break;
			case 180:
				blockLength = 47;
				velocity = 47;
			break;
		}
		if(delay_flag){
			delay20();
		}
		else{
			switch(stage){
				case 0:
					FC_Unlock();
					stage = 1;
					delaycnt = 200;
					delay_flag = 1;
				break;
				case 1:
					targetHeight = height;
					OneKey_Takeoff(height);
					stage = 2;
					delaycnt = 200;
					delay_flag = 1;
				break;
				case 2:
					Horizontal_Move(4*40,velocity,0);
					stage = 3;
					delaycnt = 250;
					delay_flag = 1;
				break;
				case 3:
					Horizontal_Move(20,velocity,90); 
					delaycnt = 100;
					delay_flag = 1;
					stage=4;
				break;	
				//K210������ĸʶ�𣬸���Aλ�õ���
				case 4:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 130;
					delay_flag = 1;
					stage=5;
				break;
				case 5:
					Horizontal_Move(k210.yoffset,20,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=6;
				break;
				case 6:
					Horizontal_Move(k210.xoffset,20,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=104;
				break;
				//K210������ĸʶ�𣬸���Aλ�õ���
				case 104:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 130;
					delay_flag = 1;
					stage=105;
				break;
				case 105:
					Horizontal_Move(k210.yoffset,20,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=106;
				break;
				case 106:
					Horizontal_Move(k210.xoffset,20,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=7;
				break;
				//����K210�ж��Ƿ���ɫ���ӵ��ж����ָ���ǰ��һ��
				case 7:
					OneKey_Hang();
					if(k210_cfg.map == 0){
						if(sendflag==0){
							k210_cfg.mode=1;
							k210_cfg.go=1;
							dt.fun[0xf4].WTS=1;                   
							sendflag=1;
						}
						if(k210.next==1){
							stage = 8;
							k210_cfg.go=0;
							k210.next=0;
							sendflag=0;
						}
					}
				else if(k210_cfg.map != 0 && map[0][step]== 1){
					if(sendflag==0){
							k210_cfg.mode=5;
							//k210_cfg.go=1;
							dt.fun[0xf4].WTS=1;                   
							sendflag=1;
					}
					if(k210.next==1){
						stage = 8;
						k210_cfg.go=0;
						k210.next=0;
						sendflag=0;
					}
				}
				else stage = 8;
					delaycnt = 25;
					delay_flag = 1;
				break;
				case 8:
					if(step != 26) Horizontal_Move(blockLength,velocity,direction);
					step = step + 1;
					delaycnt = blockLength * 50 / velocity + 70;
					delay_flag = 1;
					stage=7;
				break;
				//�ߵ�����
				case 10:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 200;
					delay_flag = 1;
					stage=11;
				break;
				case 11:
					Horizontal_Move(k210.yoffset,20,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=12;
				break;
				case 12:
					Horizontal_Move(k210.xoffset,20,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=13;
				break;
				//ԭ·����
				case 13:
					Horizontal_Move(40,velocity,90);
					delaycnt = 150;
					delay_flag = 1;
					stage=14;
				break;
				case 14:
					Horizontal_Move(175,velocity,0);
					delaycnt = 230;
					delay_flag = 1;
					stage=114;
				break;
				case 114:
					k210_cfg.mode=3;
					dt.fun[0xf4].WTS=1;
					delaycnt = 200;
					delay_flag = 1;
					stage = 15;
				break;
				//����ʮ��λ���жϷɻ�λ�ã�K210���Ʒɻ����������
				case 15:
					Horizontal_Move(k210.xoffset,50,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=16;
					if(k210.land == 1) stage = 60;
				break;
				case 16:
					Horizontal_Move(k210.yoffset,50,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=15;
					if(k210.land == 1) stage = 60;
				break;
				case 60:
					Horizontal_Move(k210.number*8,velocity,180);
					delaycnt = 250;
					delay_flag = 1;
					stage=15;
				break;
				case 61:
					OneKey_Land();
				break;
				//Ѱ�Ҹ��ӺͶ�ά��λ��
				case 50:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 200;
					delay_flag = 1;
					stage=51;
				break;
				case 51:
					Horizontal_Move(k210.yoffset,20,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=52;
				break;
				case 52:
					Horizontal_Move(k210.xoffset,20,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=20;
				break;
				case 20:
					Right_Rotate(180,90);
					stage = 70;
					delaycnt = 300;
					delay_flag = 1;
				break;
				case 21:
					openmv_cfg.mode = 1;
					dt.fun[0xf6].WTS= 1;
					delaycnt = 100;
					delay_flag = 1;
					stage = 22;
					break;
				case 22:
					if(openmv.xoffset !=0 | openmv.yoffset != 0) stage = 23;
					else{
						Horizontal_Move(50,velocity,270);
						openmv.xtotal += 50;
						delaycnt = 200;
						delay_flag = 1;
					}
				case 23:
					Horizontal_Move(openmv.xoffset,20,openmv.xdirection*180+90);
					if(openmv.xdirection == 1)	openmv.xtotal -= openmv.xoffset;
					else openmv.xtotal += openmv.xoffset;
					openmv.xoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=24;
					if(openmv.ready == 1) stage = 25;
				break;
				case 24:
					Horizontal_Move(openmv.yoffset,20,openmv.ydirection*180);
					openmv.ytotal += openmv.yoffset;
					openmv.yoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=22;
					if(openmv.ready == 1) stage = 24;
				break;
				//��ʼʶ������
				case 25:
					openmv_cfg.mode=0;
					dt.fun[0xf6].WTS=1;
					//�߶ȵ���
					targetHeight = 115;
					Vertical_Down(35,10);
					k210_cfg.mode=4;
					dt.fun[0xf4].WTS=1;
					delaycnt = 250;
					delay_flag = 1;
					stage = 26;
				break;
				case 26:
					delaycnt = 100;
					delay_flag = 1;
					stage = 27;
				break;
				//ʶ�����ֺ���˸
				case 27:
						blink();
						delaycnt = 30;
						delay_flag = 1;
						counterLED++;
						//2*����
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
					//2*����
					if(counterLED >= 2*k210.number){
						delaycnt = 150;
						delay_flag = 1;
						stage = 29;
						counterLED = 0;
					}
				break;
					//���س�ʼ�߶�
				case 29:	
					targetHeight = 145;
					Vertical_Target(145);
					delaycnt = 100;
					delay_flag = 1;
					stage = 30;
				break;
				case 30:
					Horizontal_Move(openmv.xtotal,20,90);
					delaycnt = 200;
					delay_flag = 1;
					stage = 31;
				break;
				case 31:
					Horizontal_Move(openmv.ytotal,20,180);
					delaycnt = 200;
					delay_flag = 1;
					stage = 10;
				break;
				//xУ��
				case 70:
					openmv_cfg.mode = 2;
					dt.fun[0xf6].WTS = 1;
					delaycnt = 200;
					delay_flag = 1;
					stage = 71;
				break;
				case 71:
					//if(xlilun > openmv.xdistance && xlilun - openmv.xdistance > 5) 
						//Horizontal_Move((xlilun-openmv.xdistance)*2,10,90);
					//else 
						if(xlilun < openmv.xdistance && openmv.xdistance - xlilun > 2)
						Horizontal_Move((openmv.xdistance - xlilun)*3,10,270);
					openmv.xdistance = xlilun;
					delaycnt = 150;
					delay_flag = 1;
					sendflag = 0;
					stage = 7;
				break;
				//y����
				case 72:
					openmv_cfg.mode = 2;
					dt.fun[0xf6].WTS = 1;
					delaycnt = 200;
					delay_flag = 1;
					stage = 73;
				break;
				case 73:
					if(ylilun > openmv.ydistance && ylilun - openmv.ydistance > 5) 
						Horizontal_Move(ylilun-openmv.ydistance,10,0);
					else if(ylilun < openmv.ydistance && openmv.ydistance - ylilun > 3)
						Horizontal_Move((openmv.ydistance - ylilun)*3,10,180);
					openmv.ydistance = ylilun;
					delaycnt = 150;
					delay_flag = 1;
					sendflag = 0;
					stage = 7;
				break;
					//xУ��
				case 74:
					openmv_cfg.mode = 2;
					dt.fun[0xf6].WTS = 1;
					delaycnt = 200;
					delay_flag = 1;
					stage = 75;
				break;
				case 75:
					if(xlilunl > openmv.xdistancel && xlilunl - openmv.xdistancel > 7) 
						Horizontal_Move((xlilunl-openmv.xdistancel)*1,10,270);
					else if(xlilunl < openmv.xdistancel && openmv.xdistancel - xlilunl > 5)
						Horizontal_Move((openmv.xdistancel - xlilunl)*3,10,90);
					openmv.xdistancel = xlilunl;
					delaycnt = 150;
					delay_flag = 1;
					sendflag = 0;
					stage = 7;
				break;
					//y����
				case 76:
					openmv_cfg.mode = 3;
					dt.fun[0xf6].WTS = 1;
					delaycnt = 250;
					delay_flag = 1;
					stage = 77;
				break;
				case 77:
					if(ylilunu > openmv.ydistanceu && ylilunu - openmv.ydistanceu > 3) 
						Horizontal_Move((ylilunu-openmv.ydistanceu)*2,10,180);
					else if(ylilunu < openmv.ydistanceu && openmv.ydistanceu - ylilunu > 5)
						Horizontal_Move(openmv.ydistanceu - ylilunu,10,0);
					openmv.ydistance = ylilun;
					delaycnt = 200;
					delay_flag = 1;
					sendflag = 0;
					stage = 7;
				break;
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

void spreadPUooo(u8 height){
		static u16 counter = 0;			//����ʱ�����
		counter ++;
		u16 maxcnt = 18000;					//������ʱ��
		static u8 stage = 19;				//����ִ�н׶�
		static u8 sendflag=0;				//���ͱ�־λ
		static u8 step = 15;					//�����ߵ��ڼ�����
		static u8 counterLED = 0;		//LED��˸����
		u8 blockLength = 50;				//���ӳ���
		u8 velocity = 50;						//ǰ���ٶ�
		static u16 direction = 90;	//ǰ������ 
		LX_Change_Mode(3);					//�л����̿�ģʽ
		static u8 xlilun = 45;
		static u8 ylilun = 33;
		static u8 xlilunl = 95;
		static u8 ylilunu = 75; //72
		
		if(hmi.mode != hmi.oldmode){
			counter = 0;
			stage = 0;
			sendflag = 0;
			step = 15;
			counterLED = 0;
			hmi.oldmode = hmi.mode;
		}
		switch(step){
			case 4:direction = 180;
				break;
			case 6:direction = 180;
						step++;
						stage = 74;
				break;
			case 9:direction = 90;
				break;
			case 10:direction = 0;
						//stage = 70;
				break;
			case 12:
						stage = 72;
						step ++;
				break;
			case 15:direction = 90;
				break;
			case 16:direction = 180;
							step ++;
						  stage = 70;
				break;
			case 22:
							Horizontal_Move(40,40,90);
							delaycnt = 60;
							delay_flag = 1;
							step++;
				break;
			case 23:direction = 0;
				break;
			case 24: //�޸���
							Horizontal_Move(40,40,0);
							delaycnt = 70;
							delay_flag = 1;
							step++;
				break;
			case 25:step ++;
							xlilun = 60;
						  stage = 70;			
				break;
			case 27:stage = 72;
							//ylilun = 35;
							ylilun = 30; //31
							step ++;
				break;
			case 31:direction = 270;
							step ++;
							stage = 76;
				break;
			case 39:Horizontal_Move(blockLength,velocity,180);
							step++;
							delaycnt=70;
							delay_flag=1;
							stage = 50;
				break;
				default:
					break;
		}
		
		//����ƽ�Ʒ���ȷ������
		switch(direction){
			case 90:
				blockLength = 47;
				velocity = 47;
			break;
			case 270:
				blockLength = 52;
				velocity = 52;
			break;
			case 0:
				blockLength = 46;
				velocity = 46;
			break;
			case 180:
				blockLength = 47;
				velocity = 47;
			break;
		}
		if(delay_flag){
			delay20();
		}
		else{
			switch(stage){
				case 0:
					FC_Unlock();
					if(k210.number == 0) k210.number = 4;
					stage = 1;
					delaycnt = 200;
					delay_flag = 1;
				break;
				case 1:
					targetHeight = height;
					OneKey_Takeoff(height);
					stage = 2;
					delaycnt = 200;
					delay_flag = 1;
				break;
				case 2:
					Horizontal_Move(4*40,velocity,0);
					stage = 3;
					delaycnt = 250;
					delay_flag = 1;
				break;
				case 3:
					Horizontal_Move(20,velocity,90); 
					delaycnt = 100;
					delay_flag = 1;
					stage=4;
				break;	
				//K210������ĸʶ�𣬸���Aλ�õ���
				case 4:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 130;
					delay_flag = 1;
					stage=5;
				break;
				case 5:
					Horizontal_Move(k210.yoffset,20,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=6;
				break;
				case 6:
					Horizontal_Move(k210.xoffset,20,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=104;
				break;
				//K210������ĸʶ�𣬸���Aλ�õ���
				case 104:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 130;
					delay_flag = 1;
					stage=105;
				break;
				case 105:
					Horizontal_Move(k210.yoffset,20,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=106;
				break;
				case 106:
					Horizontal_Move(k210.xoffset,20,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=7;
				break;
				//����K210�ж��Ƿ���ɫ���ӵ��ж����ָ���ǰ��һ��
				case 7:
					OneKey_Hang();
				if(k210_cfg.map == 0){
				if(sendflag==0){
							k210_cfg.mode=2;
							k210_cfg.go=1;
							dt.fun[0xf4].WTS=1;                   
							sendflag=1;
					}
					if(k210.next==1){
						stage = 8;
						k210_cfg.go=0;
						k210.next=0;
						sendflag=0;
					}
				}
					else if(k210_cfg.map != 0 && map[k210_cfg.map][step]== 1){
					if(sendflag==0){
							k210_cfg.mode=5;
							//k210_cfg.go=1;
							dt.fun[0xf4].WTS=1;                   
							sendflag=1;
					}
					if(k210.next==1){
						stage = 8;
						k210_cfg.go=0;
						k210.next=0;
						sendflag=0;
					}
				}
					else stage = 8;
					delaycnt = 25;
					delay_flag = 1;
				break;
				case 8:
					if(step != 26) Horizontal_Move(blockLength,velocity,direction);
					step = step + 1;
					delaycnt = blockLength * 50 / velocity + 70;
					delay_flag = 1;
					stage=7;
				break;
				//�ߵ�����
				case 10:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 200;
					delay_flag = 1;
					stage=11;
				break;
				case 11:
					Horizontal_Move(k210.yoffset,20,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=12;
				break;
				case 12:
					Horizontal_Move(k210.xoffset,20,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=13;
				break;
				//ԭ·����
				case 13:
					Horizontal_Move(40,velocity,270);
					delaycnt = 150;
					delay_flag = 1;
					stage=14;
				break;
				case 14:
					Horizontal_Move(175,velocity,180);
					delaycnt = 230;
					delay_flag = 1;
					stage=114;
				break;
				case 114:
					k210_cfg.mode=3;
					dt.fun[0xf4].WTS=1;
					delaycnt = 200;
					delay_flag = 1;
					stage = 15;
				break;
				//����ʮ��λ���жϷɻ�λ�ã�K210���Ʒɻ����������
				case 15:
					Horizontal_Move(k210.xoffset,50,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=16;
					if(k210.land == 1) stage = 60;
				break;
				case 16:
					Horizontal_Move(k210.yoffset,50,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=15;
					if(k210.land == 1) stage = 60;
				break;
				case 60:
					Horizontal_Move(k210.number*8,velocity,180);
					delaycnt = 250;
					delay_flag = 1;
					stage=15;
				break;
				case 61:
					OneKey_Land();
				break;
				//Ѱ�Ҹ��ӺͶ�ά��λ��
				case 50:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 200;
					delay_flag = 1;
					stage=51;
				break;
				case 51:
					Horizontal_Move(k210.yoffset,20,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=52;
				break;
				case 52:
					Horizontal_Move(k210.xoffset,20,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=20;
				break;
				case 20:
					Right_Rotate(180,90);
					stage = 70;
					delaycnt = 300;
					delay_flag = 1;
				break;
				case 21:
					openmv_cfg.mode = 1;
					dt.fun[0xf6].WTS= 1;
					delaycnt = 100;
					delay_flag = 1;
					stage = 22;
					break;
				case 22:
					if(openmv.xoffset !=0 | openmv.yoffset != 0) stage = 23;
					else{
						Horizontal_Move(50,velocity,270);
						openmv.xtotal += 50;
						delaycnt = 200;
						delay_flag = 1;
					}
				case 23:
					Horizontal_Move(openmv.xoffset,20,openmv.xdirection*180+90);
					if(openmv.xdirection == 1)	openmv.xtotal -= openmv.xoffset;
					else openmv.xtotal += openmv.xoffset;
					openmv.xoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=24;
					if(openmv.ready == 1) stage = 25;
				break;
				case 24:
					Horizontal_Move(openmv.yoffset,20,openmv.ydirection*180);
					openmv.ytotal += openmv.yoffset;
					openmv.yoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=22;
					if(openmv.ready == 1) stage = 24;
				break;
				//��ʼʶ������
				case 25:
					openmv_cfg.mode=0;
					dt.fun[0xf6].WTS=1;
					//�߶ȵ���
					targetHeight = 115;
					Vertical_Down(35,10);
					k210_cfg.mode=4;
					dt.fun[0xf4].WTS=1;
					delaycnt = 250;
					delay_flag = 1;
					stage = 26;
				break;
				case 26:
					delaycnt = 100;
					delay_flag = 1;
					stage = 27;
				break;
				//ʶ�����ֺ���˸
				case 27:
						blink();
						delaycnt = 30;
						delay_flag = 1;
						counterLED++;
						//2*����
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
					//2*����
					if(counterLED >= 2*k210.number){
						delaycnt = 150;
						delay_flag = 1;
						stage = 29;
						counterLED = 0;
					}
				break;
					//���س�ʼ�߶�
				case 29:	
					targetHeight = 145;
					Vertical_Target(145);
					delaycnt = 100;
					delay_flag = 1;
					stage = 30;
				break;
				case 30:
					Horizontal_Move(openmv.xtotal,20,90);
					delaycnt = 200;
					delay_flag = 1;
					stage = 31;
				break;
				case 31:
					Horizontal_Move(openmv.ytotal,20,180);
					delaycnt = 200;
					delay_flag = 1;
					stage = 10;
				break;
				//xУ��
				case 70:
					openmv_cfg.mode = 2;
					dt.fun[0xf6].WTS = 1;
					delaycnt = 200;
					delay_flag = 1;
					stage = 71;
				break;
				case 71:
					if(xlilun > openmv.xdistance && xlilun - openmv.xdistance > 3) 
						Horizontal_Move((xlilun-openmv.xdistance)*3,10,90);
					else if(xlilun < openmv.xdistance && openmv.xdistance - xlilun > 3)
						Horizontal_Move((openmv.xdistance - xlilun)*3,10,270);
					openmv.xdistance = xlilun;
					delaycnt = 150;
					delay_flag = 1;
					sendflag = 0;
					stage = 7;
				break;
				//y����
				case 72:
					openmv_cfg.mode = 2;
					dt.fun[0xf6].WTS = 1;
					delaycnt = 200;
					delay_flag = 1;
					stage = 73;
				break;
				case 73:
					if(ylilun > openmv.ydistance && ylilun - openmv.ydistance > 5) 
						Horizontal_Move(ylilun-openmv.ydistance,10,0);
					else if(ylilun < openmv.ydistance && openmv.ydistance - ylilun > 5)
						Horizontal_Move((openmv.ydistance - ylilun)*3,10,180);
					openmv.ydistance = ylilun;
					delaycnt = 150;
					delay_flag = 1;
					sendflag = 0;
					stage = 7;
				break;
					//xУ��
				case 74:
					openmv_cfg.mode = 2;
					dt.fun[0xf6].WTS = 1;
					delaycnt = 200;
					delay_flag = 1;
					stage = 75;
				break;
				case 75:
					if(xlilunl > openmv.xdistancel && xlilunl - openmv.xdistancel > 7) 
						Horizontal_Move((xlilunl-openmv.xdistancel)*1,10,270);
					else if(xlilunl < openmv.xdistancel && openmv.xdistancel - xlilunl > 5)
						Horizontal_Move((openmv.xdistancel - xlilunl)*3,10,90);
					//openmv.xdistancel = xlilunl;
					delaycnt = 150;
					delay_flag = 1;
					sendflag = 0;
					stage = 7;
				break;
					//y����
				case 76:
					openmv_cfg.mode = 3;
					dt.fun[0xf6].WTS = 1;
					delaycnt = 250;
					delay_flag = 1;
					stage = 77;
				break;
				case 77:
					if(ylilunu > openmv.ydistanceu && ylilunu - openmv.ydistanceu > 5) 
						Horizontal_Move((ylilunu-openmv.ydistanceu)*3,10,180);
					else if(ylilunu < openmv.ydistanceu && openmv.ydistanceu - ylilunu > 5)
						Horizontal_Move(openmv.ydistanceu - ylilunu,10,0);
					openmv.ydistance = ylilun;
					delaycnt = 150;
					delay_flag = 1;
					sendflag = 0;
					stage = 7;
				break;
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



void spreadPUoo(u8 height){
		static u16 counter = 0;			//����ʱ�����
		counter ++;
		u16 maxcnt = 18000;					//������ʱ��
		static u8 stage = 19;				//����ִ�н׶�
		static u8 sendflag=0;				//���ͱ�־λ
		static u8 step = 1;					//�����ߵ��ڼ�����
		static u8 counterLED = 0;		//LED��˸����
		u8 blockLength = 50;				//���ӳ���
		u8 velocity = 50;						//ǰ���ٶ�
		static u16 direction = 90;	//ǰ������ 
		LX_Change_Mode(3);					//�л����̿�ģʽ
		static u8 xlilun = 47;
		static u8 ylilun = 90;
		
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
						//stage = 70;
				break;
			case 8:direction = 90;
				break;
			case 9:direction = 0;
						//stage = 70;
				break;
			case 10:
						stage = 72;
						step ++;
				break;
			case 14:direction = 90;
				break;
			case 15:direction = 180;
							step ++;
						  stage = 70;
				break;
			case 21:
							Horizontal_Move(20,20,90);
							delaycnt = 60;
							delay_flag = 1;
							step++;
				break;
			case 22:direction = 0;	
				break;
			case 23:step ++;
						  stage = 70;
							xlilun = 12;
				break;
			case 24:step ++;
						  stage = 72;
							ylilun = 70;
				break;
			case 28:step ++;
						  stage = 70;
				break;
			case 29:direction = 270;
				break;
			case 37:Horizontal_Move(blockLength,velocity,180);
							step++;
							delaycnt=70;
							delay_flag=1;
							stage = 50;
				break;
				default:
					break;
		}
		
		//����ƽ�Ʒ���ȷ������
		switch(direction){
			case 90:
				blockLength = 45;
				velocity = 50;
			case 270:
				blockLength = 45;
				velocity = 50;
			break;
			case 0:
				blockLength = 10;
				velocity = 10;
			case 180:
				blockLength = 50;
				velocity = 50;
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
					delaycnt = 200;
					delay_flag = 1;
				break;
				case 2:
					Horizontal_Move(4*40,velocity,0);
					stage = 3;
					delaycnt = 250;
					delay_flag = 1;
				break;
				case 3:
					Horizontal_Move(20,velocity,90); 
					delaycnt = 100;
					delay_flag = 1;
					stage=4;
				break;	
				//K210������ĸʶ�𣬸���Aλ�õ���
				case 4:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 200;
					delay_flag = 1;
					stage=5;
				break;
				case 5:
					Horizontal_Move(k210.yoffset,20,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=6;
				break;
				case 6:
					Horizontal_Move(k210.xoffset,20,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=104;
				break;
				//K210������ĸʶ�𣬸���Aλ�õ���
				case 104:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 200;
					delay_flag = 1;
					stage=105;
				break;
				case 105:
					Horizontal_Move(k210.yoffset,20,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=106;
				break;
				case 106:
					Horizontal_Move(k210.xoffset,20,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=7;
				break;
				//����K210�ж��Ƿ���ɫ���ӵ��ж����ָ���ǰ��һ��
				case 7:
					OneKey_Hang();
					if(sendflag==0){
						k210_cfg.mode=1;
						k210_cfg.go=1;
						dt.fun[0xf4].WTS=1;                   
						sendflag=1;
					}
					if(k210.next==1){
						stage = 8;
						k210_cfg.go=0;
						k210.next=0;
						sendflag=0;
					}
					delaycnt = 25;
					delay_flag = 1;
				break;
				case 8:
					Horizontal_Move(blockLength,velocity,direction);
					step = step + 1;
					delaycnt = blockLength * 50 / velocity + 20;
					delay_flag = 1;
					stage=7;
				break;
				//�ߵ�����
				case 10:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 200;
					delay_flag = 1;
					stage=11;
				break;
				case 11:
					Horizontal_Move(k210.yoffset,20,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=12;
				break;
				case 12:
					Horizontal_Move(k210.xoffset,20,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=13;
				break;
				//ԭ·����
				case 13:
					Horizontal_Move(50,velocity,270);
					delaycnt = 300;
					delay_flag = 1;
					stage=14;
				break;
				case 14:
					Horizontal_Move(170,velocity,180);
					delaycnt = 230;
					delay_flag = 1;
					stage=114;
				break;
				case 114:
					k210_cfg.mode=3;
					dt.fun[0xf4].WTS=1;
					delaycnt = 200;
					delay_flag = 1;
					stage = 15;
				break;
				//����ʮ��λ���жϷɻ�λ�ã�K210���Ʒɻ����������
				case 15:
					Horizontal_Move(k210.xoffset,50,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=16;
					if(k210.land == 1) stage = 60;
				break;
				case 16:
					Horizontal_Move(k210.yoffset,50,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=15;
					if(k210.land == 1) stage = 60;
				break;
				case 60:
					Horizontal_Move(k210.number*8,velocity,180);
				break;
				case 61:
					OneKey_Land();
				break;
				//Ѱ�Ҹ��ӺͶ�ά��λ��
				case 50:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 200;
					delay_flag = 1;
					stage=51;
				break;
				case 51:
					Horizontal_Move(k210.yoffset,20,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=52;
				break;
				case 52:
					Horizontal_Move(k210.xoffset,20,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=20;
				break;
				case 20:
					Right_Rotate(180,90);
					stage = 70;
					delaycnt = 300;
					delay_flag = 1;
				break;
				case 21:
					openmv_cfg.mode = 1;
					dt.fun[0xf6].WTS= 1;
					delaycnt = 100;
					delay_flag = 1;
					stage = 22;
					break;
				case 22:
					if(openmv.xoffset !=0 | openmv.yoffset != 0) stage = 23;
					else{
						Horizontal_Move(50,velocity,270);
						openmv.xtotal += 50;
						delaycnt = 200;
						delay_flag = 1;
					}
				case 23:
					Horizontal_Move(openmv.xoffset,20,openmv.xdirection*180+90);
					if(openmv.xdirection == 1)	openmv.xtotal -= openmv.xoffset;
					else openmv.xtotal += openmv.xoffset;
					openmv.xoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=24;
					if(openmv.ready == 1) stage = 25;
				break;
				case 24:
					Horizontal_Move(openmv.yoffset,20,openmv.ydirection*180);
					openmv.ytotal += openmv.yoffset;
					openmv.yoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=22;
					if(openmv.ready == 1) stage = 24;
				break;
				//��ʼʶ������
				case 25:
					openmv_cfg.mode=0;
					dt.fun[0xf6].WTS=1;
					//�߶ȵ���
					targetHeight = 115;
					Vertical_Down(35,10);
					k210_cfg.mode=4;
					dt.fun[0xf4].WTS=1;
					delaycnt = 250;
					delay_flag = 1;
					stage = 26;
				break;
				case 26:
					delaycnt = 100;
					delay_flag = 1;
					stage = 27;
				break;
				//ʶ�����ֺ���˸
				case 27:
						blink();
						delaycnt = 30;
						delay_flag = 1;
						counterLED++;
						//2*����
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
					//2*����
					if(counterLED >= 2*k210.number){
						delaycnt = 150;
						delay_flag = 1;
						stage = 29;
						counterLED = 0;
					}
				break;
					//���س�ʼ�߶�
				case 29:	
					targetHeight = 145;
					Vertical_Target(145);
					delaycnt = 100;
					delay_flag = 1;
					stage = 30;
				break;
				case 30:
					Horizontal_Move(openmv.xtotal,20,90);
					delaycnt = 200;
					delay_flag = 1;
					stage = 31;
				break;
				case 31:
					Horizontal_Move(openmv.ytotal,20,180);
					delaycnt = 200;
					delay_flag = 1;
					stage = 10;
				break;
				//xУ��
				case 70:
					openmv_cfg.mode = 2;
					dt.fun[0xf6].WTS = 1;
					delaycnt = 150;
					delay_flag = 1;
					stage = 71;
				break;
				case 71:
					openmv.xdistance = openmv.xdistance * 2;
					openmv.xdistance = 160 - openmv.xdistance;
					if(xlilun > openmv.xdistance) Horizontal_Move((xlilun-openmv.xdistance),10,270);
					//else Horizontal_Move(openmv.xdistance - lilun,20,90);
					delaycnt = 100;
					delay_flag = 1;
					sendflag = 0;
					stage = 7;
				break;
				//y����
				case 72:
					openmv_cfg.mode = 2;
					dt.fun[0xf6].WTS = 1;
					delaycnt = 150;
					delay_flag = 1;
					stage = 73;
				break;
				case 73:
					openmv.ydistance = openmv.ydistance * 2;
					openmv.ydistance = 120 - openmv.ydistance;
					if(ylilun > openmv.ydistance) Horizontal_Move(ylilun-openmv.ydistance,10,180);
					//else Horizontal_Move(openmv.ydistance - 65,20,0);
					delaycnt = 100;
					delay_flag = 1;
					sendflag = 0;
					stage = 7;
				break;
				
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



void spreadPUo(u8 height){
		static u16 counter = 0;			//����ʱ�����
		counter ++;
		u16 maxcnt = 18000;					//������ʱ��
		static u8 stage = 0;				//����ִ�н׶�
		static u8 sendflag=0;				//���ͱ�־λ
		static u8 step = 8;					//�����ߵ��ڼ�����
		static u8 counterLED = 0;		//LED��˸����
		u8 blockLength = 50;				//���ӳ���
		u8 velocity = 50;						//ǰ���ٶ�
		static u16 direction = 90;	//ǰ������ 
		LX_Change_Mode(3);					//�л����̿�ģʽ
		static u8 xlilun = 75;
		static u8 ylilun = 50;
		
		if(hmi.mode != hmi.oldmode){
			counter = 0;
			stage = 0;
			sendflag = 0;
			step = 8;
			counterLED = 0;
			hmi.oldmode = hmi.mode;
		}
		switch(step){
			case 4:direction = 180;
						//stage = 70;
				break;
			case 8:direction = 90;
				break;
			case 9:direction = 0;
						//stage = 70;
				break;
			case 10:
						stage = 72;
						step ++;
				break;
			case 13:direction = 90;
				break;
			case 14:direction = 180;
							step ++;
						  stage = 70;
				break;
			case 18:direction = 90;
				break;
			case 19:direction = 0;
							step++;
						  stage = 70;
							xlilun = 25;
				break;
			case 20:direction = 0;
							step++;
						  stage = 72;
				break;
			case 21:
							step++;
						  stage = 70;
							xlilun = 25;
				break;
			case 22:
							step++;
						  stage = 70;
							xlilun = 25;
				break;
			case 23:
							step++;
						  stage = 70;
							xlilun = 25;
				break;
			case 24:direction = 270;
							step++;
						  stage = 70;
				break;
			case 30:Horizontal_Move(blockLength,velocity,180);
							step++;
							delaycnt=70;
							delay_flag=1;
							stage = 60;
				break;
				default:
					break;
		}
		
		//����ƽ�Ʒ���ȷ������
		switch(direction){
			case 90:
				blockLength = 40;
				velocity = 50;
			case 270:
				blockLength = 45;
				velocity = 50;
			break;
			case 0:
				blockLength = 10;
				velocity = 10;
			case 180:
				blockLength = 50;
				velocity = 50;
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
					stage = 7;
					delaycnt = 400;
					delay_flag = 1;
				break;
				case 2:
					Horizontal_Move(4*40,velocity,0);
					stage = 3;
					delaycnt = 250;
					delay_flag = 1;
				break;
				case 3:
					Horizontal_Move(20,velocity,90); 
					delaycnt = 100;
					delay_flag = 1;
					stage=4;
				break;	
				//K210������ĸʶ�𣬸���Aλ�õ���
				case 4:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 200;
					delay_flag = 1;
					stage=5;
				break;
				case 5:
					Horizontal_Move(k210.yoffset,20,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=6;
				break;
				case 6:
					Horizontal_Move(k210.xoffset,20,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=104;
				break;
				//K210������ĸʶ�𣬸���Aλ�õ���
				case 104:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 200;
					delay_flag = 1;
					stage=105;
				break;
				case 105:
					Horizontal_Move(k210.yoffset,20,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=106;
				break;
				case 106:
					Horizontal_Move(k210.xoffset,20,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=7;
				break;
				//����K210�ж��Ƿ���ɫ���ӵ��ж����ָ���ǰ��һ��
				case 7:
					OneKey_Hang();
					if(sendflag==0){
						k210_cfg.mode=1;
						k210_cfg.go=1;
						dt.fun[0xf4].WTS=1;                   
						sendflag=1;
					}
					if(k210.next==1){
						stage = 8;
						k210_cfg.go=0;
						k210.next=0;
						sendflag=0;
					}
					delaycnt = 25;
					delay_flag = 1;
				break;
				case 8:
					Horizontal_Move(blockLength,velocity,direction);
					step = step + 1;
					delaycnt = blockLength * 50 / velocity + 20;
					delay_flag = 1;
					stage=7;
				break;
				//��λ��A��
				case 10:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 200;
					delay_flag = 1;
					stage=11;
				break;
				case 11:
					Horizontal_Move(k210.yoffset,20,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=12;
				break;
				case 12:
					Horizontal_Move(k210.xoffset,20,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=13;
				break;
				//ԭ·����
				case 13:
					Horizontal_Move(20,velocity,270);
					delaycnt = 300;
					delay_flag = 1;
					stage=14;
				break;
				case 14:
					Horizontal_Move(160,velocity,270);
					k210_cfg.mode=3;
					dt.fun[0xf4].WTS=1;
					delaycnt = 200;
					delay_flag = 1;
					stage=15;
				break;
				//����ʮ��λ���жϷɻ�λ�ã�K210���Ʒɻ����������
				case 15:
					Horizontal_Move(k210.xoffset,50,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=16;
					if(k210.land == 1)stage=60;
				break;
				case 16:
					Horizontal_Move(k210.yoffset,50,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=15;
					if(k210.land == 1)stage=50;
				break;
				//Ѱ�Ҹ��ӺͶ�ά��λ��
				case 50:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 200;
					delay_flag = 1;
					stage=51;
				break;
				case 51:
					Horizontal_Move(k210.yoffset,20,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=52;
				break;
				case 52:
					Horizontal_Move(k210.xoffset,20,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 70;
					delay_flag = 1;
					stage=20;
				break;
				case 20:
					Right_Rotate(180,90);
					stage = 70;
					delaycnt = 300;
					delay_flag = 1;
				break;
				case 21:
					openmv_cfg.mode = 1;
					dt.fun[0xf6].WTS= 1;
					delaycnt = 100;
					delay_flag = 1;
					stage = 22;
					break;
				case 22:
					if(openmv.xoffset !=0 | openmv.yoffset != 0) stage = 23;
					else{
						Horizontal_Move(50,velocity,270);
						openmv.xtotal += 50;
						delaycnt = 200;
						delay_flag = 1;
					}
				case 23:
					Horizontal_Move(openmv.xoffset,20,openmv.xdirection*180+90);
					if(openmv.xdirection == 1)	openmv.xtotal -= openmv.xoffset;
					else openmv.xtotal += openmv.xoffset;
					openmv.xoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=24;
					if(openmv.ready == 1) stage = 25;
				break;
				case 24:
					Horizontal_Move(openmv.yoffset,20,openmv.ydirection*180);
					openmv.ytotal += openmv.yoffset;
					openmv.yoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=22;
					if(openmv.ready == 1) stage = 24;
				break;
				//��ʼʶ������
				case 25:
					openmv_cfg.mode=0;
					dt.fun[0xf6].WTS=1;
					targetHeight = 115;
					Vertical_Down(35,10);
					k210_cfg.mode=4;
					dt.fun[0xf4].WTS=1;
					delaycnt = 250;
					delay_flag = 1;
					stage = 26;
				break;
				case 26:
					if(k210.number != 0){
						delaycnt = 100;
						delay_flag = 1;
						stage = 27;
					}
				break;
				//ʶ�����ֺ���˸
				case 27:
						blink();
						delaycnt = 30;
						delay_flag = 1;
						counterLED++;
						//2*����
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
					//2*����
					if(counterLED >= 2*k210.number){
						delaycnt = 150;
						delay_flag = 1;
						stage = 29;
						counterLED = 0;
					}
				break;
					//���س�ʼ�߶�
				case 29:	
					targetHeight = 150;
					Vertical_Target(150);
					delaycnt = 100;
					delay_flag = 1;
					stage = 30;
				break;
				case 30:
					Horizontal_Move(openmv.xtotal,20,90);
					delaycnt = 200;
					delay_flag = 1;
					stage = 31;
				break;
				case 31:
					Horizontal_Move(openmv.ytotal,20,180);
					delaycnt = 200;
					delay_flag = 1;
					stage = 10;
				break;

					//������������ָ��λ��
				
				//ʮ��ֱ�ӽ��䣬������
				//LED����ʶ�𵽵�������˸
				case 60:
					OneKey_Land();
				break;
				//xУ��
				case 70:
					openmv_cfg.mode = 2;
					dt.fun[0xf6].WTS = 1;
					delaycnt = 150;
					delay_flag = 1;
					stage = 71;
				break;
				case 71:
					openmv.xdistance = openmv.xdistance * 2;
					openmv.xdistance = 160 - openmv.xdistance;
					if(xlilun > openmv.xdistance) Horizontal_Move(75-openmv.xdistance,20,270);
					//else Horizontal_Move(openmv.xdistance - lilun,20,90);
					delaycnt = 200;
					delay_flag = 1;
					sendflag = 0;
					stage = 7;
				break;
				//y����
				case 72:
					openmv_cfg.mode = 2;
					dt.fun[0xf6].WTS = 1;
					delaycnt = 150;
					delay_flag = 1;
					stage = 73;
				break;
				case 73:
					openmv.ydistance = openmv.ydistance * 2;
					openmv.ydistance = 120 - openmv.ydistance;
					if(ylilun > openmv.ydistance) Horizontal_Move(ylilun-openmv.ydistance,20,180);
					//else Horizontal_Move(openmv.ydistance - 65,20,0);
					delaycnt = 100;
					delay_flag = 1;
					sendflag = 0;
					stage = 7;
				break;
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
		//2*����
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
		//2*����
		if(counterLED >= 2*k210.number){
			hmi.mode = 0;
		}
	}
	//hmi.mode = 0;
}
//����LED��˸������һ���л�һ��״̬
void blink(void){
	u8 color = 7;
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


