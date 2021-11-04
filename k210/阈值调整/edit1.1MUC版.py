 # Untitled - By: Tualatin - 周一 8月 2 2021
THRESHOLD = (38, 100, -25, 28, -49, 60)
from fpioa_manager import fm
fm.register(9, fm.fpioa.UART1_TX, force=True)
fm.register(10, fm.fpioa.UART1_RX, force=True)
from machine import UART
import sensor, image, time ,lcd
uart_A = UART(UART.UART1, 115200, 8, 1, 0, timeout=1000, read_buf_len=4096)
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time = 2000)
lcd.init()
clock = time.clock()
#f=open("threshold.txt","w")
#f.write("38,100,25,128,29,160")
#f.close()
def uart_edit():
    if(uart_A.any()):
        txt=uart_A.read()
        if(len(txt)>=11):
            if(txt[0]==0xAA):
                if(txt[3]==0x06):
                    THRES = "{},{},{},{},{},{}".format(txt[4],txt[5],txt[6],txt[7],txt[8],txt[9])
                    f=open("threshold.txt","w")
                    f.write(THRES)
                    f.close()


while(True):
    clock.tick()
    img = sensor.snapshot()
    f=open("threshold.txt","r")
    threshold_txt=f.read()
    L=[]
    for i in threshold_txt.split(","):
        L.append(int(i))

    for i in range(0,5):
        if(i>=2):
            L[i]-=128

    f.close()
    #print(THRESHOLD[1])
    img.binary([L]).invert()
    uart_edit()
    lcd.display(img)

