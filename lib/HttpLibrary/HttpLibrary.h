#include <RedFlyClient.h>

typedef enum {
        ReadProtocol,
        ReadStatusCode,
        ReadStatusMessage,
        ReadHeader
} ParseState;

int parse_response(RedFlyClient client);
