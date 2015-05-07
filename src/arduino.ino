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

bool connected = false;
int state = 0;
int wait = 50000;

RedFlyClient client(server, 80);

bool rf_init() {
    int baud = 9600;
    uint8_t pwr = LOW_POWER;
    log("RedFly.init(");
    log(String(baud));
    log(", ");
    log(String(pwr));
    logln(")");
    uint8_t ret;
    ret = RedFly.init(baud, pwr);
    if (ret) {
        log("INIT ERROR: ");
        log(String(ret));
        logln("");
        return false;
    }
    return true;
}

bool rf_join() {
    log("RedFly.join: ");
    log(String(BSSID));
    logln("");
    uint8_t ret;
    RedFly.scan();
    ret = RedFly.join(BSSID, PASSWORD);
    if (ret) {
        log("JOIN ERROR: ");
        log(String(ret));
        logln("");
        RedFly.disconnect();
        return false;
    }
    return true;
}

bool rf_begin() {
    logln("RedFly.begin");
    uint8_t ret;
    ret = RedFly.begin();
    if (ret) {
        log("ERROR BEGIN: ");
        log(String(ret));
        logln("");
        RedFly.disconnect();
        connected = false;
        return false;
    }
    return true;
}

bool get_ip() {
    log("RedFly.getip(");
    log(String(HOSTNAME));
    logln(")");
    char* hostname_char;
    if (RedFly.getip(HOSTNAME, server)) {
        logln("DNS ERR");
        return false;
    }
    return true;
}

bool rf_set_client() {
    int port = 80;
    log("RedFlyClient(");
    log(String(server[0]));
    log(".");
    log(String(server[1]));
    log(".");
    log(String(server[2]));
    log(".");
    log(String(server[3]));
    log(", ");
    log(String(port));
    logln(")");
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
    log("Connect to: ");
    log(String(server[0]));
    log(".");
    log(String(server[1]));
    log(".");
    log(String(server[2]));
    log(".");
    log(String(server[3]));
    log(":");
    log(String(port));
    logln("");
//    logln("connect" + String(server));
    if (client.connect(server, port)) {
        return true;
    } else {
        logln("CLIENT ERR: ");
        return false;
    }
}

bool send_request() {
    logln("client.write:");
    char resultChar[255];
    get_request_data(resultChar);
    log(String(resultChar));
    logln("");
    client.write(resultChar);
    return true;
}

void get_request_data(char* resultChar) {
    String data = "POST /sensors/" NAME "/points/ HTTP/1.1\r\nHost:";
    data += HOSTNAME;
    data += "\r\nContent-Type: application/json";
    data += "\r\nContent-length: 16\r\n\r\n{\"measure\": ";
    data += sensorValue;
    data += "}\r\n";

    data.toCharArray(resultChar, 255);
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
