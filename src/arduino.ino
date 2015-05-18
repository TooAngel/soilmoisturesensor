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

char resultChar[255];

int motorPin = 12;

int connectionState = 0;
uint8_t ret;

int motorTime = 0;

int baud = 9600;
uint8_t pwr = LOW_POWER;

uint16_t previousMillis = 0;
uint16_t interval = 60000;

RedFlyClient client;

bool rf_init() {
    Log.Info("RedFly.init %d %d"CR, baud, pwr);
    ret = RedFly.init(baud, pwr);
    if (ret) {
        Log.Error("ERROR: RedFly.init %d"CR, ret);
        return false;
    }
    return true;
}

bool rf_join() {
    Log.Info("RedFly.join %s", BSSID);
    RedFly.scan();
    ret = RedFly.join(BSSID, PASSWORD);
    if (ret) {
        Log.Error("ERROR: RedFly.join %d"CR, ret);
        RedFly.disconnect();
        return false;
    }
    return true;
}

bool rf_begin() {
    Log.Info("RedFly.begin"CR);
    ret = RedFly.begin();
    if (ret) {
        Log.Error("ERROR: RedFly.begin %d"CR, ret);
        RedFly.disconnect();
        return false;
    }
    return true;
}

bool get_ip() {
    Log.Info("RedFly.getip %s"CR, HOSTNAME);
    ret = RedFly.getip(HOSTNAME, server);
    if (ret) {
        Log.Error("ERROR: RedFly.get_ip %d"CR, ret);
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
    get_request_data(NAME, HOSTNAME, sensorValue, resultChar);
    client.write(resultChar);
    return true;
}

bool read_response() {
    Log.Info("read_response"CR);
    motorTime = parse_response(client);
    Log.Debug("response: %d"CR, motorTime);
//    client.stop();
//    Log.Debug("client stopped");
    return true;
}

bool start_motor() {
    Log.Info("start_motor"CR);
    if (motorTime > 0) {
        Log.Info("Start motor %d"CR, motorTime);
        digitalWrite(motorPin, HIGH);
        delay(motorTime * 1000);
        digitalWrite(motorPin, LOW);
        motorTime = 0;
    }
    return true;
}

bool waiting() {
    Log.Info("waiting"CR);
    uint16_t currentMillis = millis();
    if (currentMillis - previousMillis > interval || connectionState != 8) {
        previousMillis = currentMillis;
        Log.Debug("Waiting done"CR);
        connectionState = 5;
    }
    Log.Info(".");
    return true;
}

bool (*states[8])();

void setup() {
    Log.Init(LOGLEVEL, 9600L);

    pinMode(12, OUTPUT);
    Serial.begin(9600);
    states[0] = rf_init;
    states[1] = rf_join;
    states[2] = rf_begin;
    states[3] = get_ip;
    states[4] = rf_set_client;
    states[5] = read_sensor;
    states[6] = connect;
    states[7] = send_request;
    states[8] = read_response;
    states[9] = start_motor;
}

void run() {
    for (int i = connectionState; i < 10; i++) {
        Log.Debug("Executing connection state: %d"CR, i);
        if (!(*states[i])()) {
            Log.Error("Execution failed %d"CR, i);
            connectionState = max(0, connectionState - 1);
            delay(1000);
            return;
        }
    }
    waiting();
}

void loop() {
    run();
}
