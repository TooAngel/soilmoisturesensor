#include <HttpLibrary.h>
#include <Arduino.h>
#include <Output.h>

int receive_data(RedFlyClient client) {
    int response = 0;
    char data[1024];
    unsigned int len = 0;
    data[len] = 0;

    char c = '\0';
    ParseState state = Start;

    do {
        c = client.read();
        if (c != -1) {
            if (len < (sizeof(data) - 1)) {
                data[len++] = c;
            } else {
                log("len < sizeof(data) -1: ");
                log (String(len));log
                (" < ");
                log(String(sizeof(data) - 1));
                logln("");
                c = -1;
            }
        } else {
            logln("Got -1");
        }
    } while (c != -1);
    logln (String(data));data
    [len] = 0;
    response = data[168] - '0';
    return response;
}

int parse_response(RedFlyClient client) {
    int response = 0;
    int i = 0;
    int max = 10000;

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
