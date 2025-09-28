#include "Player/ChaosJumpPlayer.h"
#include "Application.h"
#include "Debugging/DebugDefinitions.h"
#include "GameMode/ChaosJumpGameMode.h"
#include "Networking/NetHandler.h"
#include "Render/Camera.h"

DEFINE_TYPE_REGISTER(ChaosJumpPlayer, 5)

void ChaosJumpPlayer::handleCollisionHit(CollisionObject* collisionObject, const Vector2& collisionNormal)
{
    if (!collisionObject && collisionNormal == Vector2{.x = 0, .y = -1}) //means we have hit the windowBorder
    {
        setIsDead(true);
        setCanMove(false);
        return;
    }

    Polygon::handleCollisionHit(collisionObject, collisionNormal);
    mVelocity *= 0.8f;

    if (collisionNormal.y < 0.f)
    {
        mVelocity.y = std::min(-mMinJumpVelocity, mVelocity.y);
    }

}

void ChaosJumpPlayer::setIsDead(const bool bInDead)
{
    bDead = bInDead;
    setColor(bDead ? Color{.r = 1, .g = 0, .b = 0} : Color{.r = 0, .g = 1, .b = 0});
}

Vector2 ChaosJumpPlayer::getViewLocation() const
{
    return mCamera->getCameraLocation();
}

ChaosJumpPlayer::ChaosJumpPlayer() : ChaosJumpPlayer(Vector2{1, 1}, Vector2{0, 0})
{
}

ChaosJumpPlayer::ChaosJumpPlayer(const Vector2& size, const Vector2& position) : Polygon({size*Vector2{0.f,-50.f}, size*Vector2{-47.5528f,-15.4508f}, size*Vector2{-29.3893f,40.4508f}, size*Vector2{29.3893f,40.4508f}, size*Vector2{47.5528f,-15.4508f}})
{
    setCollisionCategory(CollisionCategory::Player);
    mDampingPerSecond = {5.f, 0.f};
    setLocation(position);
}

void ChaosJumpPlayer::handleKeyPressed(const SDL_Scancode scancode)
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
    case SDL_SCANCODE_S:
    case SDL_SCANCODE_DOWN:
        mInputMovement.y = 1;
        break;
    default: ;
    }
}

void ChaosJumpPlayer::handleKeyTrigger(const SDL_Scancode scancode, float deltaTime)
{
    //if (scancode == SDL_SCANCODE_S || scancode == SDL_SCANCODE_DOWN)
    //{
    //    mInputMovement.y = mVelocity.y > 0;
    //}
}

void ChaosJumpPlayer::handleKeyReleased(const SDL_Scancode scancode)
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

void ChaosJumpPlayer::reset()
{
    mReachedHeight = 0.f;
    setIsDead(false);
}

void ChaosJumpPlayer::tick(const float deltaTime)
{
    const float currentPlayerHeight = -getLocation().y - ChaosJumpGameMode::getPlayerSpawnLocation().y;
    mReachedHeight = std::max(mReachedHeight, currentPlayerHeight/100);

    if (!isLocallyOwned()) return;
    if (bDead) return;
    
    const float deltaMovementX = mInputMovement.x * mSpeed;
    const float deltaMovementY = mInputMovement.y * mSpeed;

    const Vector2 deltaMovement  = Vector2{.x = deltaMovementX, .y = deltaMovementY} * deltaTime;
    mVelocity += deltaMovement;

    mCollisionResponseConfig[CollisionCategory::Ground] = mVelocity.y < 0.f ? CollisionResponse::Overlap : CollisionResponse::Block; 

    Polygon::tick(deltaTime);
    InputReceiverInterface::tick(deltaTime);
}

std::string ChaosJumpPlayer::serialize() const  
{
    std::string serialized;
    serialized.resize(sizeof(mLocation) + sizeof(bDead));

    memcpy(serialized.data(), &mLocation, sizeof(mLocation));
    memcpy(serialized.data() + sizeof(mLocation), &bDead, sizeof(bDead));

    return serialized;
}

void ChaosJumpPlayer::deserialize(std::string serialized)
{
    if (!ensure(serialized.size() >= sizeof(Vector2))) return;

    memcpy(&mLocation, serialized.data(), sizeof(Vector2));
    
    bool bNewDead;
    memcpy(&bNewDead, serialized.data() + sizeof(Vector2), sizeof(bool));

    if (bDead != bNewDead)
    {
        setIsDead(bNewDead);
        setCanMove(!bNewDead);
    }
}

void ChaosJumpPlayer::setOwningConnection(HSteamNetConnection inOwningConnection)
{
    Player::setOwningConnection(inOwningConnection);    

    if (isLocallyOwned())
    {
        Application& app = Application::getApplication();

        Vector2 cameraOffset = app.getWindowSize();
        cameraOffset.x *= -0.5f;
        cameraOffset.y *= -0.2f;
         
        mCamera = std::make_shared<Camera>(cameraOffset, this);

        app.setRenderCamera(mCamera);
    }
    else
    {
        mCamera.reset();
    }

    const bool bWindowXCollide = isLocallyOwned();
    const bool bWindowYCollide = isLocallyOwned();
    setCanCollideWithWindowBorder(bWindowXCollide, bWindowYCollide);
}
