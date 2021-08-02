enable_lens_corr = True # turn on for straighter lines...打开以获得更直的线条…
THRESHOLD = (38, 100, -25, 28, -49, 60)
#THRESHOLD = (30, 94, -23, 6, 21, -12)

import sensor, image, time ,lcd , pyb
from pyb import UART
sensor.reset()
sensor.set_pixformat(sensor.RGB565) # 灰度更快
sensor.set_framesize(sensor.QQVGA)#QQVGA 120*160
sensor.set_windowing((120,120))
sensor.skip_frames(time = 2000)
sensor.set_contrast(3)
lcd.init()
clock = time.clock()
uart = UART(3,115200)
#上下左右0123
def get_dis(wei,dat):
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
        a[7]=dat%256
        a[6]=int(dat/256)
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
nownn=0
flag_qian=0
while(True):
    clock.tick()
    img = sensor.snapshot()#.cartoon(seed_threshold=0.01, floating_thresholds=0.01)
    #img.find_edges(image.EDGE_CANNY, threshold=(50, 80))
    #img.erode(4, threshold = 1) 图像侵蚀
    #img.binary([THRESHOLD])#.invert()
    if enable_lens_corr: img.lens_corr(1.7) # for 2.8mm lens...
    s_count=0
    x_count=0
    z_count=0
    y_count=0
    qian=0
    duan=0
    hou=0
    you=0
    zuo=0
    for l in img.find_line_segments(merge_distance =20 , max_theta_diff = 15):
        img.draw_line(l.line(), color = (0, 255, 0))
        y_mean=(l.y1()+l.y2())/2
        x_mean=(l.x1()+l.x2())/2
        if(l.length()>=50):
            if(y_mean>60 and l.theta()>=75 and l.theta()<=105):
                if(l.length()>=50 and l.length()<=70):
                    duan+=1
                x_count+=1
            if(y_mean<=60 and l.theta()>=75 and l.theta()<=105):
                if(l.length()>=50 and l.length()<=70):
                    duan+=1
                s_count+=1
            if(x_mean<60 and (l.theta()<=15 or l.theta()>=165)):
                z_count+=1
            if(x_mean>=60 and (l.theta()<=15 or l.theta()>=165)):
                y_count+=1


            if(x_mean>=30 and x_mean<=90 and (l.theta()<=15 or l.theta()>=165)):
                if(min(l.y1(),l.y2())<=40):
                    qian+=1
            if(x_mean>=30 and x_mean<=90 and (l.theta()<=15 or l.theta()>=165)):
                if(max(l.y1(),l.y2())>=80):
                    hou+=1
            if(y_mean>=30 and y_mean<=90 and l.theta()>=75 and l.theta()<=105):
                if(max(l.x1(),l.x2())>=80):
                    you+=1
            if(y_mean>=30 and y_mean<=90 and l.theta()>=75 and l.theta()<=105):
                if(min(l.x1(),l.x2())<=40):
                    zuo+=1
        #print(l)
    if(nownn==0):
                nownn=pyb.millis()
    else:
        if(pyb.millis()-nownn>100):
            if(you==2):
                flag_qian=0
                get_dis(3,5)
                get_dis(0,0)
            elif(qian==2):
                get_dis(0,5)
                get_dis(3,0)
            elif(zuo==2):
                get_dis(2,5)
                get_dis(0,0)
            elif(hou==2):
                get_dis(1,5)
                get_dis(3,0)
            nownn=pyb.millis()
    print("shang:%d"%qian)
    print("xia:%d"%hou)
    print("zuo:%d"%zuo)
    print("you:%d"%you)


    #print("FPS %f" % clock.fps())
