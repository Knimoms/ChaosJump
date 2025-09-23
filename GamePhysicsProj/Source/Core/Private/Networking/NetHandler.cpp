#include "Networking/NetHandler.h"

#include "SteamSDK/public/steam/steam_api.h"

NetHandler::NetHandler()
{
    SteamAPI_Init();
    SteamNetworkingUtils()->InitRelayNetworkAccess();

    SteamNetworkingUtils()->SetDebugOutputFunction(k_ESteamNetworkingSocketsDebugOutputType_Msg,
    [](ESteamNetworkingSocketsDebugOutputType, const char* msg){
        fprintf(stderr, "[SNS] %s\n", msg);
    });
}

void NetHandler::HostLocal()
{
    SteamNetworkingIPAddr addr; addr.Clear();
    addr.m_port = 27020; // any free port

    // Listen for P2P on a "virtual port" (just a number you choose)
    const int kVirtualPort = 1;
    auto listenSocket = SteamNetworkingSockets()->CreateListenSocketIP(addr, 0, nullptr);

    HSteamNetPollGroup poll = SteamNetworkingSockets()->CreatePollGroup();
    SteamNetworkingMessage_t* msgs[32];

    while (true) {
        
        int n = SteamNetworkingSockets()->ReceiveMessagesOnPollGroup(poll, msgs, 32);
        for (int i = 0; i < n; ++i) {
            auto* m = msgs[i];
            
            SteamNetworkingSockets()->SendMessageToConnection(m->m_conn, m->m_pData, m->m_cbSize, k_nSteamNetworkingSend_Unreliable, nullptr);
            m->Release();
        }

        SteamAPI_RunCallbacks(); 
    }
}

void NetHandler::ConnectLocalInstance()
{
    SteamNetworkingIPAddr server;
    server.ParseString("127.0.0.1");
    server.m_port = 27020;
    auto conn = SteamNetworkingSockets()->ConnectByIPAddress(server, 0, nullptr);
}
