import sensor, image, time ,utime,pyb,math
from pyb import Pin
from pyb import LED
from pyb import UART

sensor.reset()
sensor.set_vflip(True)
#sensor.set_hmirror(True)
#sensor.set_contrast(1)
#sensor.set_gainceiling(16)
sensor.set_auto_gain(False) # True开启；False关闭，使用颜色追踪时，需关闭
sensor.set_auto_whitebal(False) # True开启；False关闭，使用颜色追踪时，需关闭
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_windowing((0,130,320,110))
sensor.skip_frames(time = 2000)     # WARNING: If you use QQVGA it may take seconds
clock = time.clock()                # to process a frame sometimes. 140 90 50 20
class ctrl(object):
    work_mode = 0x01 #工作模式，可以通过串口设置成其他模式
ctrl=ctrl()
uart = UART(3,115200)
def change_mod():
    if(uart.any()):
        txt=uart.read()
        if(len(txt)>=6):
            if(txt[0]==0xAA):
                if(txt[1]==0xff):
                    ctrl.work_mode=txt[4]
class find_b(object):
    def __init__(self):
        self.GRAYSCALE_THRESHOLD = [(57, 100, -47, -3, 8, 45)]#(57, 100, -47, -3, 8, 45)

    def get_dis(self,dat,daty):
        a =[0xAA,0xFF,0xf1,0x03,0x04,0x00,0x00,0x00,0x00]
        a[5]=int(dat)%256
        a[6]=int(daty)%256
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
        blobs = img.find_blobs(self.GRAYSCALE_THRESHOLD, pixels_threshold=2000, area_threshold=50, merge=True, margin=5)
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



            px=blobs[largest_blob].x()/2
            py=blobs[largest_blob].h()/2
            print("x:")
            print(px)
            print("y:")
            print(py)
            self.get_dis(px,py)
            blobs[largest_blob].w()
            ctrl.work_mode=0x01  #记得改

mode2=find_b()
while(True):
    clock.tick()
    img = sensor.snapshot()#.binary([THRESHOLD])
    img.lens_corr(1.7)
    change_mod()
    if (ctrl.work_mode==0x00):#MODE1 待机
        True
    if (ctrl.work_mode==0x01):#MODE2 开始找杆
        mode2.find(img)

