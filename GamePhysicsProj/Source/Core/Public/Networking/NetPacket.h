#pragma once
#include <string>

enum EReservedTypes : uint8_t
{
    INVALID = 0,
    HEARTBEAT = 1,
    MESSAGE = 2,
    REMOTEPROCEDURECALL = 3
};

struct NetPacket
{
    struct Header
    {
        uint8_t type;
        uint8_t netGUID;
        uint32_t size;
        uint64_t timestamp;
    };
    
    Header header;
    std::string body;

    NetPacket(uint8_t inType, const std::string& inBody);
    NetPacket(const void* data, int size);
    std::string toString() const;
};
