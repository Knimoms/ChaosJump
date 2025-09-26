#include "Networking/SerializableInterface.h"

#include "Application.h"
#include "Networking/NetHandler.h"

SerializableInterface::~SerializableInterface()
{
    mOnDestroyDelegate(this);
}

void SerializableInterface::setOwningConnection(HSteamNetConnection inOwningConnection)
{
    mOwningConnection = inOwningConnection;
}

void SerializableInterface::registerObject()
{
    Application::getApplication().getNetHandler()->addNetworkObject(this);
}
