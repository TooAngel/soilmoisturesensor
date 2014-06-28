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
        host = 'http://192.168.1.173:5000'
#         host = 'https://sensors.cloudcontrolapp.com'
        measure = int(ser.readline().strip())
        date = time.time()
        data = {'date': date, 'measure': measure}
        response = requests.post('{}/points/'.format(host), headers={'content-type': 'application/json'}, data=json.dumps(data))
        if response.status_code != 200:
            print response.content

if __name__ == '__main__':
    main()
