#pragma once

#include <functional>
#include <map>
#include <memory>
#include <unordered_set>
#include <vector>

#include "NetPacket.h"
#include "SteamSDK/public/steam/steam_api.h"

class SerializableInterface;

class NetHandler
{

private:

    bool bSteamInitialized = false;

    std::unordered_set<uint32_t> mUsedNetGUIDs;

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

    STEAM_CALLBACK_MANUAL(NetHandler, handleConnStatusChanged, SteamNetConnectionStatusChangedCallback_t, mCallbackConnStatusChanged);
    STEAM_CALLBACK(NetHandler, handleGameLobbyJoinRequested, GameLobbyJoinRequested_t, m_GameLobbyJoinRequested);
    STEAM_CALLBACK(NetHandler, handleGameRichPresenceJoinRequested, GameRichPresenceJoinRequested_t, m_GameRichPresenceJoinRequested);

    void replicateObject(const SerializableInterface* object) const;
    void replicateObjects() const;

    static SerializableInterface* createRemoteObject(uint8_t typeId, uint32_t uint32_t);
    void handleObjectNetPacket(const NetPacket& packet, HSteamNetConnection sendingConnection) const;
    void handleNetPacket(const NetPacket& packet, HSteamNetConnection sendingConnection) const;
    
    static void sendPacketToConnection(const NetPacket& packet, const HSteamNetConnection& connection);

public:

    NetHandler();

    bool initializeSteam();
    
    void host();

    void receiveMessages() const;
    void runCallbacks();

    void addNetworkObject(SerializableInterface* object);
    
    void removeNetworkObject(const SerializableInterface* object);
    
    static void registerTypeID(uint8_t typeID, std::function<std::unique_ptr<SerializableInterface>()>&& factoryFunction);

    uint32_t getFreeNetGUID() const;
};
