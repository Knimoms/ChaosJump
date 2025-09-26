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

    if (newOwningConnection)
    {
        const NetPacket packet(OBJECTOWNERSHIPGRANTED, this, {});
        NetHandler::sendPacketToConnection(packet, newOwningConnection);
    }

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

void SerializableInterface::callOnDestroy()
{
    mOnDestroyDelegate(this);
}
