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
        host = '192.168.1.173:5000'
        measure = ser.readline()
        date = time.time()
        data = {'data': date, 'measure': measure}
        print requests.post('http://{}/points/'.format(host), headers={'content-type': 'application/json'}, data=json.dumps(data))

if __name__ == '__main__':
    main()
