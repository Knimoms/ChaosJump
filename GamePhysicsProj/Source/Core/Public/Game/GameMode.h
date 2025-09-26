#pragma once
#include <vector>

#include "Player.h"
#include "Networking/SerializableInterface.h"
#include "SteamSDK/public/steam/steamnetworkingtypes.h"

class GameMode : public SerializableInterface
{

private:

    std::vector<HSteamNetConnection> mJoinedConnections = {0};
    Player* mLocalPlayer = nullptr;

protected:

    bool bGameInProgress = false;

public:

    GameMode();

    std::vector<HSteamNetConnection> getJoinedConnections() const { return mJoinedConnections; }

    virtual void handleConnectionJoined(HSteamNetConnection connection);
    virtual void handleConnectionLeft(HSteamNetConnection connection);

    Player* getLocalPlayer() const { return mLocalPlayer; }
    virtual void setLocalPlayer(Player* inLocalPlayer);
};
