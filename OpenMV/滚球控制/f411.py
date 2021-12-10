from pyb import Servo
from machine import UART,Pin
import machine
import micropython
import time
import utime
import ustruct
global aimm
def string_del(st):
	if  st[3]>=48 and st[3]<=57 :
		nube=(int(st[1])-48)*100+(int(st[2])-48)*10+int(st[3])-48
		return int(nube)
	else:
		if st[2]>=48 and st[2]<=57 :
			nube=(int(st[1])-48)*10+int(st[2])-48
			return int(nube)
		else:
			if st[1]>=48 and st[1]<=57 :
				nube=int(st[1])-48
				return int(nube)
	return -1

def UART_ISR(u):
	uartdata=uart.read(uart.any())
	PID_Param = PID_Param_init()
	PID_Param.Kp = 1
	PID_Param.Ki = -5.2
	PID_Param.Kd = 38
	global aimm
	ooo=string_del(uartdata)
	if ooo != -1 :
		aimm=ooo
	print("{}".format(PID_Param.set_val) )
	return

class PID_Param_init:
	def __init__(self):
		self.Kp = 0    #P
		self.Ki = 0    #I
		self.Kd = 0    #D
		self.set_val = 0    #设定值
		self.error_last = 0 #上一时刻的差值
		self.error_prev = 0 #上上一时刻的差值
		self.error_sum = 0  #所有时刻的差值总和
		self.error_In = [2]


# 增量计算公式：
# Pout=Kp*[e(t) - e(t-1)] + Ki*e(t) + Kd*[e(t) - 2*e(t-1) +e(t-2)]
def PID_Controller_Increa(pid,val_in): # val_in当前时刻的输入量
	error = pid.set_val - val_in
	pid.error_In.append(error)
	#系统刚开始时
	t = len(pid.error_In)
	print(t)
	if t > 2:
		pid.error_last = pid.error_In[t-1]
		pid.error_prev = pid.error_In[t-2]
	else:
		pid.error_last = 0
		pid.error_prev = 0
	Res = pid.Kp*(error-pid.error_prev) + pid.Ki*error + pid.Kd*(error-2*pid.error_last+pid.error_prev)
	pid.error_prev = pid.error_last
	pid.error_last = error
	if t>=250:
		er_now=pid.error_In[t-1]
		er_last=pid.error_In[t-2]
		er_prev=pid.error_In[t-3]
		pid.error_In.clear()
		pid.error_In.append(er_prev)
		pid.error_In.append(er_last)
		pid.error_In.append(er_now)
	return Res
s1 = Servo(1) # servo on position 1 (PA0, VIN, GND)
s1.pulse_width(20)
s1.angle(0) # move to 45 degrees
#s1.angle(-60) # move to -60 degrees in 1500ms
uart = UART(1, baudrate=9600)
uartmv = UART(2, baudrate=9600)
uart.irq(trigger = UART.IRQ_RXIDLE,handler = UART_ISR )
uart.write(b'\xff\xff\xff')
#初始化参数
PID_Param = PID_Param_init()
#参数定义
par=b'\xff\xff\xff'
PID_Param.Kp = 1
PID_Param.Ki = -5.2
PID_Param.Kd = 38
PID_Param.set_val=150
aimm=150
val_in = 120
noww=150
header = 0
bin_data = bytearray(100)
#计算得出输出
while 1:
	if uartmv.any():
		uartmv.readinto(bin_data)
		header,noww = ustruct.unpack("ii",bin_data)
	val_in=noww
	PID_Param.set_val=aimm
	res = PID_Controller_Increa(PID_Param, val_in)
	res=float(res)
	coutt=res/25-2
	#uart.write('t0.txt="{}"'.format(int(coutt)))
	#utime.sleep_ms(1)
	#uart.write(par)
	print(coutt)
	#PID_Param.Ki = -5.7
	#PID_Param.Kd = 45
	s1.angle(coutt)
	#print(res)
	#print(PID_Param.set_val)
	utime.sleep_ms(50)
	#utime.sleep_ms(1500)