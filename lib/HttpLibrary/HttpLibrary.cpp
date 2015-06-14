/*
 * Copyright 2015 Tobias Wilken
 */

#include <HttpLibrary.h>
#include <Arduino.h>
#include <Logging.h>

ParseState parseState;

unsigned int protocolPos;
char protocol[10];
const char* protocolPrefix = "HTTP/*.* ";
const char* protocolPtr;

int statusCode;

unsigned int statusMessagePos;
char statusMessage[20];

int headerlinePos;

int return_int;

// unsigned int dataPos = 0;
//
// char c = '\0';
//

// char headerline[100];
// int contentLength = 0;
//
//
void readProtocol(char c) {
    if ((*protocolPtr == '*') || (*protocolPtr == c)) {
        protocol[protocolPos++] = c;
        protocolPtr++;
        if (*protocolPtr == '\0') {
            parseState = ReadStatusCode;
            protocol[protocolPos++] = '\0';
            Log.Verbose("Protocol %s"CR, protocol);
        }
    }
}

int readStatusCode(char c) {
    if (isdigit(c)) {
        statusCode = statusCode * 10 + (c - '0');
    } else {
        if (statusCode != 200) {
            Log.Error("Status code wrong: %d"CR, statusCode);
            parseState = ReadStatusMessage;
            return 1;
        }
        parseState = ReadStatusMessage;
//        Log.Info("Status %d"CR, statusCode);
    }
    return 0;
}

void readStatusMessage(char c) {
    if (c == '\n') {
//        statusMessage[statusMessagePos++] = '\0';
        // Don't know, but this line does not work
//        Log.Verbose("Message %s"CR, statusMessage);
//        Serial.print("Message ");
//        Serial.println(statusMessage);
        parseState = ReadHeader;
        return;
    }
//    statusMessage[statusMessagePos++] = c;
}

void readHeader(char c) {
    if (c == '\n') {
        if (headerlinePos == 1) {
//            Log.Debug("Empty line found - finishing headers"CR);
//            // Properly readout content-length
//            contentLength = 2;
            parseState = ReadData;
            return;
        }
        headerlinePos = 0;
        return;
    }
    headerlinePos++;
//
//    headerline[headerlinePos++] = c;
}
//
int readData(char c) {
//    Log.Info("ReadData %c"CR, c);
    return_int = c - '0';
    parseState = Done;
}

int read_parts(char c) {
    switch (parseState) {
    case ReadProtocol:
        readProtocol(c);
        break;
    case ReadStatusCode:
        if (readStatusCode(c)) {
            return 0;
        }
        break;
    case ReadStatusMessage:
        readStatusMessage(c);
        break;
    case ReadHeader:
        readHeader(c);
        break;
    case ReadData:
        readData(c);
        break;
    case Done:
//        Log.Debug("Done"CR);
        return return_int;
        break;
    }
    return -1;
}

int receive_data(RedFlyClient client) {
    char c;
    return_int = 0;

    do {
        c = client.read();
        if (read_parts(c) != -1) {
            break;
        }
    } while (c != -1);
    if (parseState != Done) {
        Log.Error("Parsing did not reach done state: %d"CR, parseState);
    }
    Serial.println("Response");
    Serial.println(return_int);
    return return_int;
}

int parse_response(RedFlyClient client) {
    parseState = ReadProtocol;
    protocolPtr = protocolPrefix;
    protocolPos = 0;
    protocol[0] = '\0';
    statusCode = 0;
    statusMessagePos = 0;
    statusMessage[0] = '\0';
    headerlinePos = 0;

    int i = 0;
    int max = 10000;
    int wait_for_response = 10;

    for (i = 0; i < max; i++) {
        Log.Info(".");
        if (client.available()) {
            Log.Info(""CR);
            Log.Info("client.available"CR);
            return receive_data(client);
        }

        if (!client.connected()) {
            Log.Info(""CR);
            Log.Info("client.disconnected"CR);
            return 0;
        }
        delay(wait_for_response);
    }
    Log.Info(""CR);
    Log.Info("!!! Iteration limit reached."CR);
    return 0;
}

void get_request_data(char* name, char* hostname, int sensorValue,
        char* resultChar) {
    String data = "POST /sensors/";
    data += name;
    data += "/points/ HTTP/1.1\r\nHost:";
    data += hostname;
    data += "\r\nContent-Type: application/json";
    data += "\r\nContent-length: 16\r\n\r\n{\"measure\": ";
    data += sensorValue;
    data += "}\r\n";

    data.toCharArray(resultChar, 255);
}
