import serial
from serial.serialutil import SerialException
import requests
import json
import time

def connect():
#     while True:
#     try:
    try:
        return serial.Serial('/dev/ttyACM0', 9600)
    except SerialException as e:
        print e
        return serial.Serial('/dev/ttyACM1', 9600)
#     except OSError as e:
#         print e
#         pass

def main():
    ser = connect()
    while True:
        host = 'https://sensors.cloudcontrolapp.com'
        try:
            data = ser.readline().strip()
        except SerialException as e:
            print e
            connect()
            continue
        print data
        measure = int(data)
        date = time.time()
        data = {'date': date, 'measure': measure}
        print data
#         response = requests.post('{}/points/'.format(host), headers={'content-type': 'application/json'}, data=json.dumps(data))
#         if response.status_code != 200:
#             print response.content

if __name__ == '__main__':
    main()
