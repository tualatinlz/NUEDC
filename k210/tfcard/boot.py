THRESHOLD = (38, 100, -25, 28, -49, 60)
import sensor, image, time,lcd,os,machine
import KPU as kpu
from pid import PID
rho_pid = PID(p=0.175, i=0.2)
theta_pid = PID(p=0.35, i=0)
from board import board_info
from fpioa_manager import fm
from machine import Timer
from machine import UART
lcd.init()
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_windowing((224,224))
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)
sensor.set_hmirror(0)
sensor.set_vflip(1)
sensor.run(1)
sensor.skip_frames(time = 2000)
fm.register(8, fm.fpioa.UART1_RX, force=True)
fm.register(7, fm.fpioa.UART1_TX, force=True)
uart_A = UART(UART.UART1, 115200, 8, None, 1, timeout=1000, read_buf_len=4096)
fm.register(9, fm.fpioa.UART2_RX, force=True)
fm.register(10, fm.fpioa.UART2_TX, force=True)
uart_B = UART(UART.UART2, 115200, 8, None, 1, timeout=1000, read_buf_len=4096)
clock = time.clock()
count_txt=0
def check(datal):
	a =[0xAA,0xFF,0xf1,0x03,0x00,0x00,0x00,0x00,0x00]
	if datal>=0:
		a[6]=datal
	else:
		dat=360+datal
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
def qr_save(img_in):
	code = img_in.find_qrcodes()
	if(code):
		for i in code:
			code_text = i.payload()
			print(code_text)
		img_in.save("/sd/QR1.jpg", quality=100)
def uart_save(img_in):
	global count_txt
	if(uart_A.any()):
		txt=uart_A.read()
		if(txt[0]==0xAA):
			if(txt[4]==0x00):
				dir_name = "/sd/{}.jpg".format(count_txt)
				sensor.snapshot().save(dir_name)
				count_txt=count_txt+1
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
def get_dis():
	a =[0xAA,0xFF,0xf1,0x03,0x03,0x00,0x00,0x00,0x00]
	tem=0
	if(uart_B.any()):
	   txt=uart_B.read()
	   if(txt[0]==0x01):
		   if(txt[3]==0xff):
			   a[5]=txt[3]
			   a[6]=txt[4]
		   else:
			   a[5]=txt[3]
			   a[6]=txt[4]
			   tem=txt[3]*256+txt[4]
			   print(tem)
	sum_check=0;
	add_check=0;
	for i in range(0,a[3]+4):
		sum_check += a[i]
		add_check += sum_check
	a[a[3]+4] = sum_check%256
	a[a[3]+5] = add_check%256
	a=bytes(a)
	return a
def on_timer(timer):
	if(uart_B.any()):
		uart_A.write(get_dis())
tim = machine.Timer(Timer.TIMER0, Timer.CHANNEL0, mode=Timer.MODE_PERIODIC, period=500, unit=Timer.UNIT_MS, callback=on_timer, arg=on_timer, start=True, priority=1, div=0)
task = kpu.load("/sd/out_txm.kmodel")
f=open("anchors_txm.txt","r")
anchor_txt=f.read()
L=[]
for i in anchor_txt.split(","):
	L.append(float(i))
anchor=tuple(L)
f.close()
a = kpu.init_yolo2(task, 0.8, 0.3, 5, anchor)
f=open("lable_txm.txt","r")
labels_txt=f.read()
labels = labels_txt.split(",")
f.close()
while(True):
	output=0
	rhoo=0
	clock.tick()
	img = sensor.snapshot()
	code = kpu.run_yolo2(task, img)
	if not code:
		img.rotation_corr(z_rotation=90)
		img2=img.copy(roi=(112,84,88,56))
		img.rotation_corr(z_rotation=0)
	if code:
		for i in code:
			a=img.draw_rectangle(i.rect(),(0,255,0),1)
			a = lcd.display(img,oft=(0,0))
		for i in code:
			lcd.draw_string(i.x()+45, i.y()-5, labels[i.classid()]+" "+'%.2f'%i.value(), lcd.WHITE,lcd.GREEN)
	else:
		a = lcd.display(img,oft=(0,0))
	if code:
		for i in code:
			img.draw_rectangle(i.rect(), (255,255,255), thickness=1, fill=True)
		img.rotation_corr(z_rotation=90)
		img2=img.copy(roi=(112,84,88,56))
		img.rotation_corr(z_rotation=0)
	img2.binary([THRESHOLD]).invert()
	line = img2.get_regression([(100,100)],robust = True)
	if (line):
		rho_err = abs(line.rho())-img2.width()/2
		if line.theta()>90:
			theta_err = line.theta()-180
		else:
			theta_err = line.theta()
		theta_err = line.theta()
		img2.draw_line(line.line(), color = 127)
		if line.magnitude()>8:
			rho_output = rho_pid.get_pid(rho_err,1)
			theta_output = theta_pid.get_pid(theta_err,1)
			output = theta_output
			rhoo=int(rho_output)
			chec_dis=check_dis(rhoo)
			uart_A.write(chec_dis)
		else:
			ouput=0
			chec_dis=check_dis(0)
			uart_A.write(chec_dis)
	else:
		chec_dis=check_dis(0)
		uart_A.write(chec_dis)
		ouput=0
		pass
	output=int(output)
	img1 = image.Image()
	chec=check(output)
	if code:
		chec_num=check_num(int(code[0].classid()))
		uart_A.write(chec_num)
	else:
		chec_num=check_num(10)
		uart_A.write(chec_num)
	uart_A.write(chec)
	img1.draw_string(60, 60, str(output), scale=2)
	if line:
		img1.draw_string(60, 80, str(line.magnitude()), scale=2)
		img1.draw_string(60, 100, str(rhoo), scale=2)
	lcd.display(img1,oft=(200,32))
	lcd.display(img2,oft=(200,0))
a = kpu.deinit(task)
