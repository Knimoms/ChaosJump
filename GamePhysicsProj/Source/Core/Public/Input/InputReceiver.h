#pragma once
#include <map>

#include "SDL3/SDL_scancode.h"

class IInputReceiver
{

    std::map<SDL_Scancode, bool> mKeyDownMap = {};
    
public:

    void handleKeyInput(const SDL_Scancode scancode, const bool bPressed);
    
    virtual void handleKeyPressed(const SDL_Scancode scancode);
    virtual void handleKeyTrigger(const SDL_Scancode scancode);
    virtual void handleKeyReleased(const SDL_Scancode scancode);
    
    virtual ~IInputReceiver() = default;
    
};
