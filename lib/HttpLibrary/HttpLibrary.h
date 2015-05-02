#include <RedFlyClient.h>

typedef enum {
        ReadProtocol,
        ReadStatusCode,
        ReadStatusMessage,
        ReadHeader,
        ReadData
} ParseState;

int parse_response(RedFlyClient client);
