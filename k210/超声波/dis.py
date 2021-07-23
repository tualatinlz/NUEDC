# Untitled - By: Tualatin - 周四 7月 22 2021

import time,utime
from Maix import GPIO
from fpioa_manager import fm
from machine import UART
fm.register(9, fm.fpioa.GPIOHS0, force=True)
fm.register(10, fm.fpioa.GPIOHS1, force=True)
wave_echo_pin=GPIO(GPIO.GPIOHS0,GPIO.IN,GPIO.PULL_DOWN)
wave_trig_pin=GPIO(GPIO.GPIOHS1,GPIO.OUT,GPIO.PULL_NONE)
wave_distance = 0
tim_counter = 0
flag_wave = 0
fm.register(8, fm.fpioa.UART1_RX, force=True)
fm.register(7, fm.fpioa.UART1_TX, force=True)
uart_A = UART(UART.UART1, 115200, 8, None, 1, timeout=1000, read_buf_len=4096)

def get_disA(dat):
    a =[0xAA,0xFF,0xf1,0x03,0x03,0x00,0x00,0x00,0x00]
    a[6]=dat%256
    a[5]=int(dat/256)
    sum_check=0;
    add_check=0;
    for i in range(0,a[3]+4):
        sum_check += a[i]
        add_check += sum_check
    a[a[3]+4] = sum_check%256
    a[a[3]+5] = add_check%256
    a=bytes(a)
    if(a[5]==0x00 and a[6]==0x00):
        return 0
    print(a[5])
    print(a[6])
    uart_A.write(a)
    return a
#超声波距离计算
def wave_distance_calculation():
    global tim_counter
    wave_distance = (tim_counter/1000)*340/2
    #输出最终的测量距离（单位cm）
    if(wave_distance>2000):
        return 0
    wave_distance=int(wave_distance)
    get_disA(wave_distance)
    print('wave_distance',wave_distance)

#超声波数据处理
def wave_distance_process():
    global tim_counter
    tim_counter=0
    wave_trig_pin.value(1)
    time.sleep_us(50)
    wave_trig_pin.value(0)


#外部中断配置
def callback(line):
    global tim_counter
    if(wave_echo_pin.value()==1):
        tim_counter = time.ticks_us()
    else:
        tim_counter= time.ticks_us()-tim_counter
        wave_distance_calculation()

#中断配置
wave_echo_pin.irq(callback,GPIO.IRQ_BOTH,GPIO.WAKEUP_NOT_SUPPORT,1)
while(True):
    wave_distance_process()
    time.sleep_ms(500)
