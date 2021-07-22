import os
#from posix import listdir
import shutil

qianzhui = ""
listStr = ['python Augment_script.py --rotate=','',' --root=./Data'] 
# 把文件src_file移动到目录dest_file
def move(src_file, dest_file):
    for src in src_file:
        for dest in dest_file:
                shutil.copy(src, dest)

def createRotate(min,max):
    listStr = ['python Augment_script.py --rotate=','',' --root=./Data'] 
    for i in range(min,max):
        listStr[1] = str(i)
        cmd = ''.join(listStr)
        print(os.system(cmd))
        list = os.listdir('./Data/change_JPEGImages')  # 列出文件夹下所有的目录与文件
        for j in range(0, len(list)):
            paths = os.path.join('./Data/change_JPEGImages/', list[j])
            os.rename(paths,"./Data/jpg/"+qianzhui+str(j)+'0'+str(i)+".jpg")
        list = os.listdir('./Data/change_Annotations')  # 列出文件夹下所有的目录与文件
        for j in range(0, len(list)):
            paths = os.path.join('./Data/change_Annotations/', list[j])
            os.rename(paths,"./Data/xml/"+qianzhui+str(j)+'0'+str(i)+".xml")

def createNoise():
    listStr = ['python Augment_script.py --root=./Data --Noise NOISE'] 
    for i in range(0,1):
        cmd = ''.join(listStr)
        print(os.system(cmd))
        list = os.listdir('./Data/change_JPEGImages')  # 列出文件夹下所有的目录与文件
        for j in range(0, len(list)):
            paths = os.path.join('./Data/change_JPEGImages/', list[j])
            os.rename(paths,"./Data/jpg/"+qianzhui+str(j)+'0'+str(i)+".jpg")
        list = os.listdir('./Data/change_Annotations')  # 列出文件夹下所有的目录与文件
        for j in range(0, len(list)):
            paths = os.path.join('./Data/change_Annotations/', list[j])
            os.rename(paths,"./Data/xml/"+qianzhui+str(j)+'0'+str(i)+".xml")
            

qianzhui = input("请输入文件名前缀：")
if not os.path.exists('./Data/xml'):
    os.mkdir('./Data/xml')
if not os.path.exists('./Data/jpg'):
    os.mkdir('./Data/jpg')

#createNoise()
createRotate(0,5)
#createRotate(listStr,80,100)
#createRotate(listStr,170,190)
#createRotate(listStr,260,280)
createRotate(355,359)
input()
os.removedirs("./Data/change_Annotations")
os.removedirs("./Data/change_JPEGImages")
    