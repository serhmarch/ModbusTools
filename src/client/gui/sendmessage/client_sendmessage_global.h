#ifndef CLIENT_SENDMESSAGE_GLOBAL_H
#define CLIENT_SENDMESSAGE_GLOBAL_H

#include <client_global.h>

struct mbClientSendMessageParams
{
    mbClientSendMessageParams()
    {
        func = MBF_READ_HOLDING_REGISTERS;
        offset = 0;
        count = 0;
        writeOffset = 0;
        writeCount = 0;
        format = mb::Dec16;
    }

    int func;
    uint16_t offset;
    uint16_t count;
    uint16_t writeOffset;
    uint16_t writeCount;
    mb::Format format;
    QString data;

};


#endif // CLIENT_SENDMESSAGE_GLOBAL_H
