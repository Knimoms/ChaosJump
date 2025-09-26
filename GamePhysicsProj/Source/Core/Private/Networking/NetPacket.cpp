#include "Networking/NetPacket.h"

#include "Debugging/DebugDefinitions.h"
#include "Networking/SerializableInterface.h"
#include "SteamSDK/public/steam/isteamnetworkingutils.h"

NetPacket::NetPacket(uint8_t inType, const std::string& inBody) : body(inBody)
{
    header.type = inType;
    header.size = body.size();
    header.timestamp = SteamNetworkingUtils()->GetLocalTimestamp();
}

NetPacket::NetPacket(const uint8_t inType, const SerializableInterface* object, const std::string& inBody) : NetPacket(inType, inBody)
{
    header.netGUID = object->getNetGUID();
    header.objectType = object->getTypeID();
}

NetPacket::NetPacket(const void* data, const int size)
{
    if (!ensure(size >= sizeof(Header))) return;
    
    memcpy(&header, data, sizeof(Header));

    if (!ensure(size >= sizeof(Header) + header.size)) return;
    
    const char* bodycstring = static_cast<const char*>(data) + sizeof(Header);
    body.assign(bodycstring, header.size);
}

std::string NetPacket::toString() const
{
    std::string packageString;
    packageString.resize(sizeof(header) + body.size());

    memcpy(packageString.data(), &header, sizeof(header));
    memcpy(packageString.data() + sizeof(header), body.data(), body.size());

    return packageString;
}