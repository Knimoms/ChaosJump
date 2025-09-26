#include "Game/GameMode.h"

GameMode::GameMode()
{
    mNetGUID = 200;
}

void GameMode::handleConnectionJoined(HSteamNetConnection connection)
{
    mJoinedConnections.push_back(connection);
}

void GameMode::handleConnectionLeft(HSteamNetConnection connection)
{
    std::erase(mJoinedConnections, connection);
}

void GameMode::setLocalPlayer(Player* inLocalPlayer)
{
    mLocalPlayer = inLocalPlayer;
}
