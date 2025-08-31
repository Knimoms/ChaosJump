#pragma once
#include <stack>
#include <vector>

class TickableInterface
{

private:

    static bool bTickInProgress;
    static std::stack<TickableInterface*> sTickablesCreatedInTick;
    static std::vector<TickableInterface*> sTickables;
    friend class Application;

public:

    TickableInterface();
    virtual ~TickableInterface();

    virtual void tick(float deltaTime) = 0;
    
};
