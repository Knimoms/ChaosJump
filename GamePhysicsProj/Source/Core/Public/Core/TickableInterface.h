#pragma once
#include <vector>

class TickableInterface
{

public:

    static std::vector<TickableInterface*> sTickables;

    TickableInterface();
    virtual ~TickableInterface();

    virtual void tick(float deltaTime) = 0;
    
};
