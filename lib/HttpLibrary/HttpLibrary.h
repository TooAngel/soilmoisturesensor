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
