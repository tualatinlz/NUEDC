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
u8 map[2][44] ={{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
				{1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,0,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
				};

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
			case 0x07:openmv_cfg.mode = 6;
					dt.fun[0xf6].WTS = 1;
					hmi.mode = 0;
				break;
			case 0x08:drawNum(145);
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
			default:
				break;
		}
}

void spreadP(u8 height){
		static u16 counter = 0;			//����ʱ�����
		counter ++;
		u16 maxcnt = 18000;				//������ʱ��
		static u8 stage = 0;			//����ִ�н׶�
		static u8 sendflag=0;			//���ͱ�־λ
		static u8 corrflag=0;			//У����־λ
		static u8 step = 1;				//�����ߵ��ڼ�����
		static u8 counterLED = 0;		//LED��˸����
		u8 blockLength = 50;			//���ӳ���
		u8 velocity = 50;				//ǰ���ٶ�
		static u16 direction = 90;		//ǰ������ 
		LX_Change_Mode(3);				//�л����̿�ģʽ
		static u8 xlilun = 38;
		static u8 ylilun = 30;
		static u8 xlilunl = 90;
		
		if(hmi.mode != hmi.oldmode){
			counter = 0;
			stage = 0;
			sendflag = 0;
			corrflag=0;
			step = 1;
			counterLED = 0;
			hmi.oldmode = hmi.mode;
		}
		switch(step){
			case 4:direction = 180;
					if(corrflag == 0){
						stage = 80;
						openmv_cfg.mode = 5;
						xlilun = 82;
						ylilun = 107;
						corrflag = 1;
					}
				break;
			case 6:
					//if(corrflag == 0){
					//	stage = 74;
					//	xlilunl = 85;
					//	corrflag = 1;
					//}
				break;
			case 8:direction = 90;
					if(corrflag == 0){
						xlilun = 84;
						ylilun = 78;
						stage = 80;
						openmv_cfg.mode = 6;
						corrflag = 1;
					}
				break;
			case 9:direction = 0;
				break;
			case 11:
					if(corrflag == 0){
						stage = 72;
						ylilun = 32;
						corrflag = 1;
					}
				break;
			case 13:direction = 90;
				break;
			case 14:direction = 180;
				break;
			case 18:direction = 90;
				break;
			case 19:direction = 0;
				if(corrflag == 0){
					stage = 80;
					openmv_cfg.mode = 7;
					xlilun = 125;
					ylilun = 70;
					corrflag = 1;
				}
				break;
			case 21:
				if(corrflag == 0){
					stage = 72;
					ylilun = 32;//x=57
					corrflag = 1;
				}		
				break;
			case 24:direction = 270;
				if(corrflag == 0){
					stage = 80;
					openmv_cfg.mode = 8;
					xlilun = 130;
					ylilun = 12;
					corrflag = 1;
				}
				break;
			case 30:direction = 180;
				if(corrflag == 0){
					stage = 80;
					xlilun = 106;
					ylilun = 10;
					openmv_cfg.mode = 9;
					corrflag = 1;
				}
				break;
			case 31:
				stage = 10;
				step++;
				break;
			default:
				break;
		}
		
		//����ƽ�Ʒ���ȷ������
		switch(direction){
			case 90:
				blockLength = 46;
				velocity = 46;
			break;
			case 270:
				blockLength = 50;
				velocity = 50;
			break;
			case 0:
				blockLength = 46;
				velocity = 46;
			break;
			case 180:
				blockLength = 48;
				velocity = 48;
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
					Horizontal_Move(4*43,velocity,0);
					stage = 3;
					delaycnt = 250;
					delay_flag = 1;
				break;
				case 3:
					Horizontal_Move(25,velocity,90); 
					delaycnt = 100;
					delay_flag = 1;
					stage=4;
				break;	
				//K210������ĸʶ�𣬸���Aλ�õ���
				case 4:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 150;
					delay_flag = 1;
					stage=5;
				break;
				case 5:
					Horizontal_Move(k210.yoffset,20,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=6;
				break;
				case 6:
					Horizontal_Move(k210.xoffset,20,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=104;
				break;
				//K210������ĸʶ�𣬸���Aλ�õ���
				case 104:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 150;
					delay_flag = 1;
					stage=105;
				break;
				case 105:
					Horizontal_Move(k210.yoffset,20,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=106;
				break;
				case 106:
					Horizontal_Move(k210.xoffset,20,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 50;
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
					//if(step == 8) Horizontal_Move(37,37,90);
					if(step == 13) Horizontal_Move(43,43,90);
					//else if(step == 18) Horizontal_Move(37,37,90);
					//else if(step == 29) Horizontal_Move(40,40,0);
					else Horizontal_Move(blockLength,velocity,direction);
					step = step + 1;
					delaycnt = blockLength * 50 / velocity + 50;
					delay_flag = 1;
					corrflag = 0;
					stage=7;
				break;
				//�ߵ�����
				case 10:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 150;
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
					delaycnt = 150;
					delay_flag = 1;
					stage = 15;
				break;
				//����ʮ��λ���жϷɻ�λ�ã�K210���Ʒɻ����������
				case 15:
					if(k210.land == 1){
						Vertical_Down(55,40);
						targetHeight = 90;
						stage = 60;
						delaycnt = 200;
						delay_flag = 1;
					}
					else if(k210.xoffset ==0 && k210.yoffset ==0){
						Horizontal_Move(20,20,270);
						delaycnt = 60;
						delay_flag = 1;
					}
					else{
						Horizontal_Move(k210.xoffset,20,k210.xdirection*180+90);
						k210.xoffset = 0;
						delaycnt = 50;
						delay_flag = 1;
						stage=16;
					}
				break;
				case 16:
					if(k210.land == 1){
						Vertical_Down(55,40);
						targetHeight = 90;
						stage = 60;
						delaycnt = 200;
						delay_flag = 1;
					}
					else{
						Horizontal_Move(k210.yoffset,20,k210.ydirection*180);
						k210.yoffset = 0;
						delaycnt = 50;
						delay_flag = 1;
						stage=15;
					}
				break;
				case 60:
					k210.land = 0;
					k210_cfg.mode=3;
					dt.fun[0xf4].WTS=1;
					delaycnt = 150;
					delay_flag = 1;
					stage = 61;
				break;
				case 61:
					if(k210.land == 1){
						stage = 63;
					}
					else{
						Horizontal_Move(k210.yoffset,10,k210.ydirection*180);
						k210.yoffset = 0;
						delaycnt = 50;
						delay_flag = 1;
						stage=62;
					}
				break;
				case 62:
					if(k210.land == 1){
						stage = 63;
					}
					else{
						Horizontal_Move(k210.xoffset,10,k210.xdirection*180+90);
						k210.yoffset = 0;
						delaycnt = 50;
						delay_flag = 1;
						stage=61;
					}
				break;
				case 63:
					OneKey_Land();
					hmi.mode = 0;
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
					stage = 21;
					delaycnt = 350;
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
					else if(openmv.ready == 1) stage = 25;
					else{
						Horizontal_Move(50,velocity,270);
						openmv.xtotal += 50;
						delaycnt = 200;
						delay_flag = 1;
					}
				case 23:
					if(openmv.ready == 1) stage = 25;
					else{
						Horizontal_Move(openmv.xoffset,10,openmv.xdirection*180+90);
						if(openmv.xdirection == 1)	openmv.xtotal -= openmv.xoffset;
						else openmv.xtotal += openmv.xoffset;
						openmv.xoffset = 0;
						delaycnt = 60;
						delay_flag = 1;
						stage=24;
					}
				break;
				case 24:
					if(openmv.ready == 1) stage = 25;
					else{
						Horizontal_Move(openmv.yoffset,20,openmv.ydirection*180);
						openmv.ytotal += openmv.yoffset;
						openmv.yoffset = 0;
						delaycnt = 70;
						delay_flag = 1;
						stage=22;
					}
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
					Horizontal_Move(openmv.ytotal - 20,50,180);
					delaycnt = 150;
					delay_flag = 1;
					stage = 27;
				break;
				//ʶ�����ֺ���˸
				case 27:
					blink(7);
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
					blink(7);
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
					delaycnt = 200;
					delay_flag = 1;
					stage = 30;
				break;
				case 30:
					Horizontal_Move(20,20,90);
					delaycnt = 200;
					delay_flag = 1;
					stage = 10;
				break;
				//xУ��
				case 70:
					openmv_cfg.mode = 2;
					dt.fun[0xf6].WTS = 1;
					delaycnt = 70;
					delay_flag = 1;
					stage = 71;
				break;
				case 71:
					//if(openmv.xdistance < xlilun && xlilun - openmv.xdistance > 5) 
						//Horizontal_Move((xlilun-openmv.xdistance)*2,10,90);
					//else 
						if(openmv.xdistance > xlilun && openmv.xdistance - xlilun > 3)
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
					delaycnt = 70;
					delay_flag = 1;
					stage = 73;
				break;
				case 73:
					if(openmv.ydistance < ylilun  && ylilun - openmv.ydistance > 5) 
						Horizontal_Move(ylilun-openmv.ydistance,10,0);
					else if(openmv.ydistance > ylilun  && openmv.ydistance - ylilun > 3)
						Horizontal_Move((openmv.ydistance - ylilun)*3,15,180);
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
					delaycnt = 70;
					delay_flag = 1;
					stage = 75;
				break;
				case 75:
					if(openmv.xdistancel < xlilunl && xlilunl - openmv.xdistancel > 7) 
						Horizontal_Move((xlilunl-openmv.xdistancel)*1,10,90);
					else if(openmv.xdistancel > xlilunl && openmv.xdistancel - xlilunl > 5)
						Horizontal_Move((openmv.xdistancel - xlilunl)*3,15,270);
					openmv.xdistancel = xlilunl;
					delaycnt = 150;
					delay_flag = 1;
					sendflag = 0;
					stage = 7;
				break;
					//y����
				//case 76:
				//	openmv_cfg.mode = 3;
				//	dt.fun[0xf6].WTS = 1;
				//	delaycnt = 70;
				//	delay_flag = 1;
				//	stage = 77;
				//break;
				//case 77:
				//	if(openmv.ydistanceu < ylilunu && ylilunu - openmv.ydistanceu > 3) 
				//		Horizontal_Move((ylilunu-openmv.ydistanceu)*2,10,180);
				//	else if(openmv.ydistanceu > ylilunu && openmv.ydistanceu - ylilunu > 5)
				//		Horizontal_Move(openmv.ydistanceu - ylilunu,10,0);
				//	openmv.ydistance = ylilun;
				//	delaycnt = 130;
				//	delay_flag = 1;
				//	sendflag = 0;
				//	stage = 7;
				//break;
				case 80:
					dt.fun[0xf6].WTS = 1;
					delaycnt = 70;
					delay_flag = 1;
					stage = 81;
				break;
				case 81:
					if(openmv.ydistance < ylilun  && ylilun - openmv.ydistance > 5){ 
						switch(openmv_cfg.mode){
                            case 5:Horizontal_Move(ylilun-openmv.ydistance,10,180);
                            break;
                            case 6:Horizontal_Move(ylilun-openmv.ydistance,10,0);
                            break;
							case 7:Horizontal_Move(ylilun-openmv.ydistance,10,0);
                            break;
                            case 8:Horizontal_Move(ylilun-openmv.ydistance,10,0);
                            break;
                            case 9:Horizontal_Move(ylilun-openmv.ydistance,10,0);
                            break;
                        }
                    }
					else if(openmv.ydistance > ylilun && openmv.ydistance - ylilun > 5){
						switch(openmv_cfg.mode){
                            case 5:Horizontal_Move(ylilun-openmv.ydistance,10,0);
                            break;
              				case 6:Horizontal_Move(ylilun-openmv.ydistance,10,180);
                            break;
							case 7:Horizontal_Move(ylilun-openmv.ydistance,10,180);
                            break;
                            case 8:Horizontal_Move(ylilun-openmv.ydistance,10,180);
                            break;
                            case 9:Horizontal_Move(ylilun-openmv.ydistance,10,180);
                            break;
                        }
                    }
					//openmv.ydistance = ylilun;
					delaycnt = 150;
					delay_flag = 1;
					sendflag = 0;
					stage = 82;
				break;
				case 82:
					if(openmv.xdistance < xlilun && xlilun - openmv.xdistance > 5){
						switch(openmv_cfg.mode){
                            case 5:Horizontal_Move(xlilun-openmv.xdistance,10,270);
                            break;
                            case 6:Horizontal_Move(xlilun-openmv.xdistance,10,90);
                            break;
                            case 7:Horizontal_Move(xlilun-openmv.xdistance,10,270);
                            break;
                            case 8:Horizontal_Move(xlilun-openmv.xdistance,10,270);
                            break;
                            case 9:Horizontal_Move(xlilun-openmv.xdistance,10,90);
                            break;
                        }
                    }
					else if(openmv.xdistance > xlilun && openmv.xdistance - xlilun > 6){
						switch(openmv_cfg.mode){
                            case 5:Horizontal_Move(xlilun-openmv.xdistance,10,90);
                            break;
                            case 6:Horizontal_Move(xlilun-openmv.xdistance,10,270);
                            break;
                            case 7:Horizontal_Move(xlilun-openmv.xdistance,10,90);
                            break;
                            case 8:Horizontal_Move(xlilun-openmv.xdistance,10,90);
                            break;
                            case 9:Horizontal_Move(xlilun-openmv.xdistance,10,270);
                            break;
                        }
                    }
					//openmv.xdistance = xlilun;
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

void spreadPU(u8 height){
		static u16 counter = 0;			//����ʱ�����
		counter ++;
		u16 maxcnt = 18000;				//������ʱ��
		static u8 stage = 0;			//����ִ�н׶�
		static u8 sendflag=0;			//���ͱ�־λ
		static u8 corrflag=0;			//У����־λ
		static u8 step = 1;				//�����ߵ��ڼ�����
		static u8 counterLED = 0;		//LED��˸����
		u8 blockLength = 50;			//���ӳ���
		u8 velocity = 50;				//ǰ���ٶ�
		static u16 direction = 90;		//ǰ������ 
		LX_Change_Mode(3);				//�л����̿�ģʽ
		static u8 xlilun = 38;
		static u8 ylilun = 30;
		static u8 xlilunl = 90;
		
		if(hmi.mode != hmi.oldmode){
			counter = 0;
			stage = 0;
			sendflag = 0;
			corrflag=0;
			step = 1;
			counterLED = 0;
			hmi.oldmode = hmi.mode;
		}
		switch(step){
			case 4:direction = 180;
					if(corrflag == 0){
						stage = 80;
						openmv_cfg.mode = 5;
						xlilun = 82;
						ylilun = 107;
						corrflag = 1;
					}
				break;
			case 6:
					//if(corrflag == 0){
					//	stage = 74;
					//	xlilunl = 85;
					//	corrflag = 1;
					//}
				break;
			case 8:direction = 90;
					if(corrflag == 0){
						xlilun = 84;
						ylilun = 78;
						stage = 80;
						openmv_cfg.mode = 6;
						corrflag = 1;
					}
				break;
			case 9:direction = 0;
				break;
			case 11:
					if(corrflag == 0){
						stage = 72;
						ylilun = 32;
						corrflag = 1;
					}
				break;
			case 13:direction = 90;
				break;
			case 14:direction = 180;
				break;
			case 18:direction = 90;
				break;
			case 19:direction = 0;
				if(corrflag == 0){
					stage = 80;
					openmv_cfg.mode = 7;
					xlilun = 125;
					ylilun = 70;
					corrflag = 1;
				}
				break;
			case 21:
				if(corrflag == 0){
					stage = 72;
					ylilun = 32;//x=57
					corrflag = 1;
				}		
				break;
			case 24:direction = 270;
				if(corrflag == 0){
					stage = 80;
					openmv_cfg.mode = 8;
					xlilun = 130;
					ylilun = 12;
					corrflag = 1;
				}
				break;
			case 30:direction = 180;
				if(corrflag == 0){
					stage = 80;
					xlilun = 106;
					ylilun = 10;
					openmv_cfg.mode = 9;
					corrflag = 1;
				}
				break;
			case 31:
				stage = 50;
				step++;
				break;
			default:
				break;
		}
		
		//����ƽ�Ʒ���ȷ������
		switch(direction){
			case 90:
				blockLength = 46;
				velocity = 46;
			break;
			case 270:
				blockLength = 50;
				velocity = 50;
			break;
			case 0:
				blockLength = 46;
				velocity = 46;
			break;
			case 180:
				blockLength = 48;
				velocity = 48;
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
					Horizontal_Move(4*43,velocity,0);
					stage = 3;
					delaycnt = 250;
					delay_flag = 1;
				break;
				case 3:
					Horizontal_Move(25,velocity,90); 
					delaycnt = 100;
					delay_flag = 1;
					stage=4;
				break;	
				//K210������ĸʶ�𣬸���Aλ�õ���
				case 4:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 150;
					delay_flag = 1;
					stage=5;
				break;
				case 5:
					Horizontal_Move(k210.yoffset,20,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=6;
				break;
				case 6:
					Horizontal_Move(k210.xoffset,20,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=104;
				break;
				//K210������ĸʶ�𣬸���Aλ�õ���
				case 104:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 150;
					delay_flag = 1;
					stage=105;
				break;
				case 105:
					Horizontal_Move(k210.yoffset,20,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=106;
				break;
				case 106:
					Horizontal_Move(k210.xoffset,20,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 50;
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
					else if(map[k210_cfg.map][step]== 1){
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
					//if(step == 8) Horizontal_Move(37,37,90);
					if(step == 13) Horizontal_Move(43,43,90);
					//else if(step == 18) Horizontal_Move(37,37,90);
					//else if(step == 29) Horizontal_Move(40,40,0);
					else Horizontal_Move(blockLength,velocity,direction);
					step = step + 1;
					delaycnt = blockLength * 50 / velocity + 50;
					delay_flag = 1;
					corrflag = 0;
					stage=7;
				break;
				//�ߵ�����
				//case 10:
				//	k210_cfg.mode=2;	
				//	dt.fun[0xf4].WTS=1; 
				//	delaycnt = 150;
				//	delay_flag = 1;
				//	stage=11;
				//break;
				//case 11:
				//	Horizontal_Move(k210.yoffset,20,k210.ydirection*180);
				//	k210.yoffset = 0;
				//	delaycnt = 70;
				//	delay_flag = 1;
				//	stage=12;
				//break;
				//case 12:
				//	Horizontal_Move(k210.xoffset,20,k210.xdirection*180+90);
				//	k210.xoffset = 0;
				//	delaycnt = 70;
				//	delay_flag = 1;
				//	stage=13;
				//break;
				//ԭ·����
				case 13:
					Horizontal_Move(40,velocity,90);
					delaycnt = 150;
					delay_flag = 1;
					stage=14;
				break;
				case 14:
					Horizontal_Move(190,velocity,0);
					delaycnt = 260;
					delay_flag = 1;
					stage=114;
				break;
				case 114:
					k210_cfg.mode=6;
					dt.fun[0xf4].WTS=1;
					delaycnt = 150;
					delay_flag = 1;
					stage = 15;
				break;
				//����ʮ��λ���жϷɻ�λ�ã�K210���Ʒɻ����������
				case 15:
					if(k210.land == 1){
						Vertical_Down(55,40);
						targetHeight = 90;
						stage = 60;
						delaycnt = 200;
						delay_flag = 1;
					}
					else if(k210.xoffset ==0 && k210.yoffset ==0){
						Horizontal_Move(20,20,270);
						delaycnt = 60;
						delay_flag = 1;
					}
					else{
						Horizontal_Move(k210.xoffset,20,k210.xdirection*180+90);
						k210.xoffset = 0;
						delaycnt = 50;
						delay_flag = 1;
						stage=16;
					}
				break;
				case 16:
					if(k210.land == 1){
						Vertical_Down(55,40);
						targetHeight = 90;
						stage = 60;
						delaycnt = 200;
						delay_flag = 1;
					}
					else{
						Horizontal_Move(k210.yoffset,20,k210.ydirection*180);
						k210.yoffset = 0;
						delaycnt = 50;
						delay_flag = 1;
						stage=15;
					}
				break;
				case 60:
					k210.land = 0;
					k210_cfg.mode=6;
					dt.fun[0xf4].WTS=1;
					delaycnt = 150;
					delay_flag = 1;
					stage = 61;
				break;
				case 61:
					if(k210.land == 1){
						stage = 63;
					}
					else{
						Horizontal_Move(k210.yoffset,10,k210.ydirection*180);
						k210.yoffset = 0;
						delaycnt = 50;
						delay_flag = 1;
						stage=62;
					}
				break;
				case 62:
					if(k210.land == 1){
						stage = 63;
					}
					else{
						Horizontal_Move(k210.xoffset,10,k210.xdirection*180+90);
						k210.yoffset = 0;
						delaycnt = 50;
						delay_flag = 1;
						stage=61;
					}
				break;
				case 63:
					Horizontal_Move(k210.number*8,10,180);
					delaycnt = 210;
					delay_flag = 1;
					stage = 64;
				break;
				case 64:OneKey_Land();
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
					stage = 21;
					delaycnt = 350;
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
					else if(openmv.ready == 1) stage = 25;
					else{
						Horizontal_Move(50,velocity,270);
						openmv.xtotal += 50;
						delaycnt = 200;
						delay_flag = 1;
					}
				case 23:
					if(openmv.ready == 1) stage = 25;
					else{
						Horizontal_Move(openmv.xoffset,20,openmv.xdirection*180+90);
						if(openmv.xdirection == 1)	openmv.xtotal -= openmv.xoffset;
						else openmv.xtotal += openmv.xoffset;
						openmv.xoffset = 0;
						delaycnt = 60;
						delay_flag = 1;
						stage=24;
					}
				break;
				case 24:
					if(openmv.ready == 1) stage = 25;
					else{
						Horizontal_Move(openmv.yoffset,20,openmv.ydirection*180);
						openmv.ytotal += openmv.yoffset;
						openmv.yoffset = 0;
						delaycnt = 70;
						delay_flag = 1;
						stage=22;
					}
				break;
				//��ʼʶ������
				case 25:
					openmv_cfg.mode=0;
					dt.fun[0xf6].WTS=1;
					//�߶ȵ���
					targetHeight = 110;
					Vertical_Down(35,10);
					k210_cfg.mode=4;
					dt.fun[0xf4].WTS=1;
					delaycnt = 250;
					delay_flag = 1;
					stage = 26;
				break;
				case 26:
					Horizontal_Move(openmv.ytotal+55,50,180);
					delaycnt = 150;
					delay_flag = 1;
					stage = 27;
				break;
				//ʶ�����ֺ���˸
				case 27:
					blink(7);
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
					blink(7);
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
					delaycnt = 200;
					delay_flag = 1;
					stage = 30;
				break;
				case 30:
					Horizontal_Move(50,50,90);
					delaycnt = 200;
					delay_flag = 1;
					stage = 80;
				break;
				//xУ��
				case 70:
					openmv_cfg.mode = 2;
					dt.fun[0xf6].WTS = 1;
					delaycnt = 70;
					delay_flag = 1;
					stage = 71;
				break;
				case 71:
					//if(openmv.xdistance < xlilun && xlilun - openmv.xdistance > 5) 
						//Horizontal_Move((xlilun-openmv.xdistance)*2,10,90);
					//else 
						if(openmv.xdistance > xlilun && openmv.xdistance - xlilun > 3)
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
					delaycnt = 70;
					delay_flag = 1;
					stage = 73;
				break;
				case 73:
					if(openmv.ydistance < ylilun  && ylilun - openmv.ydistance > 5) 
						Horizontal_Move(ylilun-openmv.ydistance,10,0);
					else if(openmv.ydistance > ylilun  && openmv.ydistance - ylilun > 3)
						Horizontal_Move((openmv.ydistance - ylilun)*3,15,180);
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
					delaycnt = 70;
					delay_flag = 1;
					stage = 75;
				break;
				case 75:
					if(openmv.xdistancel < xlilunl && xlilunl - openmv.xdistancel > 7) 
						Horizontal_Move((xlilunl-openmv.xdistancel)*1,10,90);
					else if(openmv.xdistancel > xlilunl && openmv.xdistancel - xlilunl > 5)
						Horizontal_Move((openmv.xdistancel - xlilunl)*3,15,270);
					openmv.xdistancel = xlilunl;
					delaycnt = 150;
					delay_flag = 1;
					sendflag = 0;
					stage = 7;
				break;
					//y����
				//case 76:
				//	openmv_cfg.mode = 3;
				//	dt.fun[0xf6].WTS = 1;
				//	delaycnt = 70;
				//	delay_flag = 1;
				//	stage = 77;
				//break;
				//case 77:
				//	if(openmv.ydistanceu < ylilunu && ylilunu - openmv.ydistanceu > 3) 
				//		Horizontal_Move((ylilunu-openmv.ydistanceu)*2,10,180);
				//	else if(openmv.ydistanceu > ylilunu && openmv.ydistanceu - ylilunu > 5)
				//		Horizontal_Move(openmv.ydistanceu - ylilunu,10,0);
				//	openmv.ydistance = ylilun;
				//	delaycnt = 130;
				//	delay_flag = 1;
				//	sendflag = 0;
				//	stage = 7;
				//break;
				case 80:
					dt.fun[0xf6].WTS = 1;
					delaycnt = 70;
					delay_flag = 1;
					stage = 81;
				break;
				case 81:
					if(openmv.ydistance < ylilun  && ylilun - openmv.ydistance > 5){ 
						switch(openmv_cfg.mode){
                            case 5:Horizontal_Move(ylilun-openmv.ydistance,10,180);
                            break;
                            case 6:Horizontal_Move(ylilun-openmv.ydistance,10,0);
                            break;
							case 7:Horizontal_Move(ylilun-openmv.ydistance,10,0);
                            break;
                            case 8:Horizontal_Move(ylilun-openmv.ydistance,10,0);
                            break;
                            case 9:Horizontal_Move(ylilun-openmv.ydistance,10,0);
                            break;
                        }
                    }
					else if(openmv.ydistance > ylilun && openmv.ydistance - ylilun > 5){
						switch(openmv_cfg.mode){
                            case 5:Horizontal_Move(ylilun-openmv.ydistance,10,0);
                            break;
              				case 6:Horizontal_Move(ylilun-openmv.ydistance,10,180);
                            break;
							case 7:Horizontal_Move(ylilun-openmv.ydistance,10,180);
                            break;
                            case 8:Horizontal_Move(ylilun-openmv.ydistance,10,180);
                            break;
                            case 9:Horizontal_Move(ylilun-openmv.ydistance,10,180);
                            break;
                        }
                    }
					//openmv.ydistance = ylilun;
					delaycnt = 150;
					delay_flag = 1;
					sendflag = 0;
					stage = 82;
				break;
				case 82:
					if(openmv.xdistance < xlilun && xlilun - openmv.xdistance > 5){
						switch(openmv_cfg.mode){
                            case 5:Horizontal_Move(xlilun-openmv.xdistance,10,270);
                            break;
                            case 6:Horizontal_Move(xlilun-openmv.xdistance,10,90);
                            break;
                            case 7:Horizontal_Move(xlilun-openmv.xdistance,10,270);
                            break;
                            case 8:Horizontal_Move(xlilun-openmv.xdistance,10,270);
                            break;
                            case 9:Horizontal_Move(xlilun-openmv.xdistance,10,90);
                            break;
                        }
                    }
					else if(openmv.xdistance > xlilun && openmv.xdistance - xlilun > 6){
						switch(openmv_cfg.mode){
                            case 5:Horizontal_Move(xlilun-openmv.xdistance,10,90);
                            break;
                            case 6:Horizontal_Move(xlilun-openmv.xdistance,10,270);
                            break;
                            case 7:Horizontal_Move(xlilun-openmv.xdistance,10,90);
                            break;
                            case 8:Horizontal_Move(xlilun-openmv.xdistance,10,90);
                            break;
                            case 9:Horizontal_Move(xlilun-openmv.xdistance,10,270);
                            break;
                        }
                    }
					//openmv.xdistance = xlilun;
					delaycnt = 150;
					delay_flag = 1;
					sendflag = 0;
					if(step>=32) stage = 13;
					else stage = 7;
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

void drawNum(u8 height){
		static u16 counter = 0;			//����ʱ�����
		counter ++;
		u16 maxcnt = 18000;				//������ʱ��
		static u8 stage = 0;			//����ִ�н׶�
		static u8 counterLED = 0;		//LED��˸����
		u8 velocity = 50;				//ǰ���ٶ�
		LX_Change_Mode(3);				//�л����̿�ģʽ
		
		if(hmi.mode != hmi.oldmode){
			counter = 0;
			stage = 0;
			counterLED = 0;
			hmi.oldmode = hmi.mode;
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
					k210.number = 3;
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
					Horizontal_Move(30,velocity,90); 
					delaycnt = 100;
					delay_flag = 1;
					stage=4;
				break;	
				//K210������ĸʶ�𣬸���Aλ�õ���
				case 4:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 150;
					delay_flag = 1;
					stage=5;
				break;
				case 5:
					Horizontal_Move(k210.yoffset,20,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=6;
				break;
				case 6:
					Horizontal_Move(k210.xoffset,20,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=104;
				break;
				//K210������ĸʶ�𣬸���Aλ�õ���
				case 104:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 150;
					delay_flag = 1;
					stage=105;
				break;
				case 105:
					Horizontal_Move(k210.yoffset,20,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=106;
				break;
				case 106:
					Horizontal_Move(k210.xoffset,20,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=20;
				break;
				//Ѱ�Ҹ��ӺͶ�ά��λ��
				case 20:
					Right_Rotate(177,90);
					stage = 21;
					delaycnt = 350;
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
					else if(openmv.ready == 1) stage = 25;
					else{
						Horizontal_Move(50,velocity,270);
						openmv.xtotal += 50;
						delaycnt = 200;
						delay_flag = 1;
					}
				case 23:
					if(openmv.ready == 1) stage = 25;
					else{
						Horizontal_Move(openmv.xoffset,20,openmv.xdirection*180+90);
						if(openmv.xdirection == 1)	openmv.xtotal -= openmv.xoffset;
						else openmv.xtotal += openmv.xoffset;
						openmv.xoffset = 0;
						delaycnt = 60;
						delay_flag = 1;
						stage=24;
					}
				break;
				case 24:
					if(openmv.ready == 1) stage = 25;
					else{
						Horizontal_Move(openmv.yoffset,20,openmv.ydirection*180);
						openmv.ytotal += openmv.yoffset;
						openmv.yoffset = 0;
						delaycnt = 70;
						delay_flag = 1;
						stage=22;
					}
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
					Horizontal_Move(openmv.ytotal-30,50,180);
					delaycnt = 150;
					delay_flag = 1;
					stage = 27;
				break;
				//ʶ�����ֺ���˸
				case 27:
					blink(7);
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
					blink(7);
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
					delaycnt = 200;
					delay_flag = 1;
					stage = 30;
				break;
				case 30:
					Horizontal_Move(80,80,270);
					delaycnt = 120;
					delay_flag = 1;				
					if(k210.number == 3){
						stage =31;
						DrvLedOnOff(1);
					}
					else if(k210.number == 4){
						stage = 41;
						DrvLedOnOff(2);
					}
					else if(k210.number == 5){
						stage = 51;
						DrvLedOnOff(3);
					}
					if(k210.number == 6){
						stage = 61;
						DrvLedOnOff(4);
					}
					else OneKey_Land();
				break;
				case 31:
					Horizontal_Move(50,50,270);
					delaycnt = 120;
					delay_flag = 1;
					stage = 32;
				break;
				case 32:
					targetHeight -= 35;
					Vertical_Down(35,35);
					delaycnt = 120;
					delay_flag = 1;
					stage = 33;
				break;
				case 33:
					Horizontal_Move(50,50,90);
					delaycnt = 120;
					delay_flag = 1;
					stage = 34;
				break;
				case 34:
					Horizontal_Move(50,50,270);
					delaycnt = 120;
					delay_flag = 1;
					stage = 35;
				break;
				case 35:
					targetHeight -= 35;
					Vertical_Down(35,35);
					delaycnt = 120;
					delay_flag = 1;
					stage = 36;
				break;
				case 36:
					Horizontal_Move(50,50,90);
					delaycnt = 150;
					delay_flag = 1;
					stage = 70;
				break;
				case 41:
					targetHeight -= 35;
					Vertical_Down(35,35);
					delaycnt = 140;
					delay_flag = 1;
					stage = 42;
				break;
				case 42:
					Horizontal_Move(50,50,270);
					delaycnt = 120;
					delay_flag = 1;
					stage = 43;
				break;
				case 43:
					targetHeight += 35;
					Vertical_Up(35,35);
					delaycnt = 160;
					delay_flag = 1;
					stage = 44;
				break;
				case 44:
					targetHeight -= 35;
					Vertical_Down(35,35);
					delaycnt = 140;
					delay_flag = 1;
					stage = 45;
				break;
				case 45:
					targetHeight -= 35;
					Vertical_Down(35,35);
					delaycnt = 140;
					delay_flag = 1;
					stage = 70;
				break;
				case 51:
					Horizontal_Move(50,50,90);
					delaycnt = 120;
					delay_flag = 1;
					stage = 52;
				break;
				case 52:
					targetHeight -= 35;
					Vertical_Down(35,35);
					delaycnt = 120;
					delay_flag = 1;
					stage = 53;
				break;
				case 53:
					Horizontal_Move(50,50,270);
					delaycnt = 120;
					delay_flag = 1;
					stage = 54;
				break;
				case 54:
					targetHeight -= 35;
					Vertical_Down(35,35);
					delaycnt = 120;
					delay_flag = 1;
					stage = 55;
				break;
				case 55:
					Horizontal_Move(50,50,90);
					delaycnt = 120;
					delay_flag = 1;
					stage = 70;
				break;
				case 61:
					Horizontal_Move(25,25,90);
					delaycnt = 120;
					delay_flag = 1;
					stage = 62;
				break;
				case 62:
					targetHeight -= 35;
					Vertical_Down(35,35);
					delaycnt = 120;
					delay_flag = 1;
					stage = 63;
				break;
				case 63:
					targetHeight -= 35;
					Vertical_Down(35,35);
					delaycnt = 120;
					delay_flag = 1;
					stage = 64;
				break;
				case 64:
					Horizontal_Move(50,50,270);
					delaycnt = 120;
					delay_flag = 1;
					stage = 65;
				break;
				case 65:
					targetHeight += 35;
					Vertical_Down(35,35);
					delaycnt = 120;
					delay_flag = 1;
					stage = 66;
				break;
				case 66:
					Horizontal_Move(25,25,90);
					delaycnt = 120;
					delay_flag = 1;
					stage = 70;
				break;
				case 70:
					OneKey_Land();
					DrvLedOnOff(0);
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
		u16 maxcnt = 18000;				//������ʱ��
		static u8 stage = 0;			//����ִ�н׶�
		static u8 sendflag=0;			//���ͱ�־λ
		static u8 corrflag=0;			//У����־λ
		static u8 step = 1;				//�����ߵ��ڼ�����
		static u8 counterLED = 0;		//LED��˸����
		u8 blockLength = 50;			//���ӳ���
		u8 velocity = 50;				//ǰ���ٶ�
		static u16 direction = 90;		//ǰ������ 
		LX_Change_Mode(3);				//�л����̿�ģʽ
		static u8 xlilun = 38;
		static u8 ylilun = 30;
		static u8 xlilunl = 90;
		static u8 ylilunu = 38;
		/*static u8 xlilun = 38;
		static u8 ylilun = 30;
		static u8 xlilunl = 90;
		static u8 ylilunu = 38;*/
		
		if(hmi.mode != hmi.oldmode){
			counter = 0;
			stage = 0;
			sendflag = 0;
			corrflag=0;
			step = 1;
			counterLED = 0;
			hmi.oldmode = hmi.mode;
		}
		switch(step){
			case 4:direction = 180;
					if(corrflag == 0){
						stage = 80;
						openmv_cfg.mode = 5;
						xlilun = 75;
						ylilun = 98;
						corrflag = 1;
					}
				break;
			case 6:
					if(corrflag == 0){
						stage = 74;
						xlilunl = 85;
						corrflag = 1;
					}
				break;
			case 8:direction = 90;
					if(corrflag == 0){
						xlilun = 90;
						ylilun = 70;
						stage = 80;
						openmv_cfg.mode = 6;
						corrflag = 1;
					}
				break;
			case 9:direction = 0;
				break;
			case 11:
					if(corrflag == 0){
						stage = 72;
						ylilun = 32;
						corrflag = 1;
					}
				break;
			case 13:direction = 90;
				break;
			case 14:direction = 180;
				break;
			case 18:direction = 90;
				break;
			case 19:direction = 0;
				if(corrflag == 0){
					stage = 80;
					openmv_cfg.mode = 7;
					xlilun = 130;
					ylilun = 62;
					corrflag = 1;
				}
				break;
			case 21:
				if(corrflag == 0){
					stage = 72;
					ylilun = 32;//x=57
					corrflag = 1;
				}		
				break;
			case 24:direction = 270;
				if(corrflag == 0){
					stage = 80;
					openmv_cfg.mode = 8;
					xlilun = 113;
					ylilun = 19;
					corrflag = 1;
				}
				break;
			case 30:direction = 180;
				if(corrflag == 0){
					stage = 80;
					xlilun = 95;
					ylilun = 17;
					openmv_cfg.mode = 9;
					corrflag = 1;
				}
				break;
			case 31:
				stage = 50;
				step++;
				break;
			default:
					break;
		}
		
		//����ƽ�Ʒ���ȷ������
		switch(direction){
			case 90:
				blockLength = 46;
				velocity = 46;
			break;
			case 270:
				blockLength = 50;
				velocity = 50;
			break;
			case 0:
				blockLength = 46;
				velocity = 46;
			break;
			case 180:
				blockLength = 48;
				velocity = 48;
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
					delaycnt = 150;
					delay_flag = 1;
					stage=5;
				break;
				case 5:
					Horizontal_Move(k210.yoffset,20,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=6;
				break;
				case 6:
					Horizontal_Move(k210.xoffset,20,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=104;
				break;
				//K210������ĸʶ�𣬸���Aλ�õ���
				case 104:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 150;
					delay_flag = 1;
					stage=105;
				break;
				case 105:
					Horizontal_Move(k210.yoffset,20,k210.ydirection*180);
					k210.yoffset = 0;
					delaycnt = 50;
					delay_flag = 1;
					stage=106;
				break;
				case 106:
					Horizontal_Move(k210.xoffset,20,k210.xdirection*180+90);
					k210.xoffset = 0;
					delaycnt = 50;
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
					//map
					else if(map[k210_cfg.map][step]== 1){
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
					//if(step == 8) Horizontal_Move(37,37,90);
					if(step == 13) Horizontal_Move(43,43,90);
					//else if(step == 18) Horizontal_Move(37,37,90);
					//else if(step == 29) Horizontal_Move(40,40,0);
					else Horizontal_Move(blockLength,velocity,direction);
					step = step + 1;
					delaycnt = blockLength * 50 / velocity + 50;
					delay_flag = 1;
					corrflag = 0;
					stage=7;
				break;
				//�ߵ�����
				case 10:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 150;
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
					stage=61;
				break;
				case 61:
					OneKey_Land();
				break;
				//Ѱ�Ҹ��ӺͶ�ά��λ��
				case 50:
					k210_cfg.mode=2;	
					dt.fun[0xf4].WTS=1; 
					delaycnt = 150;
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
					stage = 21;
					delaycnt = 200;
					delay_flag = 1;
				break;
				case 21:
					openmv_cfg.mode = 1;
					dt.fun[0xf6].WTS= 1;
					delaycnt = 120;
					delay_flag = 1;
					stage = 22;
					break;
				case 22:
					if(openmv.xoffset !=0 | openmv.yoffset != 0) stage = 23;
					else if(openmv.ready == 1) stage = 25;
					else{
						Horizontal_Move(50,velocity,270);
						openmv.xtotal += 50;
						delaycnt = 200;
						delay_flag = 1;
					}
				case 23:
					if(openmv.ready == 1) stage = 25;
					else{
						Horizontal_Move(openmv.xoffset,20,openmv.xdirection*180+90);
						if(openmv.xdirection == 1)	openmv.xtotal -= openmv.xoffset;
						else openmv.xtotal += openmv.xoffset;
						openmv.xoffset = 0;
						delaycnt = 60;
						delay_flag = 1;
						stage=24;
					}
				break;
				case 24:
					if(openmv.ready == 1) stage = 25;
					else{
						Horizontal_Move(openmv.yoffset,20,openmv.ydirection*180);
						openmv.ytotal += openmv.yoffset;
						openmv.yoffset = 0;
						delaycnt = 70;
						delay_flag = 1;
						stage=22;
					}
				break;
				//��ʼʶ������
				case 25:
					openmv_cfg.mode=0;
					dt.fun[0xf6].WTS=1;
					//�߶ȵ���
					targetHeight = 110;
					Vertical_Down(40,10);
					k210_cfg.mode=4;
					dt.fun[0xf4].WTS=1;
					delaycnt = 250;
					delay_flag = 1;
					stage = 26;
				break;
				case 26:
					Horizontal_Move(openmv.ytotal,50,180);
					delaycnt = 150;
					delay_flag = 1;
					stage = 27;
				break;
				//ʶ�����ֺ���˸
				case 27:
					blink(7);
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
					blink(7);
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
					delaycnt = 200;
					delay_flag = 1;
					stage = 30;
				break;
				case 30:
					Horizontal_Move(25,25,90);
					delaycnt = 80;
					delay_flag = 1;
					stage = 10;
				break;
				//xУ��
				case 70:
					openmv_cfg.mode = 2;
					dt.fun[0xf6].WTS = 1;
					delaycnt = 70;
					delay_flag = 1;
					stage = 71;
				break;
				case 71:
					//if(openmv.xdistance < xlilun && xlilun - openmv.xdistance > 5) 
						//Horizontal_Move((xlilun-openmv.xdistance)*2,10,90);
					//else 
						if(openmv.xdistance > xlilun && openmv.xdistance - xlilun > 3)
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
					delaycnt = 70;
					delay_flag = 1;
					stage = 73;
				break;
				case 73:
					if(openmv.ydistance < ylilun  && ylilun - openmv.ydistance > 5) 
						Horizontal_Move(ylilun-openmv.ydistance,10,0);
					else if(openmv.ydistance > ylilun  && openmv.ydistance - ylilun > 3)
						Horizontal_Move((openmv.ydistance - ylilun)*3,15,180);
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
					delaycnt = 70;
					delay_flag = 1;
					stage = 75;
				break;
				case 75:
					if(openmv.xdistancel < xlilunl && xlilunl - openmv.xdistancel > 7) 
						Horizontal_Move((xlilunl-openmv.xdistancel)*1,10,90);
					else if(openmv.xdistancel > xlilunl && openmv.xdistancel - xlilunl > 5)
						Horizontal_Move((openmv.xdistancel - xlilunl)*3,15,270);
					openmv.xdistancel = xlilunl;
					delaycnt = 150;
					delay_flag = 1;
					sendflag = 0;
					stage = 7;
				break;
					//y����
				//case 76:
				//	openmv_cfg.mode = 3;
				//	dt.fun[0xf6].WTS = 1;
				//	delaycnt = 70;
				//	delay_flag = 1;
				//	stage = 77;
				//break;
				//case 77:
				//	if(openmv.ydistanceu < ylilunu && ylilunu - openmv.ydistanceu > 3) 
				//		Horizontal_Move((ylilunu-openmv.ydistanceu)*2,10,180);
				//	else if(openmv.ydistanceu > ylilunu && openmv.ydistanceu - ylilunu > 5)
				//		Horizontal_Move(openmv.ydistanceu - ylilunu,10,0);
				//	openmv.ydistance = ylilun;
				//	delaycnt = 130;
				//	delay_flag = 1;
				//	sendflag = 0;
				//	stage = 7;
				//break;
				case 80:
					dt.fun[0xf6].WTS = 1;
					delaycnt = 70;
					delay_flag = 1;
					stage = 81;
				break;
				case 81:
					if(openmv.ydistance < ylilun  && ylilun - openmv.ydistance > 4){ 
						switch(openmv_cfg.mode){
                            case 5:Horizontal_Move(ylilun-openmv.ydistance,10,180);
                            break;
                            case 6:Horizontal_Move(ylilun-openmv.ydistance,10,0);
                            break;
							case 7:Horizontal_Move(ylilun-openmv.ydistance,10,0);
                            break;
                            case 8:Horizontal_Move(ylilun-openmv.ydistance,10,0);
                            break;
                            case 9:Horizontal_Move(ylilun-openmv.ydistance,10,0);
                            break;
                        }
                    }
					else if(openmv.ydistance > ylilun && openmv.ydistance - ylilun > 4){
						switch(openmv_cfg.mode){
                            case 5:Horizontal_Move(ylilun-openmv.ydistance,10,0);
                            break;
              				case 6:Horizontal_Move(ylilun-openmv.ydistance,10,180);
                            break;
							case 7:Horizontal_Move(ylilun-openmv.ydistance,10,180);
                            break;
                            case 8:Horizontal_Move(ylilun-openmv.ydistance,10,180);
                            break;
                            case 9:Horizontal_Move(ylilun-openmv.ydistance,10,180);
                            break;
                        }
                    }
					//openmv.ydistance = ylilun;
					delaycnt = 130;
					delay_flag = 1;
					sendflag = 0;
					stage = 82;
				break;
				case 82:
					if(openmv.xdistance < xlilun && xlilun - openmv.xdistance > 4){
						switch(openmv_cfg.mode){
                            case 5:Horizontal_Move(xlilun-openmv.xdistance,10,270);
                            break;
                            case 6:Horizontal_Move(xlilun-openmv.xdistance,10,90);
                            break;
                            case 7:Horizontal_Move(xlilun-openmv.xdistance,10,270);
                            break;
                            case 8:Horizontal_Move(xlilun-openmv.xdistance,10,270);
                            break;
                            case 9:Horizontal_Move(xlilun-openmv.xdistance,10,90);
                            break;
                        }
                    }
					else if(openmv.xdistance > xlilun && openmv.xdistance - xlilun > 4){
						switch(openmv_cfg.mode){
                            case 5:Horizontal_Move(xlilun-openmv.xdistance,10,90);
                            break;
                            case 6:Horizontal_Move(xlilun-openmv.xdistance,10,270);
                            break;
                            case 7:Horizontal_Move(xlilun-openmv.xdistance,10,90);
                            break;
                            case 8:Horizontal_Move(xlilun-openmv.xdistance,10,90);
                            break;
                            case 9:Horizontal_Move(xlilun-openmv.xdistance,10,270);
                            break;
                        }
                    }
					//openmv.xdistance = xlilun;
					delaycnt = 130;
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
		blink(7);
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
		blink(7);
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
void blink(u8 color){
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


