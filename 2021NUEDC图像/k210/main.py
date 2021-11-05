#THRESHOLD = (38, 100, -25, 28, -49, 60)
import time,utime,machine,image,sensor,lcd,math
from machine import Timer
import KPU as kpu
from Maix import GPIO
from fpioa_manager import fm
from machine import UART
lcd.init()
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_windowing((224,224))
sensor.set_auto_gain(False) # True开启；False关闭，使用颜色追踪时，需关闭
sensor.set_auto_whitebal(False) # True开启；False关闭，使用颜色追踪时，需关闭
#sensor.set_hmirror(1)
#sensor.set_vflip(1)
sensor.run(1)
sensor.skip_frames(time = 2000)
fm.register(8, fm.fpioa.UART1_RX, force=True)
fm.register(7, fm.fpioa.UART1_TX, force=True)
uart_A = UART(UART.UART1, 115200, 8, None, 1, timeout=1000, read_buf_len=4096)
fm.register(9, fm.fpioa.UART2_RX, force=True)
fm.register(10, fm.fpioa.UART2_TX, force=True)
uart_B = UART(UART.UART2, 115200, 8, None, 1, timeout=1000, read_buf_len=4096)
fm.register(21,fm.fpioa.GPIO2)
laser_out=GPIO(GPIO.GPIO2,GPIO.OUT)
laser_out.value(0)
class ctrl(object):
    work_mode = 0x00 #工作模式，可以通过串口设置成其他模式

ctrl=ctrl()
#YOLOV2模型
class kkpu(object):
    def __init__(self,task,anchor,labels):#模型初始化
        #dir_name = "/sd/{}.kmodel".format(task)
        self.task=kpu.load(0x300000)
        anchors_name = "{}.txt".format(anchor)
        f=open(anchors_name,"r")
        anchor_txt=f.read()
        L=[]
        for i in anchor_txt.split(","):
            L.append(float(i))
        self.anchor=tuple(L)
        f.close()
        self.a = kpu.init_yolo2(self.task, 0.6, 0.3, 5, self.anchor)
        labels_name = "{}.txt".format(labels)
        f=open(labels_name,"r")
        labels_txt=f.read()
        self.labels = labels_txt.split(",")
        f.close()

    def get_dis(self,wei,dat):
        a =[0xAA,0xFF,0xf1,0x04,0x00,0x00,0x00,0x00,0x00,0x00]
        if(wei==0):
            a[4]=0x02
            a[5]=0x00
        if(wei==1):
            a[4]=0x02
            a[5]=0x01
        if(wei==2):
            a[4]=0x01
            a[5]=0x01
        if(wei==3):
            a[4]=0x01
            a[5]=0x00
        dat=abs(dat)
        a[7]=dat%256
        a[6]=int(dat/256)
        sum_check=0
        add_check=0
        for i in range(0,a[3]+4):
            sum_check += a[i]
            add_check += sum_check
        a[a[3]+4] = sum_check%256
        a[a[3]+5] = add_check%256
        a=bytes(a)
        print(a)
        uart_A.write(a)
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
            for i in code:
                num_x=i.x()+i.w()/2
                num_y=i.y()+i.h()/2
                    #int(labels[int(code[0].classid())])
                num_x=(112-num_x)*0.476
                num_y=(112-num_y)*0.476
            if(int(num_x)>0):
                self.get_dis(1,int(num_x))
            else:
                self.get_dis(0,int(num_x))
            if(int(num_y)>0):
                self.get_dis(2,int(num_y))
            else:
                self.get_dis(3,int(num_y))
            #uart_A.write(chec_num,)#0,1,2,3 前 后 左 右
            laser_out.value(1)
            utime.sleep_ms(5000) # 记得清
            laser_out.value(0)
            utime.sleep_ms(1000)
            ctrl.work_mode=0x00

class bar_code(object):
    def __init__(self):
        True
    def send_num(self,obj):
        a =[0xAA,0xFF,0xf1,0x02,0x04,0x00,0x00,0x00]
        a[5]=obj
        sum_check=0
        add_check=0
        for i in range(0,a[3]+4):
            sum_check += a[i]
            add_check += sum_check
        a[a[3]+4] = sum_check%256
        a[a[3]+5] = add_check%256
        a=bytes(a)
        uart_A.write(a)
        print(a)
        return a
    def get_num(self):
        if(uart_B.any()):
            txt=uart_B.read()
            print(txt)
            if(len(txt)>=3):
                if(txt[0]==0x30):
                    if(txt[1]==0x30):
                        self.send_num(txt[3]-48)
                        print(txt[3]-48)

#寻找淡绿色色块
class dot_green(object):
    def __init__(self):
        self.flag=1         #标记设1
        self.num=0
        self.ok=0
        self.x=0
        self.y=0
        self.THRESHOLD = (11, 75, -24, -5, 1, 19)#淡绿色阈值

    def laser_b(self):
        for i in range(0,2):
            laser_out.value(1)
            utime.sleep_ms(750) # 记得清
            laser_out.value(0)
            utime.sleep_ms(750)

    def send_ans(self): #喷洒完成
        a =[0xAA,0xFF,0xf1,0x02,0x05,0x01,0x00,0x00]
        sum_check=0
        add_check=0
        for i in range(0,a[3]+4):
            sum_check += a[i]
            add_check += sum_check
        a[a[3]+4] = sum_check%256
        a[a[3]+5] = add_check%256
        a=bytes(a)
        uart_A.write(a)
        return a
    def send_green(self): #识别绿色
        a =[0xAA,0xFF,0xf1,0x02,0x03,0x01,0x00,0x00]
        sum_check=0
        add_check=0
        for i in range(0,a[3]+4):
            sum_check += a[i]
            add_check += sum_check
        a[a[3]+4] = sum_check%256
        a[a[3]+5] = add_check%256
        a=bytes(a)
        uart_A.write(a)
        return a
    def find_dot(self,img):
        #thresholds为黑色物体颜色的阈值，是一个元组，需要用括号［ ］括起来可以根据不同的颜色阈值更改；pixels_threshold 像素个数阈值，
        #如果色块像素数量小于这个值，会被过滤掉area_threshold 面积阈值，如果色块被框起来的面积小于这个值，会被过滤掉；merge 合并，如果
        #设置为True，那么合并所有重叠的blob为一个；margin 边界，如果设置为5，那么两个blobs如果间距5一个像素点，也会被合并。
        for blob in img.find_blobs([self.THRESHOLD], pixels_threshold=2000, area_threshold=80, merge=True, margin=5):
            if blob.pixels()>=20:#寻找最大的黑点
                ##先对图像进行分割，二值化，将在阈值内的区域变为白色，阈值外区域变为黑色
                #img.binary([self.THRESHOLD])
                #对图像边缘进行侵蚀，侵蚀函数erode(size, threshold=Auto)，size为kernal的大小，去除边缘相邻处多余的点。threshold用
                #来设置去除相邻点的个数，threshold数值越大，被侵蚀掉的边缘点越多，边缘旁边白色杂点少；数值越小，被侵蚀掉的边缘点越少，边缘
                #旁边的白色杂点越多。

                if(blob.cx()<160 and blob.cx()>80 and blob.cy()>80 and blob.cx()<160 ):
                    self.ok=1
                #在图像中画一个十字；x,y是坐标；size是两侧的尺寸；color可根据自己的喜好设置
                img.draw_cross(blob.cx(),blob.cy(), color=127, size = 10)
                #在图像中画一个圆；x,y是坐标；5是圆的半径；color可根据自己的喜好设置
                img.draw_circle(blob.cx(), blob.cy(), 5, color = 127)
                print("centre_x = %d, centre_y = %d"%(blob.cx(), blob.cy()))

        #判断标志位 是否可以喷洒
        if(self.ok==1):
            self.send_green()
            self.laser_b()
            self.send_ans()
            print("YES!")
            self.flag = 1  #标记设1

        #清零标志位
        self.ok = 0


#寻找十字物体坐标
class dot(object):
    def __init__(self):
        self.pixels=0
        self.flag=0
        self.num=0
        self.ok=0
        self.x=0
        self.y=0
        self.THRESHOLD = (72, 99, -6, 11, -10, 11)
    def send_num(self,obj):
        a =[0xAA,0xFF,0xf1,0x03,0x01,0x00,0x00,0x00,0x00]
        a[5]=obj
        sum_check=0
        add_check=0
        for i in range(0,a[3]+4):
            sum_check += a[i]
            add_check += sum_check
        a[a[3]+4] = sum_check%256
        a[a[3]+5] = add_check%256
        a=bytes(a)
        uart_A.write(a)
        return a
    def find_dot(self,img):
        #thresholds为黑色物体颜色的阈值，是一个元组，需要用括号［ ］括起来可以根据不同的颜色阈值更改；pixels_threshold 像素个数阈值，
        #如果色块像素数量小于这个值，会被过滤掉area_threshold 面积阈值，如果色块被框起来的面积小于这个值，会被过滤掉；merge 合并，如果
        #设置为True，那么合并所有重叠的blob为一个；margin 边界，如果设置为5，那么两个blobs如果间距5一个像素点，也会被合并。
        for blob in img.find_blobs([self.THRESHOLD], pixels_threshold=1200, area_threshold=80, merge=True, margin=5):
            if self.pixels<blob.pixels():#寻找最大的黑点
                ##先对图像进行分割，二值化，将在阈值内的区域变为白色，阈值外区域变为黑色
                #img.binary([self.THRESHOLD])
                #对图像边缘进行侵蚀，侵蚀函数erode(size, threshold=Auto)，size为kernal的大小，去除边缘相邻处多余的点。threshold用
                #来设置去除相邻点的个数，threshold数值越大，被侵蚀掉的边缘点越多，边缘旁边白色杂点少；数值越小，被侵蚀掉的边缘点越少，边缘
                #旁边的白色杂点越多。
                #img.erode(2)
                self.pixels=blob.pixels() #将像素值赋值给dot.pixels
                self.x = blob.cx() #将识别到的物体的中心点x坐标赋值给dot.x
                self.y = blob.cy() #将识别到的物体的中心点x坐标赋值给dot.x
                self.ok= 1
                #在图像中画一个十字；x,y是坐标；size是两侧的尺寸；color可根据自己的喜好设置
                img.draw_cross(self.x, self.y, color=127, size = 10)
                #在图像中画一个圆；x,y是坐标；5是圆的半径；color可根据自己的喜好设置
                img.draw_circle(self.x, self.y, 5, color = 127)

                print("centre_x = %d, centre_y = %d"%(self.x, self.y))


        #判断标志位 赋值像素点数据
        if(self.ok==1):
            x_wei=(160-self.x)*0.476
            y_wei=(120-self.y)*0.476
            #self.send_num()
        #清零标志位
        self.pixels = 0
        self.ok = 0

#声明灰绿色块类
object1=dot_green()
#声明条码转发
object2=bar_code()
#声明YOLO类(模型名称，anchor，lable)
object3=kkpu('out','anchors','lable')
#声明寻点类
object4=dot()

#改变工作模式
def change_mod():
    if(uart_A.any()):
        txt=uart_A.read()
        if(len(txt)>=6):
            if(txt[0]==0xAA):
                if(txt[1]==0xff):
                    ctrl.work_mode=txt[4]
                if(txt[5]==0x01):
                    object1.flag=0


#主程序
while(True):
    img = sensor.snapshot()
    if (ctrl.work_mode==0x01):#MODE1 寻找(淡绿色)色块
        if(object1.flag==0):
            object1.find_dot(img)
            lcd.display(img)
    if (ctrl.work_mode==0x04):#MODE2 条形码获取与转发
        object2.get_num()
    if (ctrl.work_mode==0x02):#MODE3 YOLO模型 字母A的识别
        object3.runn(img)
    if (ctrl.work_mode==0x03):#MODE4 #寻找十字的坐标
        object4.find_dot(img)
        sensor.set_windowing((320,240))
    if (ctrl.work_mode==0x00):#MODE5 #空
        True
    change_mod()#模式改变
