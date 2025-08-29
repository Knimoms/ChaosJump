#include "Input/InputRouter.h"

#include "Application.h"
#include "Input/InputReceiverInterface.h"

void InputRouter::routeKeyEvent(const SDL_Scancode scancode, const bool pressed) const
{
    for (InputReceiverInterface* inputReceiver : mInputReceivers)
    {
        inputReceiver->handleKeyInput(scancode, pressed);
    }
}

void InputRouter::addInputReceiver(InputReceiverInterface* inputReceiver)
{
    mInputReceivers.push_back(inputReceiver);
}
