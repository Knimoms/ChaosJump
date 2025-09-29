#pragma once
#include <functional>
#include <string>

#include "SteamSDK/public/steam/steamnetworkingtypes.h"

#define DECLARE_TYPE_REGISTER(Class) \
    struct Class##TypeRegister \
    { \
        Class##TypeRegister(); \
    }; \
    static Class##TypeRegister TypeRegister; \
    uint8_t getTypeID() const override;

#define DEFINE_TYPE_REGISTER(Class, TypeID, ...) \
    Class::Class##TypeRegister::Class##TypeRegister() \
    { \
        NetHandler::registerTypeID(TypeID, [](){ return std::make_unique<Class>(__VA_ARGS__); }); \
    } \
    Class::Class##TypeRegister Class::TypeRegister; \
    uint8_t Class::getTypeID() const { return TypeID; }

class SerializableInterface
{

protected:

    uint32_t mNetGUID = 0;

    HSteamNetConnection mOwningConnection;
    bool bRemotelyCreated = false;

    friend class NetHandler;

    using OnDestroyCallback = std::function<void(SerializableInterface*)>;
    OnDestroyCallback mOnDestroyDelegate;

public:

    virtual ~SerializableInterface();

    void transferOwnershipToConnection(HSteamNetConnection newOwningConnection);
    virtual void setOwningConnection(HSteamNetConnection inOwningConnection);

    virtual void handleRemoteObjectAboutToBeDestroyed();

    virtual std::string serialize() const = 0;
    virtual void deserialize(std::string serialized) = 0;
    
    void registerObject();
    
    virtual uint8_t getTypeID() const = 0;
    uint32_t getNetGUID() const { return mNetGUID; }
    
    HSteamNetConnection getOwningConnection() const { return mOwningConnection; }
    bool isLocallyOwned() const { return !mOwningConnection; }
    bool wasRemotelyCreated() const { return bRemotelyCreated; }
    
    void callOnDestroy();
};
