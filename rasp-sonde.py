#Code du Raspberry Sonde

import time
import serial

ser = serial.Serial(
        port='/dev/ttyUSB0',
        baudrate = 115200,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS,
        timeout=1
)

def debug_message(msg):
        with open("info_debug.txt",'a') as f:
                f.write(msg)
                print("Message : %s enregistre sur le fichier de debug"%(msg))

def read_sonde(id):
#Lire la sonde de l'ID correspondant
        print("Lecture de la sonde %s"%(id))
        ser.write("{T:1,S:%s,V:23}\n"%(id))

def action_sonde(id, val):
#En fonction de l'ID, effectuer tel action sur la sonde
        print("Action sur la sonde %s"%(id))
        ser.write("{T:2,S:%s,M:ACTION OK}\n"%(id))

with open ("/dev/ttyUSB0",'r') as r :
        while 1:
                time.sleep(1)
                x=r.readline()
                print(x)
                if x != "" and len(x)>4:
                        if x[3] == "1":
                                read_sonde(x[7:-2])
                        if x[3] == "2":
                                action_sonde(x[7], x[11:-2])
                        if x[0] != "{":
                                debug_message(x)


#print(x[7:-1]) pour le message en entier sans "}"
