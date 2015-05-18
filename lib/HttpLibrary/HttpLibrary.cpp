/*
 * Copyright 2015 Tobias Wilken
 */

#include <HttpLibrary.h>
#include <Arduino.h>
#include <Logging.h>

unsigned int dataPos = 0;
int return_int = 0;

char c = '\0';
ParseState parseState = ReadProtocol;

unsigned int protocolPos = 0;
char protocol[10];
const char* protocolPrefix = "HTTP/*.* ";
const char* protocolPtr = protocolPrefix;
unsigned int statusMessagePos = 0;
char statusMessage[20];
int statusCode = 0;
char headerline[100];
int headerlinePos = 0;
int contentLength = 0;


void readProtocol() {
    if ((*protocolPtr == '*') || (*protocolPtr == c)) {
        protocol[protocolPos++] = c;
        protocolPtr++;
        if (*protocolPtr == '\0') {
            parseState = ReadStatusCode;
            protocol[protocolPos++] = '\0';
            Log.Verbose("Protocol %d"CR, protocol);
        }
    }
}

int readStatusCode() {
    if (isdigit(c)) {
        statusCode = statusCode * 10 + (c - '0');
    } else {
        if (statusCode != 200) {
            Log.Error("Status code wrong: %d"CR, statusCode);
            return 1;
        }
        parseState = ReadStatusMessage;
        Log.Info("Status %d"CR, statusCode);
    }
    return 0;
}

void readStatusMessage() {
    if (c == '\n') {
        statusMessage[statusMessagePos++] = '\0';
        Log.Verbose("Message %s"CR, statusMessage);
        parseState = ReadHeader;
        return;
    }
    statusMessage[statusMessagePos++] = c;
}

void readHeader() {
    if (c == '\n') {
        if (headerlinePos == 1) {
            Log.Debug("Empty line found - finishing headers"CR);
            // Properly readout content-length
            contentLength = 2;
            parseState = ReadData;
            return;
        }
        headerlinePos = 0;
        return;
    }

    headerline[headerlinePos++] = c;
}

int readData() {
    Log.Info("ReadData %s"CR, c);
    return_int = c - '0';
    parseState = Done;
}

int receive_data(RedFlyClient client) {
    dataPos = 0;
    return_int = 0;
    c = '\0';
    parseState = ReadProtocol;
    protocolPos = 0;
    protocolPtr = protocolPrefix;
    statusMessagePos = 0;
    statusCode = 0;
    headerlinePos = 0;
    contentLength = 0;

    do {
        c = client.read();
        if (c != -1) {
            switch (parseState) {
            case ReadProtocol:
                readProtocol();
                break;
            case ReadStatusCode:
                if (readStatusCode()) {
                    return 0;
                }
                break;
            case ReadStatusMessage:
                readStatusMessage();
                break;
            case ReadHeader:
                readHeader();
                break;
            case ReadData:
                readData();
                break;
            case Done:
                Log.Debug("Done"CR);
                break;
            }
        } else {
            Log.Error("client.read %c"CR, c);
        }
    } while (c != -1);
    return return_int;
}

int parse_response(RedFlyClient client) {
    int i = 0;
    int max = 1000;
    int wait_for_response = 100;

    for (i = 0; i < max; i++) {
        Log.Info(".");
        if (client.available()) {
            Log.Info(""CR);
            Log.Info("client.available"CR);
            return receive_data(client);
        }

        if (!client.connected()) {
            Log.Info(""CR);
            Log.Error("client.disconnected"CR);
            return 0;
        }
        delay(wait_for_response);
    }
    Log.Info(""CR);
    Log.Error("Iteration limit reached"CR);
    return 0;
}

void get_request_data(char* name,
        char* hostname, int sensorValue, char* resultChar) {
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
