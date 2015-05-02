#include <HttpLibrary.h>
#include <Arduino.h>
#include <Output.h>

int receive_data(RedFlyClient client) {
    int response = 0;
    char data[1024];
    unsigned int len = 0;
    data[len] = 0;

    char c = '\0';
    ParseState state = ReadProtocol;

    int protocolPos = 0;
    char protocol[10];
    const char* protocolPrefix = "HTTP/*.* ";
    const char* protocolPtr = protocolPrefix;
    int statusMessagePos = 0;
    char statusMessage[20];
    int statusCode = 0;
    char headerline[100];
    int headerlinePos = 0;
    const char* contentlengthPrefix = "Content-Length:";
    const char* contentlengthPtr = contentlengthPrefix;
    bool contentlengthcheck = false;

    do {
        c = client.read();
        if (c != -1) {
            switch (state) {
            case ReadProtocol:
                if ((*protocolPtr == '*') || (*protocolPtr == c)) {
                    protocol[protocolPos++] = c;
                    protocolPtr++;
                    if (*protocolPtr == '\0') {
                        state = ReadStatusCode;
                        protocol[protocolPos++] = '\0';
                        log("Protocol: ");
                        log(String(protocol));
                        logln("");
                    }
                }
                break;
            case ReadStatusCode:
                if (isdigit(c)) {
                    statusCode = statusCode * 10 + (c - '0');
                } else {
                    state = ReadStatusMessage;
                    log("Status: ");
                    log(String(statusCode));
                    logln("");
                }
                break;
            case ReadStatusMessage:
                if (c == '\n') {
                    statusMessage[statusMessagePos++] = '\0';
                    log("Message: ");
                    log(String(statusMessage));
                    logln("");
                    state = ReadHeader;
                    break;
                }
                statusMessage[statusMessagePos++] = c;
                break;
            case ReadHeader:
//                logln(String(c));
                if (c == '\n') {
                    logln(String(headerlinePos));
                    if (headerlinePos == 1) {
                        logln("Emptylinefound");
                        state = ReadData;
                        break;
                    }
                    headerline[headerlinePos++] = '\0';
                    log("Header line: ");
                    log(String(headerline));
                    logln("");
                    headerlinePos = 0;
                    break;
                }

                if (*contentlengthPtr == c) {
                    //TODO check if the field matches contentlength, store value and use for reading data
                }

                headerline[headerlinePos++] = c;
                break;
            case ReadData:
                logln(String(c));
            }

//            if (len < (sizeof(data) - 1)) {
//                data[len++] = c;
//            } else {
//                log("len < sizeof(data) -1: ");
//                log(String(len));
//                log(" < ");
//                log(String(sizeof(data) - 1));
//                logln("");
//                c = -1;
//            }
        } else {
            logln("Got -1");
        }
    } while (c != -1);
//    logln(String(data));
//    data[len] = 0;
//    response = data[168] - '0';

    return response;
}

int parse_response(RedFlyClient client) {
    int response = 0;
    int i = 0;
    int max = 1000;

    for (i = 0; i < max; i++) {
        log(".");
        if (client.available()) {
            logln("client.available");
            response = receive_data(client);
            break;
        }

        if (!client.connected()) {
            logln("client.disconnected");
            break;
        }
        delay(1);
    }
    if (i == max) {
        logln("!!! Iteration limit reached.");
    }
    return response;
}
