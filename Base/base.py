import serial
from struct import *
ser = serial.Serial('COM5', 9600)
print(calcsize(">BffffffffffffddffhhIQ"))
while 1:
    print(unpack(">BffffffffffffddffhhHQ",ser.read(calcsize(">BffffffffffffddffhhHQ"))))
    