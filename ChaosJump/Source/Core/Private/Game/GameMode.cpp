#include "Game/GameMode.h"

GameMode::GameMode()
{
    mNetGUID = 200;
}

std::string GameMode::handleJoiningConnection(HSteamNetConnection connection)
{
    return "";
}

void GameMode::handleConnectionJoined(HSteamNetConnection connection)
{
    mJoinedConnections.push_back(connection);
}

void GameMode::handleConnectionLeft(HSteamNetConnection connection)
{
    std::erase(mJoinedConnections, connection);
}

void GameMode::addPlayer(Player* player)
{
    mPlayers.push_back(player);
}

void GameMode::removePlayer(Player* player)
{
    std::erase(mPlayers, player);
}