import serial
from serial.serialutil import SerialException
import requests
import json
import time
import logging

logging.basicConfig(level=logging.DEBUG)

def connect():
    logging.debug('connect')
    while True:
        for device in ['/dev/ttyACM0', '/dev/ttyACM1']:
            try:
                ser = serial.Serial(device, 9600)
            except SerialException as e:
                logging.exception('SerialException')
                continue
            except OSError as e:
                logging.exception('OSError')
                continue
                
            logging.debug('Device: {}'.format(device))
            return ser
        time.sleep(1)

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
