/*
 * Copyright 2015 Tobias Wilken
 */

#include <RedFly.h>
#include <RedFlyClient.h>
#include <Config.h>
#include <HttpLibrary.h>
#include <Logging.h>

#define LOGLEVEL LOG_LEVEL_DEBUG

byte server[] = { 192, 168, 178, 25 };
int port = 80;

int sensorPin = A0;
int sensorValue = 0;

int motorPin = 12;

int connectionState = 0;
uint8_t ret;

int motorTime = 0;

int baud = 9600;
uint8_t pwr = LOW_POWER;

RedFlyClient client;

bool rf_init() {
    Log.Info("RedFly.init %d %d"CR, baud, pwr);
    ret = RedFly.init(baud, pwr);
    if (ret) {
        Log.Error("RedFly.init %d"CR, ret);
        return false;
    }
    return true;
}

bool rf_join() {
    Log.Info("RedFly.join %s"CR, BSSID);
    RedFly.scan();
    ret = RedFly.join(BSSID, PASSWORD);
    if (ret) {
        Log.Error("RedFly.join %d"CR, ret);
        RedFly.disconnect();
        return false;
    }
    return true;
}

bool rf_begin() {
    Log.Info("RedFly.begin"CR);
    ret = RedFly.begin();
    if (ret) {
        Log.Error("RedFly.begin %d"CR, ret);
        RedFly.disconnect();
        return false;
    }
    return true;
}

bool get_ip() {
    Log.Info("RedFly.getip %s"CR, HOSTNAME);
    ret = RedFly.getip(HOSTNAME, server);
    if (ret) {
        Log.Error("RedFly.get_ip %d"CR, ret);
        return false;
    }
    return true;
}

bool rf_set_client() {
    Log.Info("RedFlyClient %d.%d.%d.%d:%d"CR,
             server[0],
             server[1],
             server[2],
             server[3],
             port);
    client = RedFlyClient(server, port);
    return true;
}

bool read_sensor() {
    Log.Info("Read sensor value: ");
    sensorValue = analogRead(sensorPin);
    Log.Info("%d"CR, sensorValue);
    return true;
}

bool connect() {
    Log.Info("Connect(%d.%d.%d.%d:%d)"CR,
              server[0],
              server[1],
              server[2],
              server[3],
              port);
    ret = client.connect(server, port);
    if (ret) {
        return true;
    } else {
        Log.Error("Connect %d"CR, ret);
        return false;
    }
}

bool send_request() {
    Log.Info("client.write"CR);
    char resultChar[255];

    get_request_data(NAME, HOSTNAME, sensorValue, resultChar);
    client.write(resultChar);

    return true;
}

bool read_response() {
    Log.Info("read_response"CR);
    motorTime = parse_response(client);
    Log.Debug("response: %d"CR, motorTime);
    client.stop();
    Log.Debug("client stopped"CR);
    return true;
}

bool start_motor() {
    Log.Info("start_motor"CR);
    if (motorTime > 0) {
        Log.Info("Start motor %d"CR, motorTime);
        digitalWrite(motorPin, HIGH);
        delay(motorTime * 1000);
        digitalWrite(motorPin, LOW);
    }
    return true;
}

bool waiting() {
    Log.Info("Waiting"CR);
    delay(20000 - (motorTime * 1000));
    Log.Debug("Waiting done"CR);
    motorTime = 0;
    connectionState = 5;
    return true;
}


bool (*connectionStates[8])();

void setup() {
    Log.Init(LOGLEVEL, 9600L);

    pinMode(motorPin, OUTPUT);
    Serial.begin(9600);
    connectionStates[0] = rf_init;
    connectionStates[1] = rf_join;
    connectionStates[2] = rf_begin;
    connectionStates[3] = get_ip;
    connectionStates[4] = rf_set_client;
    connectionStates[5] = read_sensor;
    connectionStates[6] = connect;
    connectionStates[7] = send_request;
}

void loop() {
    for (int i = connectionState; i < 8; i++) {
        if (!(*connectionStates[i])()) {
            Log.Error("Execution failed %d"CR, i);
            connectionState = max(0, connectionState - 1);
            delay(1000);
            return;
        }
    }

    read_response();
    start_motor();
    waiting();
}
