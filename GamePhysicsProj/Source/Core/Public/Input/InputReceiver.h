#pragma once
#include "SDL3/SDL_scancode.h"

class IInputReceiver
{
    
public:

    virtual void handleKeyInput(const SDL_Scancode scancode, const bool bPressed) = 0;
    virtual void tickInput(float deltaTime) = 0;
    
    virtual ~IInputReceiver() = default;
    
};
