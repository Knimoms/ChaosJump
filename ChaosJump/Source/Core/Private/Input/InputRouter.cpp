#include "Input/InputRouter.h"

#include "Application.h"
#include "Input/InputReceiverInterface.h"

void InputRouter::routeKeyEvent(const SDL_Scancode scancode, const bool pressed)
{
    for (InputReceiverInterface* inputReceiver : mAddedInputReceivers)
    {
        mInputReceivers.push_back(inputReceiver);
    }

    for (InputReceiverInterface* inputReceiver : mRemovedInputReceivers)
    {
        std::erase(mInputReceivers, inputReceiver);
    }
    
    for (InputReceiverInterface* inputReceiver : mInputReceivers)
    {
        if (inputReceiver)
        {
            inputReceiver->handleKeyInput(scancode, pressed);
        }
    }
}

void InputRouter::addInputReceiver(InputReceiverInterface* inputReceiver)
{
    mAddedInputReceivers.push_back(inputReceiver);
}

void InputRouter::removeInputReceiver(InputReceiverInterface* inputReceiver)
{
    auto it = std::ranges::find(mInputReceivers, inputReceiver);

    if (it != mInputReceivers.end()) return;

    *it = nullptr;
    mRemovedInputReceivers.push_back(inputReceiver);
}
