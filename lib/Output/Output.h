/*
 * Copyright 2015 Tobias Wilken
 */

#ifndef LIB_OUTPUT_OUTPUT_H_
#define LIB_OUTPUT_OUTPUT_H_
    void log(String message);
    void logln(String message);
    void logError(String message, uint8_t ret);
    void logServerAndPort(String command, byte server[], int port);
    void logBegin(String one, String two);
    void logBegin(String one, String two, String three);
#endif /* LIB_OUTPUT_OUTPUT_H_ */
