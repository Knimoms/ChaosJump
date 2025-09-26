#pragma once
#include <string>

#define DECLARE_TYPE_REGISTER(Class) \
    struct Class##TypeRegister \
    { \
        Class##TypeRegister(); \
    }; \
    static Class##TypeRegister TypeRegister; \
    static uint8_t mTypeID;

#define DEFINE_TYPE_REGISTER(Class, TypeID, ...) \
    uint8_t Class##::mTypeID = TypeID; \
    Class::Class##TypeRegister::Class##TypeRegister() \
    { \
        NetHandler::registerTypeID(TypeID, [](){ return std::make_unique<Class>(__VA_ARGS__); }); \
    } \
    Class::Class##TypeRegister Class::TypeRegister;

class SerializableInterface
{

private:

    uint32_t mNetGUID = 0;

    bool bOwnedLocally = false;

    friend class NetHandler;

protected:

    void setOwnership(bool inLocallyOwned);
    
public:

    SerializableInterface();
    virtual ~SerializableInterface();

    virtual std::string serialize() const = 0;
    virtual void deserialize(std::string serialized) = 0;

    virtual bool isNetGUIDValid() const { return mNetGUID; }

};
