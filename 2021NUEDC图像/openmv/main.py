# main.py -- put your code here!
THRESHOLD = (5, 70, -23, 15, -57, 0) # Grayscale threshold for dark things...
import sensor, image, time ,utime,pyb,math
from pyb import LED
from pyb import UART
THRESHOLD = (0, 105)

sensor.reset()
#sensor.set_vflip(True)
#sensor.set_hmirror(True)
#sensor.set_contrast(1)
#sensor.set_gainceiling(16)
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA) # 80x60 (4,800 pixels) - O(N^2) max = 2,3040,000.
sensor.set_windowing((160, 160))
sensor.skip_frames(time = 2000)     # WARNING: If you use QQVGA it may take seconds
clock = time.clock()                # to process a frame sometimes.
class ctrl(object):
    work_mode = 0x01 #工作模式，可以通过串口设置成其他模式
    num_flag =0
ctrl=ctrl()
def change_mod():
    if(uart.any()):
        txt=uart.read()
        if(txt[0]==0xAA):
            if(txt[5]==0x01):
                ctrl.work_mode=0x01
            if(txt[5]==0x02):
                ctrl.num_flag=1

uart = UART(3,115200)
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
        uart.write(a)
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
        print("Turn distance: %f" % center_pos)
        #将结果打印在terminal中

mode2=find_gan();
#uart.irq(trigger = UART.IRQ_RXIDLE,handler = change_mod())
while(True):
    clock.tick()
    img = sensor.snapshot()#.binary([THRESHOLD])
    #img.lens_corr(1.0)

    if (ctrl.work_mode==0x00):#MODE1 待机
        change_mod()
    if (ctrl.work_mode==0x01):#MODE2 开始找杆
        mode2.find(img)


