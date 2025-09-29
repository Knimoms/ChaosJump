#pragma once

#include <functional>
#include <memory>
#include <unordered_set>
#include <vector>

#include "NetPacket.h"
#include "Networking/SerializableInterface.h"
#include "SteamSDK/public/steam/steam_api.h"

struct RemoteObjectDeleter
{
    void operator()(SerializableInterface* serializableObject) const;
};

class NetHandler
{

private:

    bool bSteamInitialized = false;

    std::unordered_set<uint32_t> mUsedNetGUIDs;

    mutable std::vector<std::unique_ptr<SerializableInterface, RemoteObjectDeleter>> mRemotelyCreatedObjects;
    std::vector<SerializableInterface*> mNetworkObjects;

    const int mVirtualPort = 1;
    
    HSteamListenSocket mListenSocket;
    HSteamNetPollGroup mPollGroup;

    bool bHosting = false;
    std::vector<HSteamNetConnection> mClientConnections = {};

    bool bConnectedAsClient = false;
    HSteamNetConnection mServerConnection;
    uint64_t mLastHeartbeat = 0;

    const uint64_t mReplicationTickRateMilliseconds = 4;
    mutable uint64_t mLastReplicateTimestamp = 0;

protected:

    STEAM_CALLBACK_MANUAL(NetHandler, handleConnectionStatusChanged, SteamNetConnectionStatusChangedCallback_t, mCallbackConnStatusChanged);
    STEAM_CALLBACK(NetHandler, handleGameRichPresenceJoinRequested, GameRichPresenceJoinRequested_t, m_GameRichPresenceJoinRequested);

    void replicateObject(const SerializableInterface* object) const;
    void replicateObjects() const;

    SerializableInterface* createRemoteObject(uint8_t typeId, uint32_t uint32_t) const;
    void handleObjectNetPacket(const NetPacket& packet, HSteamNetConnection sendingConnection) const;
    void handleNetPacket(const NetPacket& packet, HSteamNetConnection sendingConnection) const;

public:
    
    static void sendPacketToConnection(const NetPacket& packet, const HSteamNetConnection& connection, bool bReliable = false);

    NetHandler();
    virtual ~NetHandler();

    bool initializeSteam();
    
    void hostSession();
    void closeSession();
    void closeServerConnection();
    
    void openInviteDialogue() const;
    static void openFriendslist();
    
    bool isHosting() const { return bHosting; }
    bool isConnectedAsClient() const { return bConnectedAsClient; }

    void receiveMessages() const;
    void runCallbacks();

    void addNetworkObject(SerializableInterface* object);
    
    void removeNetworkObject(const SerializableInterface* object);
    
    static void registerTypeID(uint8_t typeID, std::function<std::unique_ptr<SerializableInterface>()>&& factoryFunction);

    uint32_t getFreeNetGUID() const;
};
