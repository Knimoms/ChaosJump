#include "Networking/NetHandler.h"

#include <string>

NetHandler::NetHandler() : m_GameLobbyJoinRequested(this, &NetHandler::handleGameLobbyJoinRequested), m_GameRichPresenceJoinRequested(this, &NetHandler::handleGameRichPresenceJoinRequested)
{
    SteamAPI_Init();
    SteamNetworkingUtils()->InitRelayNetworkAccess();

    mCallbackConnStatusChanged.Register(this, &NetHandler::handleConnStatusChanged);

    SteamNetworkingUtils()->SetDebugOutputFunction(k_ESteamNetworkingSocketsDebugOutputType_Msg,
                                                   [](ESteamNetworkingSocketsDebugOutputType, const char* msg)
                                                   {
                                                       fprintf(stderr, "[SNS] %s\n", msg);
                                                   });
}

void NetHandler::handleConnStatusChanged(SteamNetConnectionStatusChangedCallback_t* pParam)
{
    fprintf(stderr, "Connection changed %d", pParam->m_info.m_eState);

    auto* sockets = SteamNetworkingSockets();
    
    switch (pParam->m_info.m_eState) {
    case k_ESteamNetworkingConnectionState_Connecting:
        sockets->AcceptConnection(pParam->m_hConn);
        sockets->SetConnectionPollGroup(pParam->m_hConn, mPollGroup);
        printf("Client connected!\n");
        break;
    case k_ESteamNetworkingConnectionState_ClosedByPeer:
    case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
        sockets->CloseConnection(pParam->m_hConn, 0, nullptr, false);
        printf("Client disconnected or problem.\n");
        break;
    default: break;
    }
}

void NetHandler::handleGameLobbyJoinRequested(GameLobbyJoinRequested_t* pParam)
{
    SteamMatchmaking()->JoinLobby(pParam->m_steamIDLobby);
}

void NetHandler::handleGameRichPresenceJoinRequested(GameRichPresenceJoinRequested_t* pParam)
{
    CSteamID host = pParam->m_steamIDFriend;
    SteamNetworkingIdentity id;
    id.SetSteamID(host);

    mConnectionMap[pParam->m_steamIDFriend.ConvertToUint64()] = SteamNetworkingSockets()->ConnectP2P(id, mVirtualPort, 0, nullptr);
}

void NetHandler::host()
{
    SteamNetworkingIPAddr addr;
    addr.Clear();
    addr.m_port = 27020;

    constexpr int virtualPort = 1;
    mListenSocket = SteamNetworkingSockets()->CreateListenSocketP2P(virtualPort, 0, nullptr);
    mPollGroup = SteamNetworkingSockets()->CreatePollGroup();

    SteamFriends()->SetRichPresence("connect", std::to_string(SteamUser()->GetSteamID().ConvertToUint64()).c_str());

    bHosting = true;
}

void NetHandler::connect()
{
    SteamNetworkingIPAddr serverAddr;
    serverAddr.ParseString("127.0.0.1");
    serverAddr.m_port = 27020;

    HSteamNetConnection conn = SteamNetworkingSockets()->ConnectByIPAddress(serverAddr, 0, nullptr);
    bConnectedAsClient = true;
}

void NetHandler::tick(float deltaTime)
{
    SteamAPI_RunCallbacks();
}
