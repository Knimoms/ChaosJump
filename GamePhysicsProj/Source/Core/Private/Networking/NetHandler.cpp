#include "Networking/NetHandler.h"

#include <ranges>
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
    bConnectedAsClient = true;
}

void NetHandler::handleGameRichPresenceJoinRequested(GameRichPresenceJoinRequested_t* pParam)
{
    CSteamID host = pParam->m_steamIDFriend;
    SteamNetworkingIdentity id;
    id.SetSteamID(host);

    mServerConnection = SteamNetworkingSockets()->ConnectP2P(id, mVirtualPort, 0, nullptr);
    bConnectedAsClient = true;
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

    mServerConnection = SteamNetworkingSockets()->ConnectByIPAddress(serverAddr, 0, nullptr);
    bConnectedAsClient = true;
}

void NetHandler::receiveMessages() const
{
    if (bHosting)
    {
        SteamNetworkingMessage_t* msgs[64];
        for (;;) {
            int n = SteamNetworkingSockets()->ReceiveMessagesOnPollGroup(mPollGroup, msgs, 64);
            if (n == 0) break;          // no more messages right now
            if (n < 0) { /* error */ break; }

            for (int i = 0; i < n; ++i) {
                auto* m = msgs[i];
                HSteamNetConnection from = m->m_conn;
                const void* data = m->m_pData;
                int len = m->m_cbSize;

                std::string msg(static_cast<char*>(m->m_pData), m->m_cbSize);
                fprintf(stderr, "Received %s\n", msg.c_str());
                m->Release();
            }
        }
    }
    else if (bConnectedAsClient)
    {
        SteamNetworkingMessage_t* msgs[32];
        for (;;) {
            const int messagesNum = SteamNetworkingSockets()->ReceiveMessagesOnConnection(mServerConnection, msgs, 32);
            if (messagesNum == 0) break;
            if (messagesNum < 0) { break; }

            for (int i = 0; i < messagesNum; ++i) {
                auto* m = msgs[i];
                const void* data = m->m_pData;
                int len = m->m_cbSize;

                // HandleServerMessage(data, len);

                m->Release();
            }
        }
    }
}

void NetHandler::runCallbacks()
{
    SteamAPI_RunCallbacks();

    if (bConnectedAsClient)
    {
        uint64_t now = SteamNetworkingUtils()->GetLocalTimestamp();

        if (now - mLastHeartbeat > 5000000) {
            const char* msg = "ping";
            SteamNetworkingSockets()->SendMessageToConnection(mServerConnection, msg, static_cast<int>(strlen(msg)), k_nSteamNetworkingSend_Unreliable, nullptr);
            mLastHeartbeat = now;
        }
    }
    else if (bHosting)
    {
        uint64_t now = SteamNetworkingUtils()->GetLocalTimestamp();

        if (now - mLastHeartbeat > 5000000)
        {
            for (const auto& connection : mConnectionMap | std::views::values)
            {
                const char* msg = "test";
                SteamNetworkingSockets()->SendMessageToConnection(connection, msg, static_cast<int>(strlen(msg)), k_nSteamNetworkingSend_Unreliable, nullptr);
                mLastHeartbeat = now;
            }
        }
    }
}
