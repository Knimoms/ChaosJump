#pragma once
#include <string>

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

    virtual uint8_t getTypeID() const = 0;
};
