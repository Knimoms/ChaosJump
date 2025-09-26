#include "Game/GameMode.h"

GameMode::GameMode()
{
}

void GameMode::handleConnectionJoined(HSteamNetConnection connection)
{
    mJoinedConnections.push_back(connection);
}

void GameMode::handleConnectionLeft(HSteamNetConnection connection)
{
    std::erase(mJoinedConnections, connection);
}
