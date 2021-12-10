import sensor, image, time ,utime,pyb,math
from pyb import Pin
from pyb import LED
from pyb import UART

sensor.reset()
sensor.set_vflip(True)
#sensor.set_hmirror(True)
#sensor.set_contrast(1)
#sensor.set_gainceiling(16)
sensor.set_auto_gain(True) # True开启；False关闭，使用颜色追踪时，需关闭
sensor.set_auto_whitebal(True) # True开启；False关闭，使用颜色追踪时，需关闭
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_windowing((90,30,150,150)) # ((0,130,320,110))
sensor.skip_frames(time = 2000)     # WARNING: If you use QQVGA it may take seconds
clock = time.clock()                # to process a frame sometimes. 140 90 50 20
class ctrl(object):
    work_mode = 0x00 #工作模式，可以通过串口设置成其他模式
ctrl=ctrl()
uart = UART(3,115200)
def change_mod():
    if(uart.any()):
        txt=uart.read()
        if(len(txt)>=6):
            if(txt[0]==0xAA):
                if(txt[1]==0xff):
                    if(txt[4]==0x01):
                        sensor.set_windowing((0,130,320,110))  #模式一换window
                    if(txt[4]==0x02):
                        sensor.set_windowing((100,0,120,180))  #模式二换window
                    if(txt[4]==0x05):
                        sensor.set_windowing((146,67,100,130)) #模式大于五换window
                    if(txt[4]>=0x06):
                        sensor.set_windowing((90,30,150,150)) #模式大于五换window
                    ctrl.work_mode=txt[4]
class find_b(object):
    def __init__(self):
        self.GRAYSCALE_THRESHOLD = [(29, 75, -45, -17, -4, 47)]#(57, 100, -47, -3, 8, 45)

    def get_dis(self,dat,daty,datw):
        a =[0xAA,0xFF,0xf1,0x05,0x04,0x00,0x00,0x00,0x00,0x00,0x00]
        a[5]=int(dat)%256
        a[6]=int(daty)%256
        a[7]=int(datw)%256
        sum_check=0
        add_check=0
        for i in range(0,a[3]+4):
            sum_check += a[i]
            add_check += sum_check
        a[a[3]+4] = sum_check%256
        a[a[3]+5] = add_check%256
        a=bytes(a)
        #print(a)
        uart.write(a)
        return a

    def get_diswindow(self,hh):
        a =[0xAA,0xFF,0xf1,0x05,0x04,0x00,0x00,0x00,0x00,0x00,0x00]
        a[8]=int(hh)%256
        sum_check=0
        add_check=0
        for i in range(0,a[3]+4):
            sum_check += a[i]
            add_check += sum_check
        a[a[3]+4] = sum_check%256
        a[a[3]+5] = add_check%256
        a=bytes(a)
        #print(a)
        uart.write(a)
        return a

    def get_disnew(self,nw,nh):
        a =[0xAA,0xFF,0xf1,0x05,0x04,0x00,0x00,0x00,0x00,0x00,0x00]
        a[5]=int(nw)%256
        a[6]=int(nh)%256
        sum_check=0
        add_check=0
        for i in range(0,a[3]+4):
            sum_check += a[i]
            add_check += sum_check
        a[a[3]+4] = sum_check%256
        a[a[3]+5] = add_check%256
        a=bytes(a)
        #print(a)
        uart.write(a)
        return a


    def find(self,img):
        if (ctrl.work_mode==0x05):
            self.GRAYSCALE_THRESHOLD = [(43, 92, -11, 50, -27, 37)]   #淡灰色
        else:
            self.GRAYSCALE_THRESHOLD = [(29, 75, -45, -12, -4, 47)]
        blobs = img.find_blobs(self.GRAYSCALE_THRESHOLD, pixels_threshold=1500, area_threshold=50, merge=True, margin=5)#5
        if blobs:
            # 查找像素最多的blob的索引。
            largest_blob = 0
            most_pixels = 0
            for i in range(len(blobs)):
            #目标区域找到的颜色块（线段块）可能不止一个，找到最大的一个，作为本区域内的目标直线
                if blobs[i].pixels() > most_pixels:
                    most_pixels = blobs[i].pixels()
                    #merged_blobs[i][4]是这个颜色块的像素总数，如果此颜色块像素总数大于
                    #most_pixels，则把本区域作为像素总数最大的颜色块。更新most_pixels和largest_blob
                    largest_blob = i

            img.draw_rectangle(blobs[largest_blob].rect())
            # 将此区域的像素数最大的颜色块画矩形和十字形标记出来
            img.draw_cross(blobs[largest_blob].cx(),
                           blobs[largest_blob].cy())


            if(ctrl.work_mode==0x05):     #判断为拐点
                self.get_disnew(blobs[largest_blob].w(),blobs[largest_blob].h())
            if(ctrl.work_mode==0x06):     #判断为左下角
                self.get_disnew(blobs[largest_blob].w(),blobs[largest_blob].h())
            if(ctrl.work_mode==0x07):     #判断为右下角
                self.get_disnew(blobs[largest_blob].w(),blobs[largest_blob].h())
            if(ctrl.work_mode==0x08):     #判断为左上角
                self.get_disnew(blobs[largest_blob].w(),blobs[largest_blob].y())
            if(ctrl.work_mode==0x09):     #判断为右上角
                self.get_disnew(blobs[largest_blob].w(),blobs[largest_blob].y())
            px=blobs[largest_blob].x()/2
            py=blobs[largest_blob].h()/2
            pw=(blobs[largest_blob].w()+blobs[largest_blob].x())/2
            ph=(blobs[largest_blob].h()+blobs[largest_blob].y())/2  #改了
            print("x:")
            print(px)
            print("y dier:")
            print(py)
            print("w:")
            print(pw)
            if(ctrl.work_mode==0x01):
                self.get_dis(px,py,pw)
            if(ctrl.work_mode==0x02):
                self.get_diswindow(ph)    #改了 py
            blobs[largest_blob].w()
            ctrl.work_mode=0x00   #记得改

mode2=find_b()
while(True):
    clock.tick()
    change_mod()
    img = sensor.snapshot()#.binary([THRESHOLD])
    img.lens_corr(1.7)
    if (ctrl.work_mode==0x00):#MODE1 待机
        True
    if (ctrl.work_mode==0x01):#MODE2 开始找杆
        mode2.find(img)
    if (ctrl.work_mode==0x02):#MODE3 换window
        mode2.find(img)
    if (ctrl.work_mode==0x05):#MODE5 换window 拐点
        mode2.find(img)
    if (ctrl.work_mode==0x06):#MODE6 换window 左下
        mode2.find(img)
    if (ctrl.work_mode==0x07):#MODE7 换window 右下
        mode2.find(img)
    if (ctrl.work_mode==0x08):#MODE8 换window 右上
        mode2.find(img)
    if (ctrl.work_mode==0x09):#MODE9 换window 坐上
        mode2.find(img)
