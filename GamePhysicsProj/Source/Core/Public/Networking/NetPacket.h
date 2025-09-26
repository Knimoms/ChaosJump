#pragma once
#include <string>

#include "SteamSDK/public/steam/steamnetworkingtypes.h"

class SerializableInterface;

enum EPacketTypes : uint8_t
{
    INVALID = 0,
    HEARTBEAT = 1,
    MESSAGE = 2,
    REMOTEPROCEDURECALL = 3,
    OBJECTDESTROY = 4,
    OBJECTUPDATE = 5
};

struct NetPacket
{
    struct Header
    {
        uint8_t type;
        uint8_t objectType;
        uint32_t netGUID;
        uint32_t size;
        uint64_t timestamp;
    };
    
    Header header;
    std::string body;

    NetPacket(uint8_t inType, const std::string& inBody);
    NetPacket(const SerializableInterface* object);
    NetPacket(uint8_t inType, const SerializableInterface* object, const std::string& inBody);
    NetPacket(const void* data, int size);
    std::string toString() const;
};
