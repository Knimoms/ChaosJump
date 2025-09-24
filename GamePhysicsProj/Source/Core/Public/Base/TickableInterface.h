#pragma once
#include <stack>
#include <vector>

class TickableInterface
{

private:

    uint8_t mTickOrder = UINT8_MAX;

    static bool bTickInProgress;
    static std::stack<TickableInterface*> sTickablesCreatedInTick;
    static std::vector<TickableInterface*> sTickables;
    friend class Application;

public:

    TickableInterface();
    virtual ~TickableInterface();

    virtual void tick(float deltaTime) = 0;
    
};
