#include <algorithm>

#include "Render/DrawableInterface.h"

std::vector<DrawableInterface*> DrawableInterface::sDrawables = {};

DrawableInterface::DrawableInterface(Color color) : mColor(color)
{
    sDrawables.push_back(this);
}

void DrawableInterface::setColor(Color color)
{
    mColor = color;
}

DrawableInterface::~DrawableInterface()
{
    auto it = std::ranges::find(sDrawables, this);
    sDrawables.erase(it);
}
