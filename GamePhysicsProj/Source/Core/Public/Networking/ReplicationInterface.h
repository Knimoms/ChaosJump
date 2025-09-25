#pragma once
#include <string>

class ReplicationInterface
{

private:

    uint8_t mNetGUID = 0;
    
public:
    virtual ~ReplicationInterface() = default;

    virtual std::string serialize() = 0;
    virtual void deserialize(std::string serialized) = 0;

    virtual bool IsNetGUIDValid() const { return mNetGUID; }
    
};
