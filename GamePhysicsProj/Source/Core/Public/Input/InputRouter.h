#pragma once
#include <memory>
#include <vector>

#include "SDL3/SDL_scancode.h"

class IInputReceiver;

class InputRouter
{

private:

    std::vector<std::shared_ptr<IInputReceiver>> mInputReceivers;
    
public:

    void routeKeyEvent(SDL_Scancode scancode, bool pressed) const;
    
};
