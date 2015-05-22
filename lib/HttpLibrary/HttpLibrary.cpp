/*
 * Copyright 2015 Tobias Wilken
 */

#include <HttpLibrary.h>
#include <Arduino.h>
#include <Logging.h>

int parse_response(RedFlyClient client) {
    int a = 0;
    int c;
    unsigned int len = 0;
    char data[1024];
    data[len] = 0;
    int i = 0;
    int max = 10000;
    for (i = 0; i < max; i++) {
        if (client.available()) {
            Log.Info("client.available"CR);
            do {
                c = client.read();
                if ((c != -1) && (len < (sizeof(data) - 1))) {
                    data[len++] = c;
                }
            } while (c != -1);
            data[len] = 0;
            a = data[169] - '0';
            break;
        }

        if (!client.connected()) {
            Log.Info("client.disconnected"CR);
            break;
        }
        delay(10);
    }
    if (i == max) {
        Log.Info("!!! Iteration limit reached."CR);
    }
    client.stop();
    Log.Info("Response: %d"CR, a);
    return a;
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
