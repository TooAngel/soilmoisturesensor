import serial
from serial.serialutil import SerialException

def main():
    try:
        ser = serial.Serial('/dev/ttyACM0', 9600)
    except SerialException:
        ser = serial.Serial('/dev/ttyACM1', 9600)
    
    while 1 :
        print ser.readline()

if __name__ == '__main__':
    main()
