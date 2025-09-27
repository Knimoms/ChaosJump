#pragma once
#include <vector>

#include "Player.h"
#include "Math/Vector2.h"
#include "Networking/SerializableInterface.h"
#include "SteamSDK/public/steam/steamnetworkingtypes.h"

class GameMode : public SerializableInterface
{

private:

    std::vector<HSteamNetConnection> mJoinedConnections = {0};
    std::vector<Player*> mPlayers;

protected:

    bool bGameInProgress = false;

public:

    GameMode();

    std::vector<HSteamNetConnection> getJoinedConnections() const { return mJoinedConnections; }

    virtual std::string handleJoiningConnection(HSteamNetConnection connection);
    virtual void handleConnectionJoined(HSteamNetConnection connection);
    virtual void handleConnectionLeft(HSteamNetConnection connection);

    virtual void addPlayer(Player* player);    
    virtual void removePlayer(Player* player);

    const std::vector<Player*>& getPlayers() const { return mPlayers; }
};
