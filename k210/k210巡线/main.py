# main.py -- put your code here!
THRESHOLD = (5, 70, -23, 15, -57, 0) # Grayscale threshold for dark things...
import sensor, image, time,lcd
#from pyb import LED
from pid import PID
from board import board_info
from fpioa_manager import fm
from machine import UART
rho_pid = PID(p=0.4, i=0)
theta_pid = PID(p=0.001, i=0)
#LED(1).on()
#LED(2).on()
#LED(3).on()
lcd.init()
sensor.reset()
sensor.set_vflip(True)
sensor.set_hmirror(True)
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQQVGA) # 80x60 (4,800 pixels) - O(N^2) max = 2,3040,000.
sensor.set_hmirror(0)
sensor.set_vflip(1)
sensor.set_windowing((0,0,80,56))
sensor.skip_frames(time = 2000)     # WARNING: If you use QQVGA it may take seconds
fm.register(9, fm.fpioa.UART1_TX, force=True)
fm.register(10, fm.fpioa.UART1_RX, force=True)
uart_A = UART(UART.UART1, 115200, 8, None, 1, timeout=1000, read_buf_len=4096)
clock = time.clock()                # to process a frame sometimes.
def check(datal):
    a =[0xAA,0xFF,0xf1,0x03 ,0x00,0x00,0x00,0x00,0x00]
    if datal>=0:
        a[6]=datal
    else:
        dat=360+datal
        a[6]=dat%256
        a[5]=int(dat/256)
        #print(a[6])
    sum_check=0;
    add_check=0;
    for i in range(0,a[3]+4):
        sum_check += a[i]
        add_check += sum_check
    a[a[3]+4] = sum_check%256
    a[a[3]+5] = add_check%256
    a=bytes(a)
    return a
while(True):
    output=0;
    clock.tick()
    img = sensor.snapshot()#.binary([THRESHOLD])
    #img.cut(20,0,8,56)
    img.midpoint(2, bias=0.7, threshold=True, offset=19, invert=True)
    line = img.get_regression([(100,100)], robust = True)

    if (line):
        rho_err = abs(line.rho())-img.width()/2
        if line.theta()>90:
            theta_err = line.theta()-180
        else:
            theta_err = line.theta()
        img.draw_line(line.line(), color = 127)
        #print(rho_err,line.magnitude(),rho_err)
        print(rho_err)
        if line.magnitude()>8:
            #if -40<b_err<40 and -30<t_err<30:
            rho_output = rho_pid.get_pid(rho_err,1)
            theta_output = theta_pid.get_pid(theta_err,1)
            output = rho_output+theta_output
            #print(output)#输出参数
        else:
            ouput=0
    else:
        ouput=0#50,-50
        pass
    #print(clock.fps())
    output=int(output)
    img1 = image.Image()
    img1.draw_string(60, 100, str(output), scale=2)
    lcd.display(img1)
    lcd.display(img)
    #print(output)#输出参数
    chec=check(output)
    #print(chec)
    uart_A.write(chec)
