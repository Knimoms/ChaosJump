#pragma once
#include <vector>

#include "SDL3/SDL_scancode.h"

class InputReceiverInterface;

class InputRouter
{

private:

    std::vector<InputReceiverInterface*> mInputReceivers;
    std::vector<InputReceiverInterface*> mAddedInputReceivers;
    std::vector<InputReceiverInterface*> mRemovedInputReceivers;
    
public:

    void routeKeyEvent(SDL_Scancode scancode, bool pressed);

    void addInputReceiver(InputReceiverInterface* inputReceiver);
    void removeInputReceiver(InputReceiverInterface* inputReceiver);
    
};
