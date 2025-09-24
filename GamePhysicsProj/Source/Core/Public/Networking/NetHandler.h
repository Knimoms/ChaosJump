#pragma once

#include <vector>

#include "SteamSDK/public/steam/steam_api.h"

class NetHandler
{

private:

    const int mVirtualPort = 1;
    
    HSteamListenSocket mListenSocket;
    HSteamNetPollGroup mPollGroup;


    bool bHosting = false;
    std::vector<HSteamNetConnection> mClientConnections = {};

    
    bool bConnectedAsClient = false;
    HSteamNetConnection mServerConnection;
    uint64_t mLastHeartbeat = 0;

protected:

    STEAM_CALLBACK_MANUAL(NetHandler, handleConnStatusChanged, SteamNetConnectionStatusChangedCallback_t, mCallbackConnStatusChanged);
    STEAM_CALLBACK(NetHandler, handleGameLobbyJoinRequested, GameLobbyJoinRequested_t, m_GameLobbyJoinRequested);
    STEAM_CALLBACK(NetHandler, handleGameRichPresenceJoinRequested, GameRichPresenceJoinRequested_t, m_GameRichPresenceJoinRequested);
    
public:

    NetHandler();

    void host();
    void connect();

    void receiveMessages() const;

    void runCallbacks();
    
};