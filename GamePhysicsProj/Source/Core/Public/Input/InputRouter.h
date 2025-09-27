#pragma once
#include <vector>

#include "SDL3/SDL_scancode.h"

class InputReceiverInterface;

class InputRouter
{

private:

    std::vector<InputReceiverInterface*> mInputReceivers;
    
public:

    void routeKeyEvent(SDL_Scancode scancode, bool pressed) const;

    void addInputReceiver(InputReceiverInterface* inputReceiver);
    void removeInputReceiver(InputReceiverInterface* inputReceiver);
    
};
