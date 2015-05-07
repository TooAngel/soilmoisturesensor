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

int state = 0;
int wait = 50000;
uint8_t ret;
char resultChar[255];

int baud = 9600;
uint8_t pwr = LOW_POWER;


RedFlyClient client(server, 80);


bool rf_init() {
    log("RedFly.init(");
    log(String(baud));
    log(", ");
    log(String(pwr));
    logln(")");
    ret = RedFly.init(baud, pwr);
    if (ret) {
        logError("init", ret);
        return false;
    }
    return true;
}

bool rf_join() {
    log("RedFly.join: ");
    log(String(BSSID));
    logln("");
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
    log("RedFly.getip(");
    log(String(HOSTNAME));
    logln(")");
    char* hostname_char;
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
    logln("client.write:");
    get_request_data(NAME, HOSTNAME, sensorValue, resultChar);
    client.write(resultChar);
    return true;
}

int read_response() {
    logln("read_response");
    int response = parse_response(client);
    logln("response: ");
    logln(String(response));
    client.stop();
    return response;
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
}

void loop() {
    int motorTime = 0;
    for (int i = state; i < 8; i++) {
        if (!(*states[i])()) {
            logln("failed");
            state = max(0, state - 1);
            delay(1000);
            return;
        }
    }

    motorTime = read_response();
    client.stop();
    if (motorTime > 0) {
        logln("Start motor");
        logln(String(motorTime));
        digitalWrite(12, HIGH);
        delay(motorTime * 1000);
        digitalWrite(12, LOW);
    }

    state = 5;
    logln("wait");
    delay(wait - (motorTime * 1000));
}
