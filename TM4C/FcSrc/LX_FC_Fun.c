#include "LX_FC_Fun.h"
#include "ANO_DT_LX.h"

/*==========================================================================
 * 描述    ：凌霄飞控基本功能主程序
 * 更新时间：2020-03-31 
 * 作者		 ：匿名科创-Jyoun
 * 官网    ：www.anotc.com
 * 淘宝    ：anotc.taobao.com
 * 技术Q群 ：190169595
 * 项目合作：18084888982，18061373080
============================================================================
 * 匿名科创团队感谢大家的支持，欢迎大家进群互相交流、讨论、学习。
 * 若您觉得匿名有不好的地方，欢迎您拍砖提意见。
 * 若您觉得匿名好，请多多帮我们推荐，支持我们。
 * 匿名开源程序代码欢迎您的引用、延伸和拓展，不过在希望您在使用时能注明出处。
 * 君子坦荡荡，小人常戚戚，匿名坚决不会请水军、请喷子，也从未有过抹黑同行的行为。  
 * 开源不易，生活更不容易，希望大家互相尊重、互帮互助，共同进步。
 * 只有您的支持，匿名才能做得更好。  
===========================================================================*/

_fc_state_st fc_sta;
//////////////////////////////////////////////////////////////////////
//基本功能函数
//////////////////////////////////////////////////////////////////////
//
u8 FC_Unlock()
{
	//
	fc_sta.unlock_cmd = 1; //解锁
	//按协议发送指令
	if (dt.wait_ck == 0) //没有其他等待校验的CMD时才发送本CMD
	{
		dt.cmd_send.CID = 0x10;
		dt.cmd_send.CMD[0] = 0x00;
		dt.cmd_send.CMD[1] = 0x01;
		CMD_Send(0XFF, &dt.cmd_send);
		return 1;
	}
	else
	{
		return 0;
	}
}
//
u8 FC_Lock()
{
	//
	fc_sta.unlock_cmd = 0; //上锁
	//按协议发送指令
	if (dt.wait_ck == 0) //没有其他等待校验的CMD时才发送本CMD
	{
		dt.cmd_send.CID = 0x10;
		dt.cmd_send.CMD[0] = 0x00;
		dt.cmd_send.CMD[1] = 0x02;
		CMD_Send(0XFF, &dt.cmd_send);
		fc_sta.take_off = 0;
		return 1;
	}
	else
	{
		return 0;
	}
}

//改变飞控模式(模式0-3)
u8 LX_Change_Mode(u8 new_mode)
{
	static u8 old_mode;
	if (old_mode != new_mode)
	{
		//
		if (dt.wait_ck == 0) //没有其他等待校验的CMD时才发送本CMD
		{
			old_mode = fc_sta.fc_mode_cmd = new_mode;
			//按协议发送指令
			dt.cmd_send.CID = 0X01;
			dt.cmd_send.CMD[0] = 0X01;
			dt.cmd_send.CMD[1] = 0X01;
			dt.cmd_send.CMD[2] = fc_sta.fc_mode_cmd;
			CMD_Send(0xff, &dt.cmd_send);
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else //已经在当前模式
	{
		return 1;
	}
}

//一键返航
//需要注意，程控模式下才能执行返航
u8 OneKey_Return_Home()
{
	//
	if (dt.wait_ck == 0) //没有其他等待校验的CMD时才发送本CMD
	{
		//按协议发送指令
		dt.cmd_send.CID = 0X10;
		dt.cmd_send.CMD[0] = 0X00;
		dt.cmd_send.CMD[1] = 0X07;
		CMD_Send(0xff, &dt.cmd_send);
		return 1;
	}
	else
	{
		return 0;
	}
}

//一键悬停
u8 OneKey_Hang()
{
	//
	if (dt.wait_ck == 0) //没有其他等待校验的CMD时才发送本CMD
	{
		//按协议发送指令
		dt.cmd_send.CID = 0X10;
		dt.cmd_send.CMD[0] = 0X00;
		dt.cmd_send.CMD[1] = 0X04;
		CMD_Send(0xff, &dt.cmd_send);
		return 1;
	}
	else
	{
		return 0;
	}
}
//一键起飞(高度cm)
u8 OneKey_Takeoff(u16 height_cm)
{
	//
	if (dt.wait_ck == 0) //没有其他等待校验的CMD时才发送本CMD
	{
		//按协议发送指令
		dt.cmd_send.CID = 0X10;
		dt.cmd_send.CMD[0] = 0X00;
		dt.cmd_send.CMD[1] = 0X05;
		dt.cmd_send.CMD[2] = BYTE0(height_cm);
		dt.cmd_send.CMD[3] = BYTE1(height_cm);
		CMD_Send(0xff, &dt.cmd_send);
		fc_sta.take_off = 1;
		return 1;
	}
	else
	{
		return 0;
	}
}
//一键降落
u8 OneKey_Land()
{
	//
	if (dt.wait_ck == 0) //没有其他等待校验的CMD时才发送本CMD
	{
		//按协议发送指令
		dt.cmd_send.CID = 0X10;
		dt.cmd_send.CMD[0] = 0X00;
		dt.cmd_send.CMD[1] = 0X06;
		CMD_Send(0xff, &dt.cmd_send);
		fc_sta.take_off = 0;
		return 1;
	}
	else
	{
		return 0;
	}
}
//平移(距离cm，速度cmps，方向角度0-360度)
u8 Horizontal_Move(u16 distance_cm, u16 velocity_cmps, u16 dir_angle_0_360)
{
	if(velocity_cmps < 10) velocity_cmps = 10;
	//
	if (dt.wait_ck == 0) //没有其他等待校验的CMD时才发送本CMD
	{
		//按协议发送指令
		dt.cmd_send.CID = 0X10;
		dt.cmd_send.CMD[0] = 0X02;
		dt.cmd_send.CMD[1] = 0X03;
		//
		dt.cmd_send.CMD[2] = BYTE0(distance_cm);
		dt.cmd_send.CMD[3] = BYTE1(distance_cm);
		dt.cmd_send.CMD[4] = BYTE0(velocity_cmps);
		dt.cmd_send.CMD[5] = BYTE1(velocity_cmps);
		dt.cmd_send.CMD[6] = BYTE0(dir_angle_0_360);
		dt.cmd_send.CMD[7] = BYTE1(dir_angle_0_360);
		//
		CMD_Send(0xff, &dt.cmd_send);
		return 1;
	}
	else
	{
		return 0;
	}
}

//指定对地高度(距离cm)
u8 Vertical_Target(u32 height_cm)
{
	//
	if (dt.wait_ck == 0) //没有其他等待校验的CMD时才发送本CMD
	{
		//按协议发送指令
		dt.cmd_send.CID = 0X10;
		dt.cmd_send.CMD[0] = 0X01;
		dt.cmd_send.CMD[1] = 0X02;
		//
		dt.cmd_send.CMD[2] = BYTE0(height_cm);
		dt.cmd_send.CMD[3] = BYTE1(height_cm);
		dt.cmd_send.CMD[4] = BYTE2(height_cm);
		dt.cmd_send.CMD[5] = BYTE3(height_cm);
		//
		CMD_Send(0xff, &dt.cmd_send);
		return 1;
	}
	else
	{
		return 0;
	}
}

//上升(距离cm，速度cms)
u8 Vertical_Up(u16 distance_cm, u16 velocity_cm)
{
	if(velocity_cm < 10) velocity_cm = 10;
	//
	if (dt.wait_ck == 0) //没有其他等待校验的CMD时才发送本CMD
	{
		//按协议发送指令
		dt.cmd_send.CID = 0X10;
		dt.cmd_send.CMD[0] = 0X02;
		dt.cmd_send.CMD[1] = 0X01;
		//
		dt.cmd_send.CMD[2] = BYTE0(distance_cm);
		dt.cmd_send.CMD[3] = BYTE1(distance_cm);
		dt.cmd_send.CMD[4] = BYTE0(velocity_cm);
		dt.cmd_send.CMD[5] = BYTE1(velocity_cm);
		//
		CMD_Send(0xff, &dt.cmd_send);
		return 1;
	}
	else
	{
		return 0;
	}
}

//下降(距离cm，速度cms 最小速度10)
u8 Vertical_Down(u16 distance_cm, u16 velocity_cm)
{
	if(velocity_cm < 10) velocity_cm = 10;
	//
	if (dt.wait_ck == 0) //没有其他等待校验的CMD时才发送本CMD
	{
		//按协议发送指令
		dt.cmd_send.CID = 0X10;
		dt.cmd_send.CMD[0] = 0X02;
		dt.cmd_send.CMD[1] = 0X02;
		//
		dt.cmd_send.CMD[2] = BYTE0(distance_cm);
		dt.cmd_send.CMD[3] = BYTE1(distance_cm);
		dt.cmd_send.CMD[4] = BYTE0(velocity_cm);
		dt.cmd_send.CMD[5] = BYTE1(velocity_cm);
		//
		CMD_Send(0xff, &dt.cmd_send);
		return 1;
	}
	else
	{
		return 0;
	}
}

//左转(角度deg，角速度degs)
u8 Left_Rotate(u16 degree, u16 velocity_degree)
{
	if(velocity_degree <5) velocity_degree = 5;
	//
	if (dt.wait_ck == 0) //没有其他等待校验的CMD时才发送本CMD
	{
		//按协议发送指令
		dt.cmd_send.CID = 0X10;
		dt.cmd_send.CMD[0] = 0X02;
		dt.cmd_send.CMD[1] = 0X07;
		//
		dt.cmd_send.CMD[2] = BYTE0(degree);
		dt.cmd_send.CMD[3] = BYTE1(degree);
		dt.cmd_send.CMD[4] = BYTE0(velocity_degree);
		dt.cmd_send.CMD[5] = BYTE1(velocity_degree);
		//
		CMD_Send(0xff, &dt.cmd_send);
		fc_sta.rotating = 1;
		return 1;
	}
	else
	{
		return 0;
	}
}


//右转(角度deg，角速度degs)
u8 Right_Rotate(u16 degree, u16 velocity_degree)
{
	//
	if(velocity_degree <5) velocity_degree = 5;
	if (dt.wait_ck == 0) //没有其他等待校验的CMD时才发送本CMD
	{
		//按协议发送指令
		dt.cmd_send.CID = 0X10;
		dt.cmd_send.CMD[0] = 0X02;
		dt.cmd_send.CMD[1] = 0X08;
		//
		dt.cmd_send.CMD[2] = BYTE0(degree);
		dt.cmd_send.CMD[3] = BYTE1(degree);
		dt.cmd_send.CMD[4] = BYTE0(velocity_degree);
		dt.cmd_send.CMD[5] = BYTE1(velocity_degree);
		//
		CMD_Send(0xff, &dt.cmd_send);
		fc_sta.rotating = 1;
		return 1;
	}
	else
	{
		return 0;
	}
}


//快速水平校准
u8 Horizontal_Calibrate()
{
	//
	if (dt.wait_ck == 0) //没有其他等待校验的CMD时才发送本CMD
	{
		//按协议发送指令
		dt.cmd_send.CID = 0X01;
		dt.cmd_send.CMD[0] = 0X00;
		dt.cmd_send.CMD[1] = 0X03;
		CMD_Send(0xff, &dt.cmd_send);
		return 1;
	}
	else
	{
		return 0;
	}
}

//磁力计校准
u8 Mag_Calibrate()
{
	//
	if (dt.wait_ck == 0) //没有其他等待校验的CMD时才发送本CMD
	{
		//按协议发送指令
		dt.cmd_send.CID = 0X01;
		dt.cmd_send.CMD[0] = 0X00;
		dt.cmd_send.CMD[1] = 0X04;
		CMD_Send(0xff, &dt.cmd_send);
		return 1;
	}
	else
	{
		return 0;
	}
}

//6面加速度校准
u8 ACC_Calibrate()
{
	//
	if (dt.wait_ck == 0) //没有其他等待校验的CMD时才发送本CMD
	{
		//按协议发送指令
		dt.cmd_send.CID = 0X01;
		dt.cmd_send.CMD[0] = 0X00;
		dt.cmd_send.CMD[1] = 0X05;
		CMD_Send(0xff, &dt.cmd_send);
		return 1;
	}
	else
	{
		return 0;
	}
}

//陀螺仪校准
u8 GYR_Calibrate()
{
	//
	if (dt.wait_ck == 0) //没有其他等待校验的CMD时才发送本CMD
	{
		//按协议发送指令
		dt.cmd_send.CID = 0X01;
		dt.cmd_send.CMD[0] = 0X00;
		dt.cmd_send.CMD[1] = 0X02;
		CMD_Send(0xff, &dt.cmd_send);
		return 1;
	}
	else
	{
		return 0;
	}
}
//旋转函数,0为顺时针
u8 Rotate(u8 direction,u8 angle){
	if(direction) Right_Rotate(angle,angle);
	else Left_Rotate(angle,angle);
	return 1;
}
