/*
 * Copyright 2015 Tobias Wilken
 */

#include <RedFly.h>
#include <RedFlyClient.h>
#include <Config.h>
#include <HttpLibrary.h>
#include <Output.h>

byte server[] = { 192, 168, 178, 25 };
int port = 80;

int sensorPin = A0;
int sensorValue = 0;

char resultChar[255];

int motorPin = 12;
int ledPin = 13;

int connectionState = 0;
uint8_t ret;

int motorTime = 0;

int baud = 9600;
uint8_t pwr = LOW_POWER;

uint16_t previousMillis = 0;
uint16_t interval = 60000;

RedFlyClient client;


bool rf_init() {
    logBegin("RedFly.init", String(baud), String(pwr));
    ret = RedFly.init(baud, pwr);
    if (ret) {
        logError("init", ret);
        return false;
    }
    return true;
}

bool rf_join() {
    logBegin("RedFly.join", String(BSSID));
    RedFly.scan();
    ret = RedFly.join(BSSID, PASSWORD);
    if (ret) {
        logError("join", ret);
        RedFly.disconnect();
        return false;
    }
    return true;
}

bool rf_begin() {
    logln("RedFly.begin");
    ret = RedFly.begin();
    if (ret) {
        logError("begin", ret);
        RedFly.disconnect();
        return false;
    }
    return true;
}

bool get_ip() {
    logBegin("RedFly.getip", String(HOSTNAME));
    ret = RedFly.getip(HOSTNAME, server);
    if (ret) {
        logError("get_ip", ret);
        return false;
    }
    return true;
}

bool rf_set_client() {
    logServerAndPort("RedFlyClient", server, port);
    client = RedFlyClient(server, port);
    return true;
}

bool read_sensor() {
    log("Read sensor value: ");
    sensorValue = analogRead(sensorPin);
    logln(String(sensorValue));
    return true;
}

bool connect() {
    logServerAndPort("Connect", server, port);
    ret = client.connect(server, port);
    if (ret) {
        return true;
    } else {
        logError("connect", ret);
        return false;
    }
}

bool send_request() {
    logln("client.write");
    get_request_data(NAME, HOSTNAME, sensorValue, resultChar);
    client.write(resultChar);
    return true;
}

bool read_response() {
    logln("read_response");
    motorTime = parse_response(client);
    log("response: ");
    logln(String(motorTime));
    client.stop();
    return true;
}

bool start_motor() {
    logBegin("start_motor", "method");
    if (motorTime > 0) {
        logBegin("Start motor", String(motorTime));
        digitalWrite(motorPin, HIGH);
        delay(motorTime * 1000);
        digitalWrite(motorPin, LOW);
        motorTime = 0;
    }
    return true;
}

bool waiting() {
    logBegin("waiting", "method");
    uint16_t currentMillis = millis();
    if (currentMillis - previousMillis > interval || connectionState != 8) {
        previousMillis = currentMillis;
        logln("");
        logln("Waiting done");
        connectionState = 5;
    }
    log(".");
    return true;
}

bool (*states[8])();

void setup() {
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
        if (!(*states[i])()) {
            logError("Execution failed", i);
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
