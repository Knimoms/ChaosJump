#include "Networking/SerializableInterface.h"

#include "Application.h"
#include "Networking/NetHandler.h"

void SerializableInterface::setOwnership(const bool inLocallyOwned)
{
    bOwnedLocally = inLocallyOwned;

    if (inLocallyOwned)
    {
        Application::getApplication().getNetHandler()->mLocallyReplicatedObjects.push_back(this);
    }
    else
    {
        std::erase(Application::getApplication().getNetHandler()->mLocallyReplicatedObjects, this);
    }
}

SerializableInterface::SerializableInterface()
{
    Application::getApplication().getNetHandler()->mNetworkObjects.push_back(this);
    setOwnership(bOwnedLocally);
}

SerializableInterface::~SerializableInterface()
{
    std::erase(Application::getApplication().getNetHandler()->mNetworkObjects, this);

    if (bOwnedLocally)
    {
        setOwnership(false);
    }
}