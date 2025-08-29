#include "Player/Player.h"
#include "Application.h"
#include "Render/Camera.h"


void Player::handleCollisionHit(CollisionObject* collisionObject, const Vector2& collisionNormal)
{
    if (!collisionObject && collisionNormal == Vector2{.x = 0, .y = -1}) //means we have hit the windowBorder
    {
        bDead = true;
        setCanMove(false);
        setColor({.r = 1, .g = 0, .b = 0});
        return;
    }

    Rectangle::handleCollisionHit(collisionObject, collisionNormal);
    mVelocity *= 0.8f;

    if (collisionNormal.y < 0.f)
    {
        mVelocity.y = std::min(-mMinJumpVelocity, mVelocity.y);
    }

}

Vector2 Player::getViewLocation() const
{
    return mCamera->getCameraLocation();
}

Player::Player(const Vector2& size, const Vector2& position) : Rectangle(size.x, size.y)
{
    setCollisionCategory(CollisionCategory::Player);
    mDampingPerSecond = {5.f, 0.f};
    setLocation(position);

    Vector2 cameraOffset = Application::getApplication().getWindowSize();
    cameraOffset.x *= -0.5f;
    cameraOffset.y *= -0.2f;
    
    mCamera = std::make_unique<Camera>(cameraOffset, this);

    constexpr bool bWindowXCollide = true;
    constexpr bool bWindowYCollide = true;
    setCanCollideWithWindowBorder(bWindowXCollide, bWindowYCollide);
}

void Player::handleKeyPressed(const SDL_Scancode scancode)
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
    default: ;
    }
}

void Player::handleKeyTrigger(const SDL_Scancode scancode, float deltaTime)
{
    if (scancode == SDL_SCANCODE_S || scancode == SDL_SCANCODE_DOWN)
    {
        mInputMovement.y = mVelocity.y > 0;
    }
}

void Player::handleKeyReleased(const SDL_Scancode scancode)
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
    case SDL_SCANCODE_S:
    case SDL_SCANCODE_DOWN:
        mInputMovement.y = 0;
    default: ;
    }
}

void Player::tick(const float deltaTime)
{
    if (bDead) return;
    
    const float deltaMovementX = mInputMovement.x * mSpeed;
    const float deltaMovementY = mInputMovement.y * mSpeed;

    const Vector2 deltaMovement {.x = deltaMovementX, .y = deltaMovementY};
    mVelocity += deltaMovement;

    mCollisionResponseConfig[CollisionCategory::Ground] = mVelocity.y < 0.f ? CollisionResponse::Overlap : CollisionResponse::Block; 

    Rectangle::tick(deltaTime);
    InputReceiverInterface::tick(deltaTime);
}