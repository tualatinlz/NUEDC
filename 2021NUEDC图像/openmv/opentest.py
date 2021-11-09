# main.py -- put your code here!
THRESHOLD = (0, 46, -22, 5, -8, 23) # Grayscale threshold for dark things...
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
sensor.set_auto_whitebal(False) # True开启；False关闭，使用颜色追踪时，需关闭
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA) # 80x60 (4,800 pixels) - O(N^2) max = 2,3040,000.
sensor.set_windowing((160, 120))
sensor.skip_frames(time = 2000)     # WARNING: If you use QQVGA it may take seconds
clock = time.clock()                # to process a frame sometimes.
class ctrl(object):
    work_mode = 0x00 #工作模式，可以通过串口设置成其他模式
ctrl=ctrl()
def change_mod():
    if(uart.any()):
        txt=uart.read()
        if(len(txt)>=6):
            if(txt[0]==0xAA):
                if(txt[1]==0xff):
                    ctrl.work_mode=txt[4]

uart = UART(3,115200)

uart_b= UART(1,115200)
class find_gan(object):
    def __init__(self):
        self.GRAYSCALE_THRESHOLD = [(0, 28, -30, 12, -21, 19)]#灰度图 16
        #GRAYSCALE_THRESHOLD =[(0, 45, -9, 24, -4, 17)]#寻杆阈值设置（彩色图）
        self.ROIS = [ # [ROI, weight]
                (0, 100, 160, 20, 0.6, 0.0), # 你需要为你的应用程序调整权重
                (0, 050, 160, 20, 0.3, 0.4), # 取决于你的机器人是如何设置的。
                (0, 000, 160, 20, 0.1, 0.6)
               ]
        # Compute the weight divisor (we're computing this so you don't have to make weights add to 1).
        self.weight_sum = 0 #权值和初始化
        for r in self.ROIS: self.weight_sum += r[4] # r[4] is the roi weight.
        #计算权值和。遍历上面的三个矩形，r[4]即每个矩形的权值。

    #串口发送位置坐标
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
        #print(a)
        uart.write(a)
        return a


    def find(self,img):
        centroid_sum = 0
        most_pixels = 0
        blobs_cont=0
        wide_now=0
        #利用颜色识别分别寻找三个矩形区域内的线段
        for r in self.ROIS:
            blobs = img.find_blobs(self.GRAYSCALE_THRESHOLD, roi=r[0:4], pixels_threshold=20, area_threshold=10, merge=True, margin=5)
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
                blobs_cont+=1
                centroid_sum += blobs[largest_blob].cx() * r[4] # r[4] is the roi weight.
                wide_now += blobs[largest_blob].w() * r[5]
                #计算centroid_sum，centroid_sum等于每个区域的最大颜色块的中心点的x坐标值乘本区域的权值
        #仅当三个位置都有时
        if(blobs_cont==2 or blobs_cont==3):
            center_pos = (centroid_sum / self.weight_sum) # Determine center of line.
            #中间公式
            wide_c= (wide_now/ self.weight_sum)
            if(center_pos==0):  #当少于
                center_pos=80
            # 将center_pos转换为一个偏角。我们用的是非线性运算，所以越偏离直线，响应越强。
            #偏移厘米
            deflection_length=(center_pos-56)*1
            #deflection_angle = -math.atan((center_pos-80)/60)
            #图像大小为QQVGA 160x120.
            deflection_length=int(deflection_length)
            print(deflection_length)
            dis_n=0
            if(deflection_length>20):
                self.get_dis(2,10) #abs(deflection_length)
            elif(deflection_length<-20):
                self.get_dis(3,10) #abs(deflection_length)
            #else:
            #    mode3.get_dis()
            #    dis_n=mode3.dis/10

            elif(wide_c<18 and wide_c>1):     #2米向前走20cm
                print("jin")
                self.get_dis(0,20)
            elif(wide_c>18 ):
                True #开始识别
                print("!!!!!!!")
                a =[0xAA,0xFF,0xf1,0x02,0x03,0x01,0xA0,0xC8]
                a=bytes(a)
                uart.write(a)
                ctrl.work_mode=0x00
            print("xianzai%d"%wide_c)
            #如果距离够近则转模式
            time.sleep_ms(1000)  # 清

            #将结果打印在terminal中

mode2=find_gan()
while(True):
    clock.tick()
    img = sensor.snapshot()#.binary([THRESHOLD])
    #img.lens_corr(1.2)
    change_mod()
    if (ctrl.work_mode==0x00):#MODE1 待机
        True
    if (ctrl.work_mode==0x01):#MODE1 开始找杆
        mode2.find(img)
    if (ctrl.work_mode==0x02):#MODE2 传参
        a =[0xAA,0xFF,0xf1,0x01,0x01,0x01,0xA0,0xC8]
        a=bytes(a)
        uart_b.write(a)
        time.sleep_ms(100)
        if(uart_b.any()):
            txt=uart_b.read()
            txt=bytes(txt)
            uart.write(txt)
            ctrl.work_mode=0x00

