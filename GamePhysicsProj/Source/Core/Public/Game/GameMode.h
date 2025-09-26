#pragma once
#include <vector>

#include "SteamSDK/public/steam/steamnetworkingtypes.h"

class GameMode
{

private:

    std::vector<HSteamNetConnection> mJoinedConnections;
        
public:

    GameMode();

    std::vector<HSteamNetConnection> getJoinedConnections() const { return mJoinedConnections; }

    virtual void handleConnectionJoined(HSteamNetConnection connection);
    virtual void handleConnectionLeft(HSteamNetConnection connection);
    
};
