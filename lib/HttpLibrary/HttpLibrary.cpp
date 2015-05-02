#include <HttpLibrary.h>
#include <Arduino.h>
#include <Output.h>

String receive_data(RedFlyClient client) {
    char data[1024];
    unsigned int dataPos = 0;

    char c = '\0';
    ParseState state = ReadProtocol;

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
                    if (statusCode != 200) {
                        log("Status code wrong: ");
                        logln(String(statusCode));
                        return String("0");
                    }
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
                if (c == '\n') {
                    logln("Newlinefound");
                    logln(String(headerlinePos));
                    if (headerlinePos == 1) {
                        logln("Empty line found - finishing headers");
                        state = ReadData;
                        break;
                    }
                    headerlinePos = 0;
                    // Properly readout content-length
                    contentLength = 2;
                    break;
                }

                headerline[headerlinePos++] = c;
                break;
            case ReadData:
                log("ReadData: ");
                logln(String(contentLength));
                if (contentLength > 0) {
                    logln(String(c));
                    data[dataPos++] = c;
                } else {
                    data[dataPos++] = '\0';
                    state = Done;
                }
            case Done:
                logln("Done");
                return String(data);
            }
        } else {
            logln("Got -1");
        }
    } while (c != -1);
    return String(data);
}

int parse_response(RedFlyClient client) {
    String response = 0;
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
        delay(10);
    }
    if (i == max) {
        logln("!!! Iteration limit reached.");
    }
    return response.toInt();
}
