#ifndef CLIENT_SENDMESSAGE_GLOBAL_H
#define CLIENT_SENDMESSAGE_GLOBAL_H

#include <client_global.h>

struct mbClientSendMessageParams
{
    int func;
    uint16_t offset;
    uint16_t count;
    uint16_t value;
    uint16_t writeOffset;
    uint16_t writeCount;
    mb::Format format;
    QString data;
};


#endif // CLIENT_SENDMESSAGE_GLOBAL_H
