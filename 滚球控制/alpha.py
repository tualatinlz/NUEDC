from pyb import Servo
from machine import UART,Pin
import machine
import micropython
import time
import utime
import ustruct

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
	uartdata=uartdp.read(uartdp.any())
	ooo=string_del(uartdata)
	if (ooo >= 1 and ooo <=300):
		pid1.SetPoint = ooo
	print(pid1.SetPoint)
	return

class PIDbak:
    def __init__(self, P=100.0, I=0, D=0):
        self.Kp = P
        self.Ki = I
        self.Kd = D
        self.sample_time = 0.00
        self.current_time = time.time()
        self.last_time = self.current_time
        self.clear()
    def clear(self):
        self.SetPoint = 0.0
        self.PTerm = 0.0
        self.ITerm = 0.0
        self.DTerm = 0.0
        self.last_error = 0.0
        self.int_error = 0.0
        self.windup_guard = 20.0
        self.output = 0.0
    def update(self, feedback_value):
        error = self.SetPoint - feedback_value
        self.current_time = time.time()
        delta_time = self.current_time - self.last_time
        delta_error = error - self.last_error
        if (delta_time >= self.sample_time):
            self.PTerm = self.Kp * error#比例
            self.ITerm += error * delta_time#积分
            if (self.ITerm < -self.windup_guard):
                self.ITerm = -self.windup_guard
            elif (self.ITerm > self.windup_guard):
                self.ITerm = self.windup_guard
            self.DTerm = 0.0
            if delta_time > 0:
                self.DTerm = delta_error / delta_time
            self.last_time = self.current_time
            self.last_error = error
            self.output = self.PTerm + (self.Ki * self.ITerm) + (self.Kd * self.DTerm)
    def setKp(self, proportional_gain):
        self.Kp = proportional_gain
    def setKi(self, integral_gain):
        self.Ki = integral_gain
    def setKd(self, derivative_gain):
        self.Kd = derivative_gain
    def setWindup(self, windup):
        self.windup_guard = windup
    def setSampleTime(self, sample_time):
        self.sample_time = sample_time

class PID:
	def __init__(self,P=100.0, I=0, D=0):
		self.Kp = P    #P
		self.Ki = I    #I
		self.Kd = D    #D
		self.set_val = 0    #设定值
		self.error_last = 0 #上一时刻的差值
		self.error_prev = 0 #上上一时刻的差值
		self.error_sum = 0  #所有时刻的差值总和
		self.error_In = [2]
		self.clear()

	def clear(self):
		self.SetPoint = 0.0
		self.PTerm = 0.0
		self.ITerm = 0.0
		self.DTerm = 0.0
		self.last_error = 0.0
		self.int_error = 0.0
		self.windup_guard = 20.0
		self.output = 0.0
	# 增量计算公式：
	# Pout=Kp*[e(t) - e(t-1)] + Ki*e(t) + Kd*[e(t) - 2*e(t-1) +e(t-2)]
	def update(self,val_in): # val_in当前时刻的输入量
		error = self.set_val - val_in
		self.error_In.append(error)
		#系统刚开始时
		t = len(self.error_In)
		#print(t)
		if t > 2:
			self.error_last = self.error_In[t-1]
			self.error_prev = self.error_In[t-2]
		else:
			self.error_last = 0
			self.error_prev = 0
		self.output = self.Kp*(error-self.error_prev) + self.Ki*error + self.Kd*(error-2*self.error_last+self.error_prev)
		self.error_prev = self.error_last
		self.error_last = error
		if t>=250:
			er_now=self.error_In[t-1]
			er_last=self.error_In[t-2]
			er_prev=self.error_In[t-3]
			self.error_In.clear()
			self.error_In.append(er_prev)
			self.error_In.append(er_last)
			self.error_In.append(er_now)
#初始化参数
#参数定义
#计算得出输出

if __name__ == "__main__":
	#初始化一号舵机
	s1 = Servo(1)
	s1.pulse_width(20)
	s1.angle(0) 
	#初始化openmv及显示串口
	uartdp = UART(1, baudrate=9600)
	uartmv = UART(2, baudrate=9600)
	uartdp.irq(trigger = UART.IRQ_RXIDLE,handler = UART_ISR )
	uartdp.write(b'\xff\xff\xff')
	#初始化pid
	pid1 = PID(10,-5.2,38)
	pid1.SetPoint = 150
	noww = 150
	header = 0
	bin_data = bytearray(100)

	while 1:
		#读取小球位置
		if uartmv.any():
			uartmv.readinto(bin_data)
			header,noww = ustruct.unpack("ii",bin_data)
		#print(noww)
		if(header == 99):
			pid1.update(noww)
			res = pid1.output
			coutt=res/25-2
			print(coutt)
			s1.angle(-coutt)
		utime.sleep_ms(100)