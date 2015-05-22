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

int baud = 9600;
uint8_t pwr = LOW_POWER;

bool connected = false;
int state = 0;

uint8_t ret;

RedFlyClient client(server, 80);

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
        connected = false;
        return false;
    }
    return true;
}

bool get_ip() {
    Log.Info("RedFly.getip %s"CR, HOSTNAME);
    char* hostname_char;
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
    client = RedFlyClient(server, 80);
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

int read_response() {
    Log.Info("read_response"CR);
    return parse_response(client);
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
}

void loop() {
    int motorTime = 0;
    for (int i = state; i < 8; i++) {
        if (!(*states[i])()) {
            Log.Error("Execution failed %d"CR, i);
            state = max(0, state - 1);
            delay(1000);
            return;
        }
    }

    motorTime = read_response();
    if (motorTime > 0) {
        Log.Info("Start motor: %d"CR, motorTime);
        digitalWrite(12, HIGH);
        delay(motorTime * 1000);
        digitalWrite(12, LOW);
    }

    state = 5;
    Log.Info("wait");
    delay(50000 - (motorTime * 1000));
}
