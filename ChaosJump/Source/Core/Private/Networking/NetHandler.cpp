#include "Networking/NetHandler.h"

#include <chrono>
#include <string>

#include "Application.h"
#include "Debugging/DebugDefinitions.h"
#include "Networking/NetPacket.h"
#include "Networking/SerializableInterface.h"
#include "Player/ChaosJumpPlayer.h"

class NetFactory {
    
public:
    
    using FactoryFn = std::function<std::unique_ptr<SerializableInterface>()>;

private:
    
    std::unordered_map<uint8_t, FactoryFn> mFactories;

public:

    static NetFactory& getInstance() {
        static NetFactory factory;
        return factory;
    }

    void registerType(uint8_t id, FactoryFn&& factoryFunction) {
        mFactories[id] = std::move(factoryFunction);
    }

    std::unique_ptr<SerializableInterface> create(uint8_t id) const {
        const auto it = mFactories.find(id);
        if (it != mFactories.end())
        {
            return it->second();
        }
        
        return nullptr;
    }
};

void NetHandler::handleNetPacket(const NetPacket& packet, HSteamNetConnection sendingConnection) const
{
    switch (packet.header.type)
    {
    case INVALID:
        ensure(false);
        break;
    case HEARTBEAT:
        break;
    case MESSAGE:
        fprintf(stderr, "[MESSAGE] %s\n", packet.body.c_str());
        break;
    case REMOTEPROCEDURECALL:
        break;
    case OBJECTDESTROY:
    case OBJECTUPDATE:
    case OBJECTOWNERSHIPGRANTED:
        handleObjectNetPacket(packet, sendingConnection);
    }
}

void printHex(const void* data, size_t size, FILE* out = stdout) {
    auto* bytes = static_cast<const unsigned char*>(data);

    for (size_t i = 0; i < size; ++i) {
        if (i % 16 == 0)
            fprintf(out, "%04zx: ", i);   // print offset
        fprintf(out, "%02x ", bytes[i]);  // print byte as 2-digit hex
        if (i % 16 == 15 || i == size - 1)
            fprintf(out, "\n");
    }
}

static void logPacket(const NetPacket& packet, HSteamNetConnection receivingConnection)
{
    fprintf(stderr, "[OUT MESSAGE TO %u] %d, %u, %d, %u, %llu, %s\n",
        receivingConnection,
        packet.header.type,
        packet.header.netGUID,
        packet.header.objectType,
        packet.header.size,
        packet.header.timestamp,
        packet.body.c_str());
}

void NetHandler::sendPacketToConnection(const NetPacket& packet, const HSteamNetConnection& connection, const bool bReliable)
{
    const std::string msg = packet.toString();
    SteamNetworkingSockets()->SendMessageToConnection(connection, msg.data(), msg.size(), bReliable ? k_nSteamNetworkingSend_Reliable : k_nSteamNetworkingSend_Unreliable, nullptr);
}

bool NetHandler::initializeSteam()
{
    if (bSteamInitialized) return true;
    if (!SteamAPI_Init()) return false;

    bSteamInitialized = true;
    SteamNetworkingUtils()->InitRelayNetworkAccess();

    mCallbackConnStatusChanged.Register(this, &NetHandler::handleConnectionStatusChanged);

    SteamNetworkingUtils()->SetDebugOutputFunction(k_ESteamNetworkingSocketsDebugOutputType_Msg,
       [](ESteamNetworkingSocketsDebugOutputType, const char* msg)
       {
           fprintf(stderr, "[SNS] %s\n", msg);
       });
    
    return true;
}

NetHandler::NetHandler() : mListenSocket(0), mPollGroup(0), mServerConnection(0),
                           m_GameLobbyJoinRequested(this, &NetHandler::handleGameLobbyJoinRequested),
                           m_GameRichPresenceJoinRequested(this, &NetHandler::handleGameRichPresenceJoinRequested)
{
}

NetHandler::~NetHandler()
{
    for (SerializableInterface* object : mNetworkObjects)
    {
        object->mOnDestroyDelegate = nullptr;
    }
}

void NetHandler::handleConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pParam)
{
    auto* sockets = SteamNetworkingSockets();
    
    switch (pParam->m_info.m_eState) {
    case k_ESteamNetworkingConnectionState_Connecting:
        if (bHosting)
        {
            GameMode* gameMode = Application::getApplication().getGameMode();
            const std::string error = gameMode->handleJoiningConnection(pParam->m_hConn);

            if (error.empty())
            {
                sockets->AcceptConnection(pParam->m_hConn);
                sockets->SetConnectionPollGroup(pParam->m_hConn, mPollGroup);
    
                const NetPacket packet(MESSAGE, "hello whats up");
                sendPacketToConnection(packet, pParam->m_hConn);
            }
            else
            {
                sockets->CloseConnection(pParam->m_hConn, 1000, error.c_str(),true);
            }
        }

        break;
    case k_ESteamNetworkingConnectionState_ClosedByPeer:
    case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
        if (bHosting)
        {
            sockets->CloseConnection(pParam->m_hConn, 0, nullptr, false);
            std::erase(mClientConnections, pParam->m_hConn);
            Application::getApplication().getGameMode()->handleConnectionLeft(pParam->m_hConn);
            printf("Client disconnected or problem.\n");
        }
        else if (bConnectedAsClient)
        {
            Application::getApplication().getGameMode()->handleNetworkError();
            bConnectedAsClient = false;
        }
        break;
    case k_ESteamNetworkingConnectionState_Connected:
        mClientConnections.push_back(pParam->m_hConn);
        Application::getApplication().getGameMode()->handleConnectionJoined(pParam->m_hConn);
        break;
    default: ;
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
    Application::getApplication().getGameMode()->setOwningConnection(mServerConnection);
    bConnectedAsClient = true;
}

void NetHandler::replicateObject(const SerializableInterface* object) const
{
    if (bConnectedAsClient)
    {
        const NetPacket packet(object);
        sendPacketToConnection(packet, mServerConnection);
        return;
    }

    for (const HSteamNetConnection& connection: mClientConnections)
    {
        if (connection == object->getOwningConnection()) continue;
        
        const NetPacket packet(object);
        sendPacketToConnection(packet, connection);
    }
}

void NetHandler::replicateObjects() const
{
    const uint64_t now = SteamNetworkingUtils()->GetLocalTimestamp();
    if (now - mLastReplicateTimestamp <= mReplicationTickRateMilliseconds*1000) return;
    
    mLastReplicateTimestamp = now;
    for (const SerializableInterface* serializableObject : mNetworkObjects)
    {
        if (bConnectedAsClient && !serializableObject->isLocallyOwned()) continue;
        NetPacket packet(OBJECTUPDATE, serializableObject, serializableObject->serialize());
        replicateObject(serializableObject);
    }
}

SerializableInterface* NetHandler::createRemoteObject(uint8_t typeId, uint32_t netGUID) const
{
    std::unique_ptr remoteObject = NetFactory::getInstance().create(typeId);
    remoteObject->mNetGUID = netGUID;
    remoteObject->bRemotelyCreated = true;
    remoteObject->registerObject();
    SerializableInterface* object = remoteObject.get();
    mRemotelyCreatedObjects.push_back(std::move(remoteObject));
    return object;
}

void NetHandler::hostSession()
{
    if (bHosting) return;
    mListenSocket = SteamNetworkingSockets()->CreateListenSocketP2P(mVirtualPort, 0, nullptr);
    mPollGroup = SteamNetworkingSockets()->CreatePollGroup();

    const uint64 hostId64 = SteamUser()->GetSteamID().ConvertToUint64();
    const std::string connect = std::to_string(hostId64) + ":" + std::to_string(mVirtualPort);

    SteamFriends()->SetRichPresence("connect", connect.c_str());
    SteamFriends()->SetRichPresence("status", "Hosting"); 

    bHosting = true;
}

void NetHandler::closeSession()
{
    if (!bHosting) return;
    
    if (mListenSocket != k_HSteamListenSocket_Invalid)
    {
        SteamNetworkingSockets()->CloseListenSocket(mListenSocket);
        mListenSocket = k_HSteamListenSocket_Invalid;
    }

    for (const HSteamNetConnection connection : mClientConnections)
    {
        SteamNetworkingSockets()->CloseConnection(connection, 0, "Host shutting down", true);
    }
    
    mClientConnections.clear();

    SteamFriends()->ClearRichPresence();

    bHosting = false;
}

void NetHandler::closeServerConnection()
{
    SteamNetworkingSockets()->CloseConnection(mServerConnection, 0, "Client leaving.", true);
    bConnectedAsClient = false;
}

void NetHandler::openInviteDialogue() const
{
    if (!bHosting) return;
    
    const uint64 hostId = SteamUser()->GetSteamID().ConvertToUint64();
    const std::string connect = std::to_string(hostId) + ":" + std::to_string(mVirtualPort);
    SteamFriends()->ActivateGameOverlayInviteDialogConnectString(connect.c_str());}

void NetHandler::openFriendslist()
{
    SteamFriends()->ActivateGameOverlay("Friends");
}

void NetHandler::receiveMessages() const
{
    if (bHosting)
    {
        SteamNetworkingMessage_t* msgs[64];
        for (;;)
        {
            const int messagesNum = SteamNetworkingSockets()->ReceiveMessagesOnPollGroup(mPollGroup, msgs, 64);
            
            if (messagesNum == 0) break;
            if (!ensure(messagesNum >= 0)) break;
            
            for (int i = 0; i < messagesNum; ++i)
            {
                auto* message = msgs[i];
                const HSteamNetConnection from = message->m_conn;
                const void* data = message->m_pData;
                int len = message->m_cbSize;

                NetPacket packet(data, len);
                handleNetPacket(packet, from);
                message->Release();
            }
        }
    }
    else if (bConnectedAsClient)
    {
        SteamNetworkingMessage_t* msgs[32];
        for (;;)
        {
            const int messagesNum = SteamNetworkingSockets()->ReceiveMessagesOnConnection(mServerConnection, msgs, 32);

            if (messagesNum == 0) break;
            if (!ensure(messagesNum >= 0)) break;

            for (int i = 0; i < messagesNum; ++i)
            {

                auto* message = msgs[i];
                const HSteamNetConnection from = message->m_conn;
                const void* data = message->m_pData;
                const int len = message->m_cbSize;
                
                NetPacket packet(data, len);
                handleNetPacket(packet, from);
                
                message->Release();
            }
        }
    }
}

void NetHandler::handleObjectNetPacket(const NetPacket& packet, const HSteamNetConnection sendingConnection) const
{
    if (packet.header.type == OBJECTDESTROY)
    {
        std::erase_if(mRemotelyCreatedObjects, [&](std::unique_ptr<SerializableInterface>& obj)
        {
            return obj->mNetGUID == packet.header.netGUID;
        });
        return;
    }
    
    SerializableInterface* const object = [&]()
    {
        const auto it = std::ranges::find_if(mNetworkObjects, [&](SerializableInterface* obj)
        {
            return obj->mNetGUID == packet.header.netGUID;
        });

        if (it == mNetworkObjects.end())
        {
            SerializableInterface* remoteObject = createRemoteObject(packet.header.objectType, packet.header.netGUID);
            remoteObject->setOwningConnection(sendingConnection);

            return remoteObject;
        }

        return *it;
    }();

    if (!ensure(object)) return;

    if (packet.header.type == OBJECTUPDATE)
    {
        object->deserialize(packet.body);
    }
    else // if (packet.header.type == OBJECTOWNERSHIPGRANTED)
    {
        object->setOwningConnection(0);
    }
}

void NetHandler::runCallbacks()
{
    SteamAPI_RunCallbacks();

    if (bConnectedAsClient)
    {
        const uint64_t now = SteamNetworkingUtils()->GetLocalTimestamp();

        if (now - mLastHeartbeat > 5000000) {
            NetPacket packet(HEARTBEAT, "");
            sendPacketToConnection(packet, mServerConnection);
            mLastHeartbeat = now;
        }
    }

    if (bConnectedAsClient || bHosting)
    {
        replicateObjects();
    }
}

void NetHandler::addNetworkObject(SerializableInterface* object)
{
    if (!object->mNetGUID)
    {
        const uint32_t netGUID = getFreeNetGUID();
        object->mNetGUID = netGUID;
    }

    mUsedNetGUIDs.insert(object->mNetGUID);
    object->mOnDestroyDelegate = [this](const SerializableInterface* serializableObject)
    {
        if (bHosting)
        {
            const NetPacket packet(OBJECTDESTROY, serializableObject, {});
            for (HSteamNetConnection connection : mClientConnections)
            {
                constexpr bool bReliable = true;
                sendPacketToConnection(packet, connection, bReliable);
            }
        }
        
        removeNetworkObject(serializableObject);
    };

    mNetworkObjects.push_back(object);
}

void NetHandler::removeNetworkObject(const SerializableInterface* object)
{
    mUsedNetGUIDs.erase(object->mNetGUID);
    std::erase(mNetworkObjects, object);
}

void NetHandler::registerTypeID(uint8_t typeID, std::function<std::unique_ptr<SerializableInterface>()>&& factoryFunction)
{
    NetFactory::getInstance().registerType(typeID, std::move(factoryFunction));
}

uint32_t NetHandler::getFreeNetGUID() const
{
    for (uint32_t guid = 1; ; ++guid)
    {
        if (!mUsedNetGUIDs.contains(guid))
        {
            return guid;
        }
    }
}
