import sensor, image, time

# 注意！！！ 使用find_displacement()时，必须使用2的幂次方分辨率。
# 这是因为该算法由称为相位相关的东西提供动力，该相位相关使用FFT进行图像比较。
# 非2的幂次方分辨率要求填充到2的幂，这降低了算法结果的有用性。
# 请使用像B64X64或B64X32这样的分辨率（快2倍）。

# 您的OpenMV Cam支持2的幂次方分辨率64x32,64x64,128x64和128x128。
# 如果您想要32x32的分辨率，可以通过在64x64图像上执行“img.pool（2,2）”来创建它。

sensor.reset()                      # 复位并初始化传感器。

sensor.set_pixformat(sensor.RGB565) # Set pixel format to RGB565 (or GRAYSCALE)
#设置图像色彩格式，有RGB565色彩图和GRAYSCALE灰度图两种

sensor.set_framesize(sensor.B64X32)   # 将图像大小设置为64x64…… (64x32)……

sensor.skip_frames(time = 2000)     # 等待设置生效。
clock = time.clock()                # 创建一个时钟对象来跟踪FPS帧率。
img2=sensor.snapshot()

while(True):
    clock.tick() # 追踪两个snapshots()之间经过的毫秒数。
    img = sensor.snapshot() # 拍一张照片并返回图像。
    displacement = img.find_displacement(img2)
    img2=img.copy()

    # 没有滤波，偏移结果是嘈杂的，所以我们降低了一些精度。
    sub_pixel_x = int(displacement.x_translation() * 5) / 5.0
    sub_pixel_y = int(displacement.y_translation() * 5) / 5.0

    if(displacement.response()>0.1): # 低于0.1左右（YMMV），结果只是噪音。

        print("{0:+f}x {1:+f}y {2} {3} FPS".format(sub_pixel_x, sub_pixel_y,
              displacement.response(),
              clock.fps()))
    else:
        1
        #print(clock.fps())
