#include <HttpLibrary.h>
#include <Arduino.h>
#include <Output.h>

char data[1024];
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
            logBegin("Protocol", String(protocol));
        }
    }
}

int readStatusCode() {
    if (isdigit(c)) {
        statusCode = statusCode * 10 + (c - '0');
    } else {
        if (statusCode != 200) {
            logBegin("Status code wrong", String(statusCode));
            return 1;
        }
        parseState = ReadStatusMessage;
        logBegin("Status", String(statusCode));
    }
    return 0;
}

void readStatusMessage() {
    if (c == '\n') {
        statusMessage[statusMessagePos++] = '\0';
        logBegin("Message", String(statusMessage));
        parseState = ReadHeader;
        return;
    }
    statusMessage[statusMessagePos++] = c;

}

void readHeader() {
    if (c == '\n') {
        if (headerlinePos == 1) {
            logln("Empty line found - finishing headers");
            parseState = ReadData;
            return;
        }
        headerlinePos = 0;
        // Properly readout content-length
        contentLength = 2;
        return;
    }

    headerline[headerlinePos++] = c;
}

int readData() {
    logBegin("ReadData", String(c));
    return_int = c - '0';
//    logln(String(return_int));
    return return_int;
    if (contentLength > 1) {
        logln(String(c));
        data[dataPos++] = c;
        contentLength--;
    } else {
        data[dataPos++] = '\0';
        parseState = Done;
    }
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
                return readData();
                break;
            case Done:
                logln("Done");
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
            logln("");
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


void get_request_data(char* name, char* hostname, int sensorValue, char* resultChar) {
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
