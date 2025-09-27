#pragma once
#include "Input/InputReceiverInterface.h"
#include "Networking/SerializableInterface.h"

class Player : public SerializableInterface, public InputReceiverInterface
{
public:

    //~ Begin SerializableInterface
    void setOwningConnection(HSteamNetConnection inOwningConnection) override;
    //~ End SerializableInterface
    
};
