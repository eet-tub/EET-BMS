import bms_rs485_protocol
import serial
import serial.tools.list_ports
import colorama as clr
import serial
import sys
import time
import datetime
import os

LOGHEADER = ""

dev = []
clr.init()

def move(x,y):
    print("\033[%d;%dH" % (y, x), end="")

ports = serial.tools.list_ports.comports(include_links=False)
for i,port in enumerate(ports,start=1) :
    print(str(i) + ". " + port.device)

portnum = input("Please choose a Port: ")
bmsnum = int(input("How many BMS are connected? "))
logginginterval = float(input("Please choose the logging interval in seconds: "))
logfilebasename = input("Please Enter the Name of the Folder for the logfile: ")
logfilepaths = []

for n in range(bmsnum):
    logfilepaths.append("./" + logfilebasename + "/logfile_bms_" + str(n+1) + ".csv")
    
try:
    os.makedirs("./"+logfilebasename)
    for logpath in logfilepaths:
        with open(logpath,'w') as file:
            file.write(LOGHEADER)
except:
    print("Cant create logfiles")
    sys.exit()


try:
    dev = serial.Serial(ports[int(portnum)-1].device,baudrate=115200,timeout=1)


except Exception as e:
    print("Failed to open Port")
    sys.exit()
    
running = True
while(running):
    try:
        logdatastr = []
        for bms, logfilepath in enumerate(logfilepaths, start=1):

            with open(logfilepath ,'a') as file:
                logdata = bms_rs485_protocol.bms_get_cell_data_as_string(dev,bms)
                file.write(str(datetime.datetime.now()) + ","+ logdata + "\n")
                logdatastr.append(logdata)

        #Pretty Data Formatting goes here
        
        move(10,5)
        for bms,bms_data in enumerate(logdatastr,start=1):
            print("BMS: " + str(bms) +" " + bms_data)        

       
    except KeyboardInterrupt:
        running = False
    except Exception as e:
        print(e)

    try:
        time.sleep(logginginterval)
    except KeyboardInterrupt:
        running = False

print("Closing COM Port")
dev.close()

