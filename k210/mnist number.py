import sensor,lcd,image
from machine import UART
import board
from board import board_info
from fpioa_manager import fm
import KPU as kpu
import ustruct
def check(datal):
    a =[0xAA,0xFF,0xf1,0x02,0x00,0x00,0x00,0x00]
    a[5]=datal
    sum_check=0;
    add_check=0;
    for i in range(0,a[3]+4):
        sum_check += a[i]
        add_check += sum_check
    a[a[3]+4] = sum_check%256
    a[a[3]+5] = add_check%256
    a=bytes(a)
    return a
lcd.init()
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_windowing((224, 224))    #set to 224x224 input
sensor.set_hmirror(1)               #flip camera
fm.register(9, fm.fpioa.UART1_TX, force=True)
fm.register(10, fm.fpioa.UART1_RX, force=True)
uart_A = UART(UART.UART1, 115200, 8, None, 1, timeout=1000, read_buf_len=4096)
task = kpu.load(0x300000)           #load model from flash address 0x200000
sensor.run(1)
while True:
    img = sensor.snapshot()
    #img.midpoint(2, bias=0.5, threshold=True, offset=5, invert=True)
    lcd.display(img,oft=(0,0))      #display large picture
    img1=img.to_grayscale(1)        #convert to gray
    img2=img1.resize(28,28)         #resize to mnist input 28x28
    img2.midpoint(2, bias=0.5, threshold=True, offset=5, invert=True)
    #a=img2.invert()                 #invert picture as mnist need
    a=img2.strech_char(1)           #preprocessing pictures, eliminate dark corner
    lcd.display(img2,oft=(224,32))  #display small 28x28 picture
    a=img2.pix_to_ai();             #generate data for ai
    fmap=kpu.forward(task,img2)     #run neural network model
    plist=fmap[:]                   #get result (10 digiZt's probability)
    pmax=max(plist)                 #get max probability
    max_index=plist.index(pmax)     #get the digit
    if pmax>=0.9:
        chec=check(max_index)
        print(chec)
        #tx_buf = ustruct.pack('bbb',b'\xAA\xFF\xF1\x01',str(max_index),b'\x00\x00')
        uart_A.write(chec)

    lcd.draw_string(224,0,"%d: %.3f"%(max_index,pmax),lcd.WHITE,lcd.BLACK)  #show result
