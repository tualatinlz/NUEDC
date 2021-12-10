import sensor, image, time
from pyb import UART
import utime
from pyb import LED
import ustruct

#初始化位置计算（左右像素 总长度单位mm）
left_pixel = 14
right_pixel = 555
length = 300

led = LED(1)
cx=0

red_thresholds = (41, 67, 55, 127, -16, 117)
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.VGA)
sensor.set_windowing((0,168,640,45))
sensor.skip_frames(time = 200)
clock = time.clock()
uart = UART(3,9600)
def find_max(blobs):
    max_size=0
    for blob in blobs:
        if blob.pixels() > max_size:
            max_blob=blob
            max_size = blob.pixels()
    return max_blob
while(True):
    clock.tick()
    img = sensor.snapshot()
    red_blobs_data = []
    img.lens_corr(1.4)
    red_blobs = img.find_blobs([red_thresholds],False,(0,0,600,40))
    if red_blobs:
        b=find_max(red_blobs)
        radius=b[2]/2
        img.draw_circle(b.cx(),b.cy(),int(radius))
        img.draw_cross(b.cx(), b.cy())
        red_blobs_data.append((b.cx(),b.cy()))
        cx=red_blobs_data[0][0]
        cy=red_blobs_data[0][1]
    #tx_buf="%.3d,%.3d"%(cx,cy)
    cx = (cx-left_pixel) * length / (right_pixel-left_pixel)
    tx_buf = ustruct.pack("ii",99,int(cx))
    #print(cx)
    ignore,text = ustruct.unpack("ii",tx_buf)
    print(text)
    utime.sleep_ms(50)
    uart.write(tx_buf)
    #uart.write(b'\xff\xff')
