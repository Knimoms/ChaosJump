#pragma once
#include "Physics/CollisionObject.h"
#include "Input/InputReceiver.h"
#include "SDL3/SDL_rect.h"

class ControllableCollisionObject : public CollisionObject, public IInputReceiver
{

private:

    float mSpeed = 3.f;

    SDL_FPoint mInputMovement = { 0, 0 };
    
public:
    
    ControllableCollisionObject(SDL_FRect rect);
    void handleKeyInput(SDL_Scancode scancode, bool bPressed) override;
    void tickInput(float deltaTime) override;
    
};
