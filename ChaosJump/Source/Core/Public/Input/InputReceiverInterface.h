#pragma once
#include <map>

#include "SDL3/SDL_scancode.h"

class InputReceiverInterface
{
private:
    
    std::map<SDL_Scancode, bool> mKeyDownMap = {};

protected:
    
    /* needs to be called by a TickableInterface object to support handleKeyTrigger */
    void tick(float deltaTime);
    
public:

    void handleKeyInput(SDL_Scancode scancode, bool bPressed);
    
    virtual void handleKeyPressed(SDL_Scancode scancode);
    virtual void handleKeyReleased(SDL_Scancode scancode);
    virtual void handleKeyTrigger(SDL_Scancode scancode, float deltaTime);
    
    virtual ~InputReceiverInterface() = default;
    
};
