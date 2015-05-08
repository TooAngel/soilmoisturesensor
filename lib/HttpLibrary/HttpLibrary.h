/*
 * Copyright 2015 Tobias Wilken
 */

#ifndef LIB_HTTPLIBRARY_HTTPLIBRARY_H_
#define LIB_HTTPLIBRARY_HTTPLIBRARY_H_
    #include <RedFlyClient.h>

    typedef enum {
            ReadProtocol,
            ReadStatusCode,
            ReadStatusMessage,
            ReadHeader,
            ReadData,
            Done
    } ParseState;

    int parse_response(RedFlyClient client);
    void get_request_data(char* name,
            char* hostname, int sensorValue, char* resultChar);
#endif /* LIB_HTTPLIBRARY_HTTPLIBRARY_H_ */
