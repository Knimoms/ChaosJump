#include "Input/InputReceiver.h"

void IInputReceiver::handleKeyInput(const SDL_Scancode scancode, const bool bPressed)
{
    if (bPressed)
    {
        if (!mKeyDownMap[scancode])
        {
            handleKeyPressed(scancode);
        }

        handleKeyTrigger(scancode);
    }
    else
    {
        handleKeyReleased(scancode);
    }
    
    mKeyDownMap[scancode] = bPressed;
}

void IInputReceiver::handleKeyPressed(const SDL_Scancode scancode)
{
}

void IInputReceiver::handleKeyTrigger(const SDL_Scancode scancode)
{
}

void IInputReceiver::handleKeyReleased(const SDL_Scancode scancode)
{
}
