#include "Networking/NetHandler.h"

#include <chrono>
#include <format>
#include <ranges>
#include <string>

#include "Debugging/DebugDefinitions.h"

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

NetPacket::NetPacket(uint8_t inType, const std::string& inBody) : body(inBody)
{
    header.type = inType;
    header.size = body.size();
    header.timestamp = SteamNetworkingUtils()->GetLocalTimestamp();
    
}

NetPacket::NetPacket(const void* data, const int size)
{
    if (!ensure(size > sizeof(Header))) return;
    
    memcpy(&header, data, sizeof(Header));

    if (!ensure(size > sizeof(Header) + header.size)) return;
    
    const char* bodycstring = static_cast<const char*>(data) + sizeof(Header);
    body.assign(bodycstring, header.size);
}

std::string NetPacket::toString() const
{
    std::string packageString;
    packageString.resize(sizeof(header) + body.size());

    memcpy(packageString.data(), &header, sizeof(header));
    memcpy(packageString.data() + sizeof(header), body.data(), body.size());

    return packageString;
}

void NetHandler::handleConnStatusChanged(SteamNetConnectionStatusChangedCallback_t* pParam)
{
    fprintf(stderr, "Connection changed %d", pParam->m_info.m_eState);

    auto* sockets = SteamNetworkingSockets();
    
    switch (pParam->m_info.m_eState) {
    case k_ESteamNetworkingConnectionState_Connecting:
        sockets->AcceptConnection(pParam->m_hConn);
        sockets->SetConnectionPollGroup(pParam->m_hConn, mPollGroup);
        mClientConnections.push_back(pParam->m_hConn);
        {
            const char hello[] = "hello whats up";
            sockets->SendMessageToConnection(pParam->m_hConn, hello, static_cast<int>(strlen(hello)), k_nSteamNetworkingSend_Reliable, nullptr);
        }
        
        printf("Client connected!\n");
        break;
    case k_ESteamNetworkingConnectionState_ClosedByPeer:
    case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
        sockets->CloseConnection(pParam->m_hConn, 0, nullptr, false);
        std::erase(mClientConnections, pParam->m_hConn);
        
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
    mListenSocket = SteamNetworkingSockets()->CreateListenSocketP2P(mVirtualPort, 0, nullptr);
    mPollGroup = SteamNetworkingSockets()->CreatePollGroup();

    SteamFriends()->SetRichPresence("connect", std::to_string(SteamUser()->GetSteamID().ConvertToUint64()).c_str());

    bHosting = true;
}

void NetHandler::connect()
{
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
            if (!ensure(messagesNum >= 0)) { break; }

            for (int i = 0; i < messagesNum; ++i) {
                SteamNetworkingMessage_t* m = msgs[i];
                const void* data = m->m_pData;
                int len = m->m_cbSize;
                
                std::string msg(static_cast<char*>(m->m_pData), m->m_cbSize);
                fprintf(stderr, "Received %s\n", msg.c_str());

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
            for (const auto& connection : mClientConnections)
            {
                const char* msg = "test";
                SteamNetworkingSockets()->SendMessageToConnection(connection, msg, static_cast<int>(strlen(msg)), k_nSteamNetworkingSend_Unreliable, nullptr);
                mLastHeartbeat = now;
            }
        }
    }
}
