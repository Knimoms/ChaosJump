#include "Networking/SerializableInterface.h"

#include "Application.h"
#include "Networking/NetHandler.h"

SerializableInterface::~SerializableInterface()
{
    mOnDestroyDelegate(this);
}

void SerializableInterface::transferOwnershipToConnection(HSteamNetConnection newOwningConnection)
{
    if (!isLocallyOwned()) return;

    NetPacket packet(OBJECTOWNERSHIPGRANTED, this, {});
    NetHandler::sendPacketToConnection(packet, newOwningConnection);

    setOwningConnection(newOwningConnection);
}

void SerializableInterface::setOwningConnection(HSteamNetConnection inOwningConnection)
{
    mOwningConnection = inOwningConnection;
}

void SerializableInterface::registerObject()
{
    Application::getApplication().getNetHandler()->addNetworkObject(this);
}
