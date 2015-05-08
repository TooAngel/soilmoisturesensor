/*
 * Copyright 2015 Tobias Wilken
 */

#include <Arduino.h>

void log(String message) {
    if (Serial) {
        Serial.print(message);
    }
}

void logln(String message) {
    if (Serial) {
        Serial.println(message);
    }
}

void logError(String message, uint8_t ret) {
    log("ERROR ");
    log(message);
    log(": ");
    log(String(ret));
    logln("");
}

void logBegin(String one, String two) {
    log(one);
    log(": ");
    logln(two);
}

void logBegin(String one, String two, String three) {
    log(one);
    log(": ");
    log(two);
    log(", ");
    logln(three);
}

void logServerAndPort(String command, byte server[], int port) {
    log(command);
    log(" (");
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
}
