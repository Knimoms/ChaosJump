#pragma once
#include <vector>

#include "Networking/SerializableInterface.h"
#include "SteamSDK/public/steam/steamnetworkingtypes.h"

class GameMode : public SerializableInterface
{

private:

    std::vector<HSteamNetConnection> mJoinedConnections;

protected:

    bool bGameInProgress = false;

public:

    GameMode();

    std::vector<HSteamNetConnection> getJoinedConnections() const { return mJoinedConnections; }

    virtual void handleConnectionJoined(HSteamNetConnection connection);
    virtual void handleConnectionLeft(HSteamNetConnection connection);
    
};
