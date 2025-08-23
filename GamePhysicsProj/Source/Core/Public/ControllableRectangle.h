#pragma once
#include "Physics/CollisionObject.h"
#include "Input/InputReceiver.h"
#include "Objects/Rectangle.h"
#include "SDL3/SDL_rect.h"

class ControllableRectangle : public Rectangle, public IInputReceiver
{

private:

    float mSpeed = 1.f;

    SDL_FPoint mInputMovement = { 0, 0 };
    
public:
    
    ControllableRectangle(Vector2 size, Vector2 position);
    
    void handleKeyPressed(SDL_Scancode scancode) override;
    void handleKeyReleased(SDL_Scancode scancode) override;
    
    void tick(float delta_time) override;
    
};
