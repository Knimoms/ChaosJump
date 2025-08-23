#include "Input/InputRouter.h"
#include "Input/InputReceiver.h"

void InputRouter::routeKeyEvent(const SDL_Scancode scancode, const bool pressed) const
{
    for (const std::shared_ptr<IInputReceiver>& inputReceiver : mInputReceivers)
    {
        inputReceiver->handleKeyInput(scancode, pressed);
    }
}

void InputRouter::addInputReceiver(std::shared_ptr<IInputReceiver> inputReceiver)
{
    mInputReceivers.push_back(inputReceiver);
}
