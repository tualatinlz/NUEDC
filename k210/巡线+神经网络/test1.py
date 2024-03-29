THRESHOLD = (5, 70, -23, 15, -57, 0)
import sensor, image, time,lcd
import KPU as kpu
from pid import PID
rho_pid = PID(p=0.4, i=0)
theta_pid = PID(p=0.001, i=0)
from board import board_info
from fpioa_manager import fm
from machine import UART
lcd.init()
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_windowing((224, 224))
sensor.set_hmirror(0)
sensor.set_vflip(1)
sensor.run(1)
sensor.skip_frames(time = 2000)
fm.register(9, fm.fpioa.UART1_RX, force=True)
fm.register(10, fm.fpioa.UART1_TX, force=True)
uart_A = UART(UART.UART1, 115200, 8, None, 1, timeout=1000, read_buf_len=4096)
clock = time.clock()                # to process a frame sometimes.
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
task = kpu.load("/sd/oout.kmodel")#oupp
f=open("anchors.txt","r")
anchor_txt=f.read()
L=[]
for i in anchor_txt.split(","):
    L.append(float(i))
anchor=tuple(L)
f.close()
a = kpu.init_yolo2(task, 0.6, 0.3, 5, anchor)
f=open("lable.txt","r")
labels_txt=f.read()
labels = labels_txt.split(",")
f.close()
while(True):
    output=0;
    clock.tick()
    img = sensor.snapshot()
    #img2=img.resize(56,56)
    img2=img.copy(roi=(72,84,80,56))
    code = kpu.run_yolo2(task, img)
    if code:
        for i in code:
            a=img.draw_rectangle(i.rect(),(0,255,0),2)
            a = lcd.display(img,oft=(0,0))
        for i in code:
            lcd.draw_string(i.x()+45, i.y()-5, labels[i.classid()]+" "+'%.2f'%i.value(), lcd.WHITE,lcd.GREEN)
        #[{"x":117, "y":39, "w":108, "h":155, "value":0.924899, "classid":0, "index":0, "objnum":1}]
        #print(code[0].classid())
    else:
        a = lcd.display(img,oft=(0,0))
    #lcd.display(img2)
    img2.midpoint(2, bias=0.7, threshold=True, offset=19, invert=True)
    line = img2.get_regression([(100,100)], robust = True)

    if (line):
        rho_err = abs(line.rho())-img2.width()/2
        if line.theta()>90:
            theta_err = line.theta()-180
        else:
            theta_err = line.theta()
        img2.draw_line(line.line(), color = 127)
        #print(rho_err,line.magnitude(),rho_err)
        #print(rho_err)
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
    chec=check(output)
    if code:
        chec_num=check_num(int(labels[int(code[0].classid())]))
        uart_A.write(chec_num)
    else:
        chec_num=check_num(10)
        uart_A.write(chec_num)
    uart_A.write(chec)
    img1.draw_string(60, 100, str(output), scale=2)
    lcd.display(img1,oft=(200,32))
    lcd.display(img2,oft=(264,0))

a = kpu.deinit(task)
