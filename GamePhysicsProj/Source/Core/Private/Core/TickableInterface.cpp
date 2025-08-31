#include "Core/TickableInterface.h"

#include "Application.h"

std::stack<TickableInterface*> TickableInterface::sTickablesCreatedInTick = {};
std::vector<TickableInterface*> TickableInterface::sTickables = {};
bool TickableInterface::bTickInProgress = false;

TickableInterface::TickableInterface()
{
    if (bTickInProgress)
    {
        sTickablesCreatedInTick.push(this);
    }
    else
    {
        sTickables.push_back(this);
    }
}

TickableInterface::~TickableInterface()
{
    const auto it = std::ranges::find(sTickables, this);
    if (it != sTickables.end())
    {
        *it = nullptr;
    }
}