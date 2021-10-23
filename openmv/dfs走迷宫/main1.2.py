# main.py -- put your code here!
THRESHOLD = (5, 70, -23, 15, -57, 0) # Grayscale threshold for dark things...
import sensor, image, time ,utime,pyb
from pyb import LED
from pyb import UART
THRESHOLD = (0, 105)

sensor.reset()
#sensor.set_vflip(True)
#sensor.set_hmirror(True)
#sensor.set_contrast(1)
#sensor.set_gainceiling(16)
sensor.set_pixformat(sensor.GRAYSCALE)
sensor.set_framesize(sensor.QVGA) # 80x60 (4,800 pixels) - O(N^2) max = 2,3040,000.
sensor.set_windowing((160, 160))
sensor.skip_frames(time = 2000)     # WARNING: If you use QQVGA it may take seconds
clock = time.clock()                # to process a frame sometimes.
class ctrl(object):
    work_mode = 0x00 #工作模式，可以通过串口设置成其他模式
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
class dfs_mg(object):
    def __init__(self):
        global b,fin
        global flag,tot1
        global x_end,y_end
        self.flag=0
        self.tot1=0

        self.b = [[0 for i in range(10)] for i in range(10)]
        self.fin = [[0 for i in range(10)] for i in range(10)]

        self.x_end=9
        self.y_end=0
        self.x_st=0
        self.y_st=9
        self.THRESHOLD = (0, 105)

    def send_ready(self):
        a =[0xAA,0xFF,0xf1,0x03,0x05,0x00,0x00,0x00,0x00]
        sum_check=0;
        add_check=0;
        for i in range(0,a[3]+4):
            sum_check += a[i]
            add_check += sum_check
        a[a[3]+4] = sum_check%256
        a[a[3]+5] = add_check%256
        a=bytes(a)
        uart.write(a)
        return a

    def send_DIR(self,DIR):
        a =[0xAA,0xFF,0xf1,0x04,0x00,0x00,0x00,0X01,0x00,0x00]
        if(DIR==1):
            a[4]=0x02
            a[5]=0x00
        if(DIR==2):
            a[4]=0x02
            a[5]=0x01
        if(DIR==3):
            a[4]=0x01
            a[5]=0x01
        if(DIR==4):
            a[4]=0x01
            a[5]=0x00
        sum_check=0;
        add_check=0;
        for i in range(0,a[3]+4):
            sum_check += a[i]
            add_check += sum_check
        a[a[3]+4] = sum_check%256
        a[a[3]+5] = add_check%256
        a=bytes(a)
        uart.write(a)
        return a



    def drawee(self,a,img):
        for i in range(0,10):
                for j in range(0,10):
                    if a[i][j]==1:
                        img.draw_arrow(i*16+8, j*16+12, i*16+8, j*16+4, 255, size = 4, thickness = 1)
                    if a[i][j]==2:
                        img.draw_arrow(i*16+8, j*16+4, i*16+8, j*16+12, 255, size = 4, thickness = 1)
                    if a[i][j]==3:
                        img.draw_arrow(i*16+12, j*16+8, i*16+4, j*16+8, 255, size = 4, thickness = 1)
                    if a[i][j]==4:
                        img.draw_arrow(i*16+4, j*16+8, i*16+12, j*16+8, 255, size = 4, thickness = 1)
    def dfs_checkb(self,img,dire,x,y):    #dir 0,1,2,3对应上下左右
        tot=0
        x=16*x+8
        y=16*y+8
        if (dire==0 and (y-16>=0)):
            num=img.get_statistics(roi=(x-3,y-16,6,16))
            tot=num.min()
        if (dire==1 and (y+16<=160)):
            num=img.get_statistics(roi=(x-3,y,6,16))
            tot=num.min()
        if (dire==2 and (x-16>=0)):
            num=img.get_statistics(roi=(x-16,y-3,16,6))
            tot=num.min()
        if (dire==3 and (x+16<160)):
            num=img.get_statistics(roi=(x,y-3,16,6))
            tot=num.min()
        if tot==0:
            return 0
        if tot>=120:
            return 1
        else:
            return 0

    def drone_go(self,di):
        if di==1:
            self.send_DIR(1)
        if di==2:
            self.send_DIR(2)
        if di==3:
            self.send_DIR(3)
        if di==4:
            self.send_DIR(4)
    def str_fly(self):
        mg_x=self.x_end-self.x_st
        mg_y=self.y_st-self.y_end
        bu_x=1
        bu_y=1
        if(mg_x and mg_y):
            bu_x=6/mg_x
            bu_y=6/mg_y

        x_tot=0
        y_tot=0
        x=self.x_st
        y=self.y_st
        while(1):
            print("wait!")
            change_mod()
            if(ctrl.num_flag):
                if(x==self.x_end and y==self.y_end):
                    ctrl.work_mode=0x00
                    return 0
                if(x_tot>=1):
                    self.drone_go(4)
                    x_tot-=1
                    ctrl.num_flag=0
                if(y_tot>=1):
                    self.drone_go(2)
                    y_tot-=1
                    ctrl.num_flag=0
                if(x_tot<=-1):
                    self.drone_go(3)
                    x_tot+=1
                    ctrl.num_flag=0
                if(y_tot<=-1):
                    self.drone_go(1)
                    y_tot+=1
                    ctrl.num_flag=0
                if(self.fin[x][y]==1):
                    y_tot-=bu_y
                    y-=1
                elif(self.fin[x][y]==2):
                    y_tot+=bu_y
                    y+=1
                elif(self.fin[x][y]==3):
                    x_tot-=bu_x
                    x-=1
                elif(self.fin[x][y]==4):
                    x_tot+=bu_x
                    x+=1




    def dfs_find(self,x,y,img):
        self.tot1+=1
        #print(b)
        #utime.sleep_ms(1000)
        if self.tot1>=800:
            return 0
        if self.flag==1:
            return 0
        if(x==self.x_end and y==self.y_end):
            self.b[self.x_end][self.y_end]=1
            self.fin=self.b
            self.flag=1
            self.drawee(self.fin,img)
            self.send_ready()
            self.str_fly()
            #ctrl.work_mode=0
            utime.sleep_ms(1000)
            return 0

        #utime.sleep_ms(100)
        if(self.dfs_checkb(img,0,x,y)):  #1,2,3,4上下左右
            if(self.b[x][y-1]==0):
                self.b[x][y]=1
                self.dfs_find(x,y-1,img)
        if(self.dfs_checkb(img,3,x,y)):
            if(self.b[x+1][y]==0):
                self.b[x][y]=4
                self.dfs_find(x+1,y,img)
        if(self.dfs_checkb(img,2,x,y)):
            if(self.b[x-1][y]==0):
                self.b[x][y]=3
                self.dfs_find(x-1,y,img)
        if(self.dfs_checkb(img,1,x,y)):
            if(self.b[x][y+1]==0):
                self.b[x][y]=2
                self.dfs_find(x,y+1,img)
        if(self.flag==1):
            return 0
        self.b[x][y]=0
    def find_mg(self,img):
        if(img.find_blobs([self.THRESHOLD],merge=True)):
            min_x=159
            min_y=159
            max_x=0
            max_y=0
            for r in img.find_blobs([self.THRESHOLD],merge=True):
                min_x=min(r.x(),min_x)
                min_y=min(r.y(),min_y)
                max_x=max(r.x()+r.w(),max_x)
                max_y=max(r.y()+r.h(),max_y)
            self.x_st=(int((min_x+8)/16))
            self.y_st=(int((max_y-8)/16))
            self.x_end=(int((max_x-8)/16))
            self.y_end=(int((min_y+8)/16))
            if(self.x_end<0 or self.x_end>9):
                self.x_end=9
            if(self.y_end<0 or self.y_end>9):
                self.y_end=0
            if(self.x_st<0 or self.x_st>9):
                self.x_st=0
            if(self.y_st<0 or self.y_st>9):
                self.y_st=9
            for i in range(0,self.x_st):
                for j in range(0,10):
                    self.b[i][j]=-1
            for i in range(self.y_st+1,10):
                for j in range(0,10):
                    self.b[j][i]=-1
            for i in range(self.x_end+1,10):
                for j in range(0,10):
                    self.b[i][j]=-1
            for i in range(0,self.y_end):
                for j in range(0,10):
                    self.b[j][i]=-1

            print("start:",self.x_st,self.y_st)
            print("end:",self.x_end,self.y_end)
        self.dfs_find(self.x_st,self.y_st,img)

#uart.irq(trigger = UART.IRQ_RXIDLE,handler = change_mod())
while(True):
    clock.tick()
    img = sensor.snapshot()#.binary([THRESHOLD])
    #img.lens_corr(1.0)

    if (ctrl.work_mode==0x00):#MODE1 待机
        print(1)
        change_mod()
    if (ctrl.work_mode==0x01):#MODE2 开始寻路
        object1=dfs_mg()
        object1.find_mg(img)

    utime.sleep_ms(100)


