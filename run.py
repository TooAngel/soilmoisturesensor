import serial
from serial.serialutil import SerialException
import requests
import json
import time

def main():
    print 'Started'
    try:
        ser = serial.Serial('/dev/ttyACM0', 9600)
    except SerialException:
        ser = serial.Serial('/dev/ttyACM1', 9600)
    
    while 1 :
        measure = ser.readline()
        date = time.time()
        data = {'data': date, 'measure': measure}
        print requests.post('http://192.168.0.101:5000/points/', headers={'content-type': 'application/json'}, data=json.dumps(data))

if __name__ == '__main__':
    main()
