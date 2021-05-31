import time
import serial
import random
import json
ser = serial.Serial(
        port='/dev/ttyUSB0', #Replace ttyS0 with ttyAM0 for Pi1,Pi2,Pi0
        baudrate = 115200,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS,
        timeout=1
)

while 1:


        #S=int(input("enter ID : \n"))
        #V= input("enter action : ON or OF \n")
        #print(V)
        #t = random.randint(5,60)

        ser.write("{T:2,S:2,V:ON}\n")


        #time.sleep(t)
        #ser.write("{T:3,S:3,V:ON}")
        #time.sleep(t)
        #ser.write("{T:3,S:1,V:ON}")
        for i in range(1,10):
                time.sleep(1)
                x=ser.readline()
                print(x)
