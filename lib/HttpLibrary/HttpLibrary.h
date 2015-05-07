#include <RedFlyClient.h>

typedef enum {
        ReadProtocol,
        ReadStatusCode,
        ReadStatusMessage,
        ReadHeader,
        ReadData
} ParseState;

int parse_response(RedFlyClient client);
void get_request_data(char* name, char* hostname, int sensorValue, char* resultChar);
