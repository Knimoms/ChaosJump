#include "Input/InputReceiverInterface.h"

#include "Input/InputRouter.h"

void InputReceiverInterface::handleKeyInput(const SDL_Scancode scancode, const bool bPressed)
{
    if (bPressed)
    {
        if (!mKeyDownMap[scancode])
        {
            handleKeyPressed(scancode);
        }
    }
    else
    {
        handleKeyReleased(scancode);
    }
    
    mKeyDownMap[scancode] = bPressed;
}

void InputReceiverInterface::handleKeyPressed(const SDL_Scancode scancode)
{
}

void InputReceiverInterface::handleKeyTrigger(const SDL_Scancode scancode, const float deltaTime)
{
}

void InputReceiverInterface::handleKeyReleased(const SDL_Scancode scancode)
{
}

void InputReceiverInterface::tick(float deltaTime)
{
    for (const auto [scanCode, bPressed]: mKeyDownMap)
    {
        if (bPressed)
        {
            handleKeyTrigger(scanCode, deltaTime);
        }
    }
}
