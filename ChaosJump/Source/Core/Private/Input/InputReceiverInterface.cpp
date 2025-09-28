#include "Input/InputReceiverInterface.h"

#include "Input/InputRouter.h"

void InputReceiverInterface::handleKeyInput(const SDL_Scancode scancode, const bool bPressed)
{
    const bool bWasPressed = mKeyDownMap[scancode]; 
    mKeyDownMap[scancode] = bPressed;
    if (bPressed)
    {
        if (!bWasPressed)
        {
            handleKeyPressed(scancode);
        }
    }
    else
    {
        handleKeyReleased(scancode);
    }
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
