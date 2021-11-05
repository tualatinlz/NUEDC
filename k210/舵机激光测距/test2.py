import time,utime,machine,image,sensor,lcd
from machine import Timer,PWM
from Maix import GPIO
from fpioa_manager import fm
from board import board_info
from machine import UART
fm.register(8, fm.fpioa.UART1_RX, force=True)
fm.register(7, fm.fpioa.UART1_TX, force=True)
uart_A = UART(UART.UART1, 115200, 8, None, 1, timeout=1000, read_buf_len=4096)
fm.register(9, fm.fpioa.UART2_RX, force=True)
fm.register(10, fm.fpioa.UART2_TX, force=True)
uart_B = UART(UART.UART2, 115200, 8, None, 1, timeout=1000, read_buf_len=4096)
class Steering_motor():
    #舵机连线，注意和单片机共地;注意舵机初始化时角度和舵机
    #当前角度不能相差太多，不然舵机会慢慢抖过去
    tem=0
    #timer参数选择
    Timer_0 = Timer.TIMER0
    Timer_1 = Timer.TIMER1
    Timer_2 = Timer.TIMER2
    #channel参数选择
    Channel_0 = Timer.CHANNEL0
    Channel_1 = Timer.CHANNEL1
    Channel_2 = Timer.CHANNEL2
    Channel_3 = Timer.CHANNEL3
    #控制舵机的最大和最小PWM值,注意不同舵机这两个值不一样，需自己测量修改
    pwm_max = 13
    pwm_min = 2
    #舵机转动角度最大值
    angle_max = 180


    def __init__(self,pin,timer,channel,angle=90,freq=50):
        #pin——PWM输出引脚;Timer——定时器;
        #Channel——定时器通道;freq——PWM频率
        #angle——舵机初始化时的角度,范围[0-180]

        self._angle = angle  #舵机角度,范围[0-180]
        self.tim = Timer(timer,channel,mode=Timer.MODE_PWM)
        self.pwm = PWM(self.tim,
                freq=freq,
                duty=self.calculate_putty(self._angle),
                pin=pin,
                enable = False)
        self.pwm.enable()
        self.tem=0

    def calculate_putty(self,angle):
        #换算成舵机占空比
        return angle/self.angle_max*(self.pwm_max-self.pwm_min) + self.pwm_min

    def calculate_angle(self,pwmm):
        #换算成舵机占空比
        return 180*pwmm*(self.pwm_max-self.pwm_min)

    @property
    def anglel(self):
        #获取舵机当前角度
        return self._angle
    #@angle.setter
    def angle(self,setAngle):
        #设置舵机转到angle角度
        if setAngle > self._angle:
            for i in range(self._angle,setAngle+1):
                self.pwm.duty(self.calculate_putty(i))
                utime.sleep_ms(10)
                if(i%5==0):
                    print("angle %d"%i)
                    testt=self.get_dis()
                    print(testt)
                    if(testt<=300):
                        print("find pole!!!!!! %d"%i)
                        utime.sleep_ms(100)

        else:
            for i in range(self._angle,setAngle+1,-1):
                self.pwm.duty(self.calculate_putty(i))
                utime.sleep_ms(10)
                if(i%5==0):
                    print("angle %d"%i)
                    testt=self.get_dis()
                    print(testt)
                    if(testt<=300):
                        print("find pole!!!!!! %d"%i)
                        utime.sleep_ms(100)
        self._angle = setAngle

    def get_dis(self):
        if(uart_B.any()):
           txt=uart_B.read()
           if(txt[0]==0x01):
                   self.tem=txt[3]*256+txt[4]
        return self.tem




steering=Steering_motor(24,Steering_motor.Timer_2,Steering_motor.Channel_1)
while(1):

    steering.angle(135)

    steering.angle(30)
