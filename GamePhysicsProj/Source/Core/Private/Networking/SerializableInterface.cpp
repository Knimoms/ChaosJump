#include "Networking/SerializableInterface.h"

#include "Application.h"
#include "Networking/NetHandler.h"

SerializableInterface::~SerializableInterface()
{
    callOnDestroy();
}

void SerializableInterface::transferOwnershipToConnection(HSteamNetConnection newOwningConnection)
{
    if (!isLocallyOwned()) return;

    if (newOwningConnection)
    {
        const NetPacket replicatePacket(this);
        const NetPacket transferOwnerPacket(OBJECTOWNERSHIPGRANTED, this, {});
        NetHandler::sendPacketToConnection(replicatePacket, newOwningConnection);
        NetHandler::sendPacketToConnection(transferOwnerPacket, newOwningConnection);
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
    if (mOnDestroyDelegate)
    {
        mOnDestroyDelegate(this);
    }
}
