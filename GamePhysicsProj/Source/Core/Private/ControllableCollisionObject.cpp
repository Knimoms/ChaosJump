#include "ControllableCollisionObject.h"

ControllableCollisionObject::ControllableCollisionObject(const SDL_FRect rect)// : CollisionObject(rect)
{
}

void ControllableCollisionObject::handleKeyInput(const SDL_Scancode scancode, const bool bPressed)
{
    switch (scancode)
    {
    case SDL_SCANCODE_A:
    case SDL_SCANCODE_LEFT:
        mInputMovement.x -= bPressed ? 1 : -1;
        break;
    case SDL_SCANCODE_D:
    case SDL_SCANCODE_RIGHT:
        mInputMovement.x += bPressed ? 1 : -1;
        break;
    case SDL_SCANCODE_W:
    case SDL_SCANCODE_UP:
        mInputMovement.y += bPressed ? 1 : -1;
        break;
    case SDL_SCANCODE_S:
    case SDL_SCANCODE_DOWN:
        mInputMovement.y -= bPressed ? 1 : -1;
        break;
    default: ;
    }
}

void ControllableCollisionObject::tickInput(float deltaTime)
{
    const float deltaMovementX = mInputMovement.x * mSpeed;
    const float deltaMovementY = mInputMovement.y * mSpeed;

    SDL_FPoint deltaMovement(deltaMovementX, deltaMovementY);
    //move(deltaMovement );
}
