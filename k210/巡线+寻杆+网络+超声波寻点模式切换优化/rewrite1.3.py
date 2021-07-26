# Untitled - By: Tualatin - 周四 7月 22 2021
THRESHOLD = (38, 100, -25, 28, -49, 60)
#(38, 100, -25, 28, -49, 60)
import time,utime,machine,image,sensor,lcd,math
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
fm.register(8, fm.fpioa.UART1_RX, force=True)
fm.register(7, fm.fpioa.UART1_TX, force=True)
uart_A = UART(UART.UART1, 115200, 8, None, 1, timeout=1000, read_buf_len=4096)

class ctrl(object):
    work_mode = 0x05 #工作模式，可以通过串口设置成其他模式
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

class find_gan(object):
    def __init__(self):
        self.GRAYSCALE_THRESHOLD = [(0, 64)]#灰度图
        #GRAYSCALE_THRESHOLD =[(0, 45, -9, 24, -4, 17)]#寻杆阈值设置（彩色图）
        self.ROIS = [ # [ROI, weight]
                (0, 100, 160, 20, 0.7), # 你需要为你的应用程序调整权重
                (0, 050, 160, 20, 0.3), # 取决于你的机器人是如何设置的。
                (0, 000, 160, 20, 0.1)
               ]
        # Compute the weight divisor (we're computing this so you don't have to make weights add to 1).
        self.weight_sum = 0 #权值和初始化
        for r in self.ROIS: self.weight_sum += r[4] # r[4] is the roi weight.
        #计算权值和。遍历上面的三个矩形，r[4]即每个矩形的权值。

    def send_dis(self,datal):
        a =[0xAA,0xFF,0xf1,0x03,0x04,0x00,0x00,0x00,0x00]
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
        if(a[5]==0x00 and a[6]==0x00):
            return 0
        print(a[5])
        print(a[6])
        uart_A.write(a)
        return a
    #串口发送位置坐标


    def find(self,img):
        centroid_sum = 0
        most_pixels = 0
        #利用颜色识别分别寻找三个矩形区域内的线段
        for r in self.ROIS:
            blobs = img.find_blobs(self.GRAYSCALE_THRESHOLD, roi=r[0:4], merge=True)
            # r[0:4] is roi tuple.
            #找到视野中的线,merge=true,将找到的图像区域合并成一个

            #目标区域找到直线
            if blobs:
                # 查找像素最多的blob的索引。
                largest_blob = 0
                for i in range(len(blobs)):
                #目标区域找到的颜色块（线段块）可能不止一个，找到最大的一个，作为本区域内的目标直线
                    if blobs[i].pixels() > most_pixels:
                        most_pixels = blobs[i].pixels()
                        #merged_blobs[i][4]是这个颜色块的像素总数，如果此颜色块像素总数大于
                        #most_pixels，则把本区域作为像素总数最大的颜色块。更新most_pixels和largest_blob
                        largest_blob = i

                # 在色块周围画一个矩形。
                img.draw_rectangle(blobs[largest_blob].rect())
                # 将此区域的像素数最大的颜色块画矩形和十字形标记出来
                img.draw_cross(blobs[largest_blob].cx(),
                               blobs[largest_blob].cy())

                centroid_sum += blobs[largest_blob].cx() * r[4] # r[4] is the roi weight.
                #计算centroid_sum，centroid_sum等于每个区域的最大颜色块的中心点的x坐标值乘本区域的权值

        center_pos = (centroid_sum / self.weight_sum) # Determine center of line.
        #中间公式

        # 将center_pos转换为一个偏角。我们用的是非线性运算，所以越偏离直线，响应越强。
        # 非线性操作很适合用于这样的算法的输出，以引起响应“触发器”。
        deflection_angle = 0
        #机器人应该转的角度

        # 80是X的一半，60是Y的一半。
        # 下面的等式只是计算三角形的角度，其中三角形的另一边是中心位置与中心的偏差，相邻边是Y的一半。
        # 这样会将角度输出限制在-45至45度左右。（不完全是-45至45度）。

        deflection_angle = -math.atan((center_pos-80)/60)
        #角度计算.80 60 分别为图像宽和高的一半，图像大小为QQVGA 160x120.
        #注意计算得到的是弧度值

        deflection_angle = math.degrees(deflection_angle)
        #将计算结果的弧度值转化为角度值

        # 现在你有一个角度来告诉你该如何转动机器人。
        # 通过该角度可以合并最靠近机器人的部分直线和远离机器人的部分直线，以实现更好的预测。
        center_pos=center_pos-80
        self.send_dis(int(center_pos))
        lcd.display(img)
        print("Turn distance: %f" % center_pos)
        #将结果打印在terminal中


#超声波类
class ultrasonic_ini (object):
    fm.register(9, fm.fpioa.GPIOHS0, force=True)
    fm.register(10, fm.fpioa.GPIOHS1, force=True)
    _wave_echo_pin=GPIO(GPIO.GPIOHS0,GPIO.IN,GPIO.PULL_DOWN)
    _wave_trig_pin=GPIO(GPIO.GPIOHS1,GPIO.OUT,GPIO.PULL_NONE)
    def __init__(self):#初始化定义引脚和中断
        self.tim_counter = 0
        self.nownn = 0
        self._wave_echo_pin.irq(self.callback,GPIO.IRQ_BOTH,GPIO.WAKEUP_NOT_SUPPORT,1)

    #超声波信息发送
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
    def wave_distance_calculation(self):
        wave_distance = (self.tim_counter/1000)*340/2
        #输出最终的测量距离（单位mm）
        if(wave_distance>2000):
            return 0
        wave_distance=int(wave_distance)
        ultrasonic_ini.get_disA(wave_distance)
        print('wave_distance',wave_distance)

    #超声波数据处理
    def wave_distance_process(self):
        self._wave_trig_pin.value(1)
        time.sleep_us(50)
        self._wave_trig_pin.value(0)




    #超声波延迟发送
    def delay_send(self,delay_time,period_time):
        if(self.nownn==0):
            self.nownn=time.ticks_ms()
        else:
            if(time.ticks_ms()-self.nownn>period_time):
                ultrasonic_ini.wave_distance_process(ultrasonic_ini)
                time.sleep_ms(delay_time)
                self.nownn=time.ticks_ms()


    #外部中断配置
    def callback(self,pin_num):
        if(self._wave_echo_pin.value()==1):
            self.tim_counter = time.ticks_us()
        else:
            self.tim_counter= time.ticks_us()-self.tim_counter
            self.wave_distance_calculation()

ctrl=ctrl()
#寻找圆形物体坐标
class dot(object):
    def __init__(self) :
        self.pixels=0
        self.flag=0
        self.num=0
        self.ok=0
        self.x=0
        self.y=0
    def find_dot(self,img):
        #thresholds为黑色物体颜色的阈值，是一个元组，需要用括号［ ］括起来可以根据不同的颜色阈值更改；pixels_threshold 像素个数阈值，
        #如果色块像素数量小于这个值，会被过滤掉area_threshold 面积阈值，如果色块被框起来的面积小于这个值，会被过滤掉；merge 合并，如果
        #设置为True，那么合并所有重叠的blob为一个；margin 边界，如果设置为5，那么两个blobs如果间距5一个像素点，也会被合并。
        for blob in img.find_blobs([THRESHOLD], pixels_threshold=80, area_threshold=80, merge=True, margin=5):
            if self.pixels<blob.pixels():#寻找最大的黑点
                ##先对图像进行分割，二值化，将在阈值内的区域变为白色，阈值外区域变为黑色
                img.binary([THRESHOLD])
                #对图像边缘进行侵蚀，侵蚀函数erode(size, threshold=Auto)，size为kernal的大小，去除边缘相邻处多余的点。threshold用
                #来设置去除相邻点的个数，threshold数值越大，被侵蚀掉的边缘点越多，边缘旁边白色杂点少；数值越小，被侵蚀掉的边缘点越少，边缘
                #旁边的白色杂点越多。
                img.erode(2)
                dot.pixels=blob.pixels() #将像素值赋值给dot.pixels
                dot.x = blob.cx() #将识别到的物体的中心点x坐标赋值给dot.x
                dot.y = blob.cy() #将识别到的物体的中心点x坐标赋值给dot.x
                dot.ok= 1
                #在图像中画一个十字；x,y是坐标；size是两侧的尺寸；color可根据自己的喜好设置
                img.draw_cross(dot.x, dot.y, color=127, size = 10)
                #在图像中画一个圆；x,y是坐标；5是圆的半径；color可根据自己的喜好设置
                img.draw_circle(dot.x, dot.y, 5, color = 127)
                print("centre_x = %d, centre_y = %d"%(dot.x, dot.y))

        #判断标志位 赋值像素点数据
        dot.flag = dot.ok
        dot.num = dot.pixels

        #清零标志位
        dot.pixels = 0
        dot.ok = 0
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


#声明YOLO类(模型名称，anchor，lable)
object1=kkpu('out','anchors','lable')
#声明寻杆类
object2=find_gan()
#声明超声波类
object3=ultrasonic_ini()
#声明寻点类
object4=dot()

#主程序
while(True):
    img = sensor.snapshot()
    if (ctrl.work_mode==0x01):#MODE1
    #超声波模块
        object3.delay_send(50,500)


    if (ctrl.work_mode==0x02):#MODE2
    #寻找竖线
        img.rotation_corr(z_rotation=180)
        img2=img.copy(roi=(112,84,80,56))
        find_stline(img2)


    if (ctrl.work_mode==0x03):#MODE3
    #YOLO模型

        object1.runn(img)


    if (ctrl.work_mode==0x04):#MODE4
    #寻找黑杆
        img.rotation_corr(z_rotation=90)
        img2=img.copy(roi=(80,60,160,120))
        img2=img2.to_grayscale()           #默认使用灰度图阈值输入
        object2.find(img2)


    if (ctrl.work_mode==0x05):#MODE5
    #寻找点的坐标
        object4.find_dot(img)
    change_mod()#模式改变
