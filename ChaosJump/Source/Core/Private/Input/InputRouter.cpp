#include "Input/InputRouter.h"

#include "Application.h"
#include "Input/InputReceiverInterface.h"

void InputRouter::routeKeyEvent(const SDL_Scancode scancode, const bool pressed)
{
    for (InputReceiverInterface* inputReceiver : mAddedInputReceivers)
    {
        mInputReceivers.push_back(inputReceiver);
    }

    mAddedInputReceivers.clear();

    for (InputReceiverInterface* inputReceiver : mRemovedInputReceivers)
    {
        std::erase(mInputReceivers, inputReceiver);
    }

    mRemovedInputReceivers.clear();
    
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
    std::erase(mAddedInputReceivers, inputReceiver);
    
    for (InputReceiverInterface*& receiver : mInputReceivers)
    {
        if (inputReceiver == receiver)
        {
            receiver = nullptr;
            mRemovedInputReceivers.push_back(inputReceiver);
            return;
        }
    }
}
