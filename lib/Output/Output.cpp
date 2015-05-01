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
