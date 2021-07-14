import tkinter
from tkinter import *
import re
b = [0xAA,0x60,0xE0,0x0B,0x10,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0B,0x00]
x = []

def calculate():  
    x.clear()
    raw = E1.get().replace(" ","")
    raw = raw.replace("0x","")
    raw = raw.replace("0X","")
    strs = ""
    for i in range(0,len(raw),2):
        seq = (str(raw[i]),str(raw[i+1]))
        x.append(int(strs.join(seq),16))
    x.append(0)
    x.append(0)    
    sumcheck = 0
    addcheck = 0
    print(x)
    for i in range(0,x[3]+4):
        sumcheck += x[i] 
        addcheck += sumcheck
    x[x[3]+4] = sumcheck%256
    x[x[3]+5] = addcheck%256
    print(x)
    for i in range(0,x[3]+6):
        x[i] = str(hex(x[i])[2:]).upper().rjust(2,'0')
    E2.delete(0, END)
    E2.insert(0,x)

top = Tk()
top.title("校验计算")
top.geometry("300x110")
L1 = Label(top, text="原数据")
L1.place(x=5,y=10)
E1 = Entry(top, bd =5)
E1.place(x=50,y=10)
L2 = Label(top, text="加校验")
L2.place(x=5,y=60)
E2 = Entry(top, bd =5)
E2.place(x=50,y=60)
B = Button(top,bd=5,height = 4,width=5,text="计算",command=calculate)
B.place(x=220,y=5)
top.mainloop()

