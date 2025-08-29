#include "Core/TickableInterface.h"

std::vector<TickableInterface*> TickableInterface::sTickables = {};

TickableInterface::TickableInterface()
{
    sTickables.push_back(this);
}

TickableInterface::~TickableInterface()
{
    const auto it = std::ranges::find(sTickables, this);
    sTickables.erase(it);
}