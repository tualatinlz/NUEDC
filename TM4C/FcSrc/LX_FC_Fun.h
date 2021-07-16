#ifndef __LX_FC_FUN_H
#define __LX_FC_FUN_H

//==引用
#include "SysConfig.h"

//==定义/声明
typedef struct
{
	u8 pre_locking;
	u8 stick_mit_pos;

} _sticks_fun_st;

typedef struct
{
	u8 CID;
	u8 CMD_0;
	u8 CMD_1;
} _cmd_fun_st;
//飞控状态
typedef struct
{
	//模式
	u8 fc_mode_cmd;
	u8 fc_mode_sta;

	//解锁上锁
	u8 unlock_cmd;
	u8 unlock_sta;

	//指令功能
	_cmd_fun_st cmd_fun;

	//state
	u8 imu_ready;
	u8 take_off;
	u8 in_air;
	u8 landing;

} _fc_state_st;

//==数据声明
extern _fc_state_st fc_sta;
//==函数声明
//static

//public
u8 FC_Unlock(void);
u8 FC_Lock(void);
u8 LX_Change_Mode(u8 new_mode);
u8 OneKey_Takeoff(u16 height_cm);
u8 OneKey_Land(void);
u8 OneKey_Flip(void);
u8 OneKey_Return_Home(void);
u8 Horizontal_Calibrate(void);
u8 Horizontal_Move(u16 distance_cm, u16 velocity_cmps, u16 dir_angle_0_360);
u8 Vertical_Target(u32 height_cm);
u8 Vertical_Up(u16 height_cm, u16 velocity_cm);
u8 Vertical_Down(u16 height_cm, u16 velocity_cm);
u8 Left_Rotate(u16 degree, u16 velocity_degree);
u8 Right_Rotate(u16 degree, u16 velocity_degree);
u8 Mag_Calibrate(void);
u8 ACC_Calibrate(void);
u8 GYR_Calibrate(void);
#endif
