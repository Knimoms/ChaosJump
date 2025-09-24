#pragma once
#include <string>

class ReplicationInterface
{

private:

    uint8_t mNetGUID;
    
public:

    virtual std::string serialize() = 0;
    virtual void deserialize(std::string serialized) = 0;
    
};
