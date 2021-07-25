# Untitled - By: Tualatin - 周四 7月 22 2021
THRESHOLD = (38, 100, -25, 28, -49, 60)
#(38, 100, -25, 28, -49, 60)
import time,utime,machine,image,sensor,lcd
from pid import PID
from machine import Timer
import KPU as kpu
from Maix import GPIO
from fpioa_manager import fm
from machine import UART
rho_pid = PID(p=0.175, i=0.2)
theta_pid = PID(p=0.35, i=0)
lcd.init()
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_windowing((224,224))
sensor.set_auto_gain(False) # True开启；False关闭，使用颜色追踪时，需关闭
sensor.set_auto_whitebal(False) # True开启；False关闭，使用颜色追踪时，需关闭
sensor.set_hmirror(0)
sensor.set_vflip(1)
sensor.run(1)
sensor.skip_frames(time = 2000)
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
class ctrl(object):
    work_mode = 0x03 #工作模式，可以通过串口设置成其他模式
#YOLOV2模型
class kkpu(object):
    def __init__(self,task,anchor,labels):#模型初始化
        dir_name = "/sd/{}.kmodel".format(task)
        self.task=kpu.load(dir_name)
        anchors_name = "{}.txt".format(anchor)
        f=open(anchors_name,"r")
        anchor_txt=f.read()
        L=[]
        for i in anchor_txt.split(","):
            L.append(float(i))
        self.anchor=tuple(L)
        f.close()
        self.a = kpu.init_yolo2(self.task, 0.8, 0.3, 5, self.anchor)
        labels_name = "{}.txt".format(labels)
        f=open(labels_name,"r")
        labels_txt=f.read()
        self.labels = labels_txt.split(",")
        f.close()

    def check_num(obj):
        a =[0xAA,0xFF,0xf1,0x03,0x01,0x00,0x00,0x00,0x00]
        a[5]=obj
        sum_check=0;
        add_check=0;
        for i in range(0,a[3]+4):
            sum_check += a[i]
            add_check += sum_check
        a[a[3]+4] = sum_check%256
        a[a[3]+5] = add_check%256
        a=bytes(a)
        return a

    def runn(self,img):#运行模型并通过串口发送识别到的编号
        code = kpu.run_yolo2(self.task, img)
        if code:
            for i in code:
                a=img.draw_rectangle(i.rect(),(0,255,0),2)
                a = lcd.display(img)
                for i in code:
                    lcd.draw_string(i.x()+45, i.y()-5, self.labels[i.classid()]+" "+'%.2f'%i.value(), lcd.WHITE,lcd.GREEN)
        else:
            a = lcd.display(img)
        if code:
            chec_num=kkpu.check_num(int(code[0].classid()))#int(labels[int(code[0].classid())])#
            uart_A.write(chec_num)
        else:
            chec_num=kkpu.check_num(10)
            uart_A.write(chec_num)



ctrl=ctrl()
#巡线角度信息发送
def check(datal):
    a =[0xAA,0xFF,0xf1,0x03,0x00,0x00,0x00,0x00,0x00]
    #print(a[4])
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
#巡线偏移量信息发送
def check_dis(datal):
    a =[0xAA,0xFF,0xf1,0x03,0x02,0x00,0x00,0x00,0x00]
    if datal>=0:
        a[6]=datal
    else:
        a[6]=abs(datal)
        a[5]=1
    sum_check=0;
    add_check=0;
    for i in range(0,a[3]+4):
        sum_check += a[i]
        add_check += sum_check
    a[a[3]+4] = sum_check%256
    a[a[3]+5] = add_check%256
    a=bytes(a)
    return a
#TOF400激光测距信息发送
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
#寻找竖线
def find_stline(img2):
    rhoo=0
    output=0

    img2.binary([THRESHOLD]).invert()
    line = img2.get_regression([(100,100)],robust = True)
    #print(line)
    if (line):
        rho_err = abs(line.rho())-img2.width()/2
        if line.theta()>90:
            theta_err = line.theta()-180
        else:
            theta_err = line.theta()
        theta_err = line.theta()
        img2.draw_line(line.line(), color = 127)
        #print(rho_err)

        if line.magnitude()>8:
            #if -40<b_err<40 and -30<t_err<30:
            rho_output = rho_pid.get_pid(rho_err,1)
            theta_output = theta_pid.get_pid(theta_err,1)
            output = theta_output
            rhoo=int(rho_output)
            chec_dis=check_dis(rhoo)
            uart_A.write(chec_dis)
            #print(output)#输出参数
        else:

            ouput=0
            chec_dis=check_dis(0)
            uart_A.write(chec_dis)
    else:
        chec_dis=check_dis(0)
        uart_A.write(chec_dis)
        ouput=0#50,-50
        pass
    output=int(output)
    chec=check(output)
    print(output)
    uart_A.write(chec)
    lcd.display(img2)

#改变工作模式
def change_mod():
    global count_txt
    if(uart_A.any()):
        txt=uart_A.read()
        if(txt[0]==0xAA):
            if(txt[4]==0x00):
                ctrl.work_mode=txt[5]
wave_echo_pin.irq(callback,GPIO.IRQ_BOTH,GPIO.WAKEUP_NOT_SUPPORT,1)
nownn=0


#声明YOLO类(模型名称，anchor，lable)
object1=kkpu('out','anchors','lable')

#主程序
while(True):
    img = sensor.snapshot()
    if (ctrl.work_mode==0x01):#MODE1
    #超声波模块
        if(nownn==0):
            nownn=time.ticks_ms()
        else:
            if(time.ticks_ms()-nownn>500):
                wave_distance_process()
                time.sleep_ms(40)
                nownn=time.ticks_ms()


    if (ctrl.work_mode==0x02):#MODE2
    #寻找竖线
        img.rotation_corr(z_rotation=180)
        img2=img.copy(roi=(112,84,80,56))
        find_stline(img2)


    if (ctrl.work_mode==0x03):#MODE3
    #YOLO模型

        object1.runn(img)


    change_mod()#模式改变
