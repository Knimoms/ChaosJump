#include "ControllableRectangle.h"
#include "Application.h"

ControllableRectangle::ControllableRectangle(Vector2 size, Vector2 position) : Rectangle(size.x, size.y)
{
    mDampingPerSecond = 5.f;
    setLocation(position);
}

void ControllableRectangle::handleKeyPressed(SDL_Scancode scancode)
{
    switch (scancode)
    {
    case SDL_SCANCODE_A:
    case SDL_SCANCODE_LEFT:
        mInputMovement.x -= 1;
        break;
    case SDL_SCANCODE_D:
    case SDL_SCANCODE_RIGHT:
        mInputMovement.x += 1;
        break;
    case SDL_SCANCODE_W:
    case SDL_SCANCODE_UP:
        mInputMovement.y -= 1;
        break;
    case SDL_SCANCODE_S:
    case SDL_SCANCODE_DOWN:
        mInputMovement.y += 1;
        break;
    default: ;
    }
}

void ControllableRectangle::handleKeyReleased(SDL_Scancode scancode)
{
    switch (scancode)
    {
    case SDL_SCANCODE_A:
    case SDL_SCANCODE_LEFT:
        mInputMovement.x -= -1;
        break;
    case SDL_SCANCODE_D:
    case SDL_SCANCODE_RIGHT:
        mInputMovement.x += -1;
        break;
    case SDL_SCANCODE_W:
    case SDL_SCANCODE_UP:
        mInputMovement.y -= -1;
        break;
    case SDL_SCANCODE_S:
    case SDL_SCANCODE_DOWN:
        mInputMovement.y += -1;
        break;
    default: ;
    }
}

void ControllableRectangle::tick(float delta_time)
{
    const float deltaMovementX = mInputMovement.x * mSpeed;
    const float deltaMovementY = mInputMovement.y * mSpeed;

    const Vector2 deltaMovement {.x = deltaMovementX, .y = deltaMovementY};
    mVelocity += deltaMovement;
    std::string output = deltaMovement.toString();

    DebugLine debugLine;
    debugLine.color = {0, 0, 1};
    debugLine.duration = 0.01f;
    debugLine.start = getLocation();
    debugLine.end = debugLine.start + mVelocity;
    Application::getApplication().addDebugLine(debugLine);

    Rectangle::tick(delta_time);

    //OutputDebugStringA(output.c_str());
}
