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

void solveMaze(u8 height){
		static u16 counter = 0;
		counter ++;
		u16 maxcnt = 6000;        //120?????? ��???????
		static u8 count1 = 0;
		static u8 stage = 0;			//?��???????��??
		u8 blockLength = 40;			//??��?��??��
		u8 velocity = 20;	        //???????? ��???10cm/s
		u8 targetHeight = height;
		LX_Change_Mode(3);
		
		//????��????��????????��???
		if(hmi.mode != hmi.oldmode){
			counter = 0;
			stage = 0;
			count1 = 0;	
			hmi.oldmode = hmi.mode;
		}
		//��?����?????? ????20ms
		if(delay_flag){
			delay20();
		}
		else{
			if(stage == 0){
				FC_Unlock();
				k210_cfg.mode = 1;
				stage = 1;
				//??��??��????4??
				delaycnt = 200;
				delay_flag = 1;
			}
			else if(stage == 1){
				OneKey_Takeoff(targetHeight);
				stage = 2;
				delaycnt = 150;
				delay_flag = 1;
			}
			else if(stage == 2){
				Horizontal_Move(3.5*blockLength,velocity,0);
				stage = 3;
				delaycnt = 350;
				delay_flag = 1;
			}
			else if(stage == 3){
				Horizontal_Move(2.5*blockLength,velocity,90);
				k210_cfg.mode = 1;
				stage = 4;
				delaycnt = 250;
				delay_flag = 1;
			}
			else if(stage == 4){	
				//????
				dt.fun[0xf4].WTS = 1;
				if(k210_cfg.mode == 0) stage = 5;
				delaycnt = 50;
				delay_flag = 1;
			}
			else if(stage==5){	
				Horizontal_Move(2.5*blockLength,velocity,270);
				stage = 6;
				delaycnt = 250;
				delay_flag = 1;
			}
			else if(stage==6){	
				Horizontal_Move(3.5*blockLength,velocity,180);
				k210_cfg.mode = 2;
				stage = 7;
				delaycnt = 350;
				delay_flag = 1;
			}
			else if(stage==7){	
				dt.fun[0xf4].WTS = 1;
				stage = 8;
				maxcnt = 30;
				delay_flag = 1;
			}
			else if(stage==8){	
				Horizontal_Move(blockLength * k210.xoffset,velocity,k210.xdirection*180 + 90);
				maxcnt = k210.xoffset * 100;
				delay_flag = 1;
				k210.xoffset = 0;
				k210.xdirection = 0;
				stage = 9;				
			}
			else if(stage==9){
				Horizontal_Move(blockLength * k210.yoffset,velocity,k210.ydirection*180);
				maxcnt = k210.yoffset * 100;
				delay_flag = 1;
				k210.yoffset = 0;
				k210.ydirection = 0;
				if(k210_cfg.mode == 0) stage = 10;
				else stage = 7;
			}
			else if(stage == 10){
				OneKey_Land();
				counter = 0;
				stage = 0;
				hmi.mode = 0;
			}
			 
		//???��????
		if(counter >= maxcnt){
			OneKey_Land();
			counter = 0;
			stage = 0;
			hmi.mode = 0;
		}
	}
}

void goMaze(u8 height){
		static u16 counter = 0;
		counter ++;
		u16 maxcnt = 3000;        //120?????? ��???????
		static u8 stage = 0;			//?��???????��??
		u8 blockLength = 38;			//??��?��??��
		u8 velocity = 15;	        //???????? ��???10cm/s
		u8 targetHeight = height;
		LX_Change_Mode(3);
		//����???????��??
		k210_cfg.mode = stage;
		
		//????��????��????????��???
		if(hmi.mode != hmi.oldmode){
			counter = 0;
			stage = 0;
			hmi.oldmode = hmi.mode;
		}
		//��?����?????? ????20ms
		if(delay_flag){
			delay20();
		}
		else{
			if(stage==0){
				FC_Unlock();
				stage = 1;
				//??��??��????4??
				delaycnt = 200;
				delay_flag = 1;
			}
			else if(stage==1){
				OneKey_Takeoff(targetHeight);
				stage = 2;
				delaycnt = 150;
				delay_flag = 1;
			}
			else if(stage==2){
				Horizontal_Move(3.5*blockLength,velocity,0);
				stage = 3;
				delaycnt = 350;
				delay_flag = 1;
			}
			else if(stage == 3){
				Horizontal_Move(2.5*blockLength,velocity,90);
				k210_cfg.mode = 1;
				stage = 4;
				delaycnt = 250;
				delay_flag = 1;
			}
			else if(stage == 4){	
				stage = 5;
				delaycnt = 50;
				delay_flag = 1;
			}
			else if(stage==5){	
				Horizontal_Move(2.5*blockLength,velocity,270);
				stage = 6;
				delaycnt = 250;
				delay_flag = 1;
			}
			else if(stage==6){	
				Horizontal_Move(3.5*blockLength,velocity,180);
				k210_cfg.mode = 1;
				stage = 7;
			}
			else if(stage==7){
				Horizontal_Move(blockLength * 2,velocity,0);
				delaycnt = 100;
				delay_flag = 1;
				stage = 8;
			}
			else if(stage==8){
				Horizontal_Move(blockLength * 1,velocity,90);
				delaycnt = 100;
				delay_flag = 1;
				stage = 9;
			}
			else if(stage==9){
				Horizontal_Move(blockLength * 1,velocity,0);
				delaycnt = 100;
				delay_flag = 1;
				stage = 10;
			}
			else if(stage == 10){
				Horizontal_Move(blockLength * 3,velocity,90);
				delaycnt = 100;
				delay_flag = 1;
				stage = 12;
			}
			else if(stage == 12){
				Horizontal_Move(blockLength * 2,velocity,0);
				delaycnt = 100;
				delay_flag = 1;
				stage = 13;
			}
			else if(stage == 13){
				Horizontal_Move(blockLength * 1,velocity,90);
				delaycnt = k210.xoffset * 100;
				delay_flag = 1;
				stage = 14;
			}
			else if(stage == 14){
				Horizontal_Move(blockLength * 2,velocity,0);
				delaycnt = 100;
				delay_flag = 1;
				stage = 15;
			}
			else if(stage == 15){
				OneKey_Land();
				counter = 0;
				stage = 0;
				hmi.mode = 0;
			}
			 
		//???��????
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
		u16 maxcnt = 3000;        //60?????? ��???????
		static u8 count1 = 0;
		static u8 stage = 0;			//?��???????��??
		u8 distance = 20;				  //???????????��??
		u8 velocity = 10;	        //???????? ��???10cm/s
		u8 targetHeight = height;
		LX_Change_Mode(3);
		counter ++;
		//????��????��????????��???
		if(hmi.mode != hmi.oldmode){
			counter = 0;
			stage = 0;
			count1 = 0;	
			hmi.oldmode = hmi.mode;
		}
		//��?����?????? ????20ms
		if(delay_flag){
			delay20();
		}
		else{
			if(stage == 0){
				FC_Unlock();
				stage = 1;
				//??��??��????
				delaycnt = 150;
				delay_flag = 1;
			}
			else if(stage == 1){
				OneKey_Takeoff(targetHeight);
				stage = 2;
				delaycnt = 300;
				delay_flag = 1;
			}
			else if(stage == 2){
				//?��??
				Vertical_Down(dh,10);
				delaycnt = 100;
				delay_flag = 1;
				stage = 3;
			}
			else if(stage == 3){
				Vertical_Up(dh,10);
				delaycnt = 100;
				delay_flag = 1;
				stage = 2;
			}
			else if(stage == 4){	//?��???��?��?��?��??????

			}
			else if(stage==5){	//?��???????��??��??��????

			}
			else if(stage==6){	//??��??????????��

			}
			else if(stage==7){	//?��???��?��?��?��??????

			}
			else if(stage==8){	//?��???????��??��??��????

			}
			else if(stage==9){	//????
				OneKey_Land();
				counter = 0;
				stage = 0;
				hmi.mode = 0;
			}
			 
		//???��????
		if(counter >= maxcnt){
			OneKey_Land();
			counter = 0;
			stage = 0;
			hmi.mode = 0;
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

