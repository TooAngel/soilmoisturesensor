#include <HttpLibrary.h>
#include <Arduino.h>
#include <Output.h>

int receive_data(RedFlyClient client) {
    char data[1024];
    unsigned int dataPos = 0;
    int return_int = 0;

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
                        return 0;
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
                logln(String(c));
                return_int = c - '0';
//                logln(String(return_int));
                return return_int;
                if (contentLength > 1) {
                    logln(String(c));
                    data[dataPos++] = c;
                    contentLength--;
                } else {
                    data[dataPos++] = '\0';
                    state = Done;
                }
                break;
            case Done:
                logln("Done");
//                logln(String(data));
//                return String(data);
                return 0;
            }
        } else {
            logln("Got -1");
        }
    } while (c != -1);
    return 0;
}

int parse_response(RedFlyClient client) {
//    String response = "0";
    int response_value = 0;
    int i = 0;
    int max = 1000;

    for (i = 0; i < max; i++) {
        log(".");
        if (client.available()) {
            logln("client.available");
            response_value = receive_data(client);
            logln("Data received");
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
//    log("Response string: ");
//    logln(response);
//    int response_value = response.toInt();
//    logln("Response int: ");
//    logln(String(response_value));
    return response_value;
}
