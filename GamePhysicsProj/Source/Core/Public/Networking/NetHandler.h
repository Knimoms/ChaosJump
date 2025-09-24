#pragma once
#include <map>

#include "Base/TickableInterface.h"
#include "SteamSDK/public/steam/steam_api.h"

class NetHandler : public TickableInterface
{

private:

    const int mVirtualPort = 1;
    
    HSteamListenSocket mListenSocket;
    HSteamNetPollGroup mPollGroup;

    std::map<uint64_t, HSteamNetConnection> mConnectionMap = {};

    bool bHosting = false;
    bool bConnectedAsClient = false;

protected:

    STEAM_CALLBACK_MANUAL(NetHandler, handleConnStatusChanged, SteamNetConnectionStatusChangedCallback_t, mCallbackConnStatusChanged);
    STEAM_CALLBACK(NetHandler, handleGameLobbyJoinRequested, GameLobbyJoinRequested_t, m_GameLobbyJoinRequested);
    STEAM_CALLBACK(NetHandler, handleGameRichPresenceJoinRequested, GameRichPresenceJoinRequested_t, m_GameRichPresenceJoinRequested);
    
public:

    NetHandler();
    virtual ~NetHandler() = default;

    void host();
    void connect();

    //~ Begin TickableInterface
    void tick(float deltaTime) override;
    //~ End TickableInterface
    
};