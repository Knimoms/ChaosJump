#pragma once
#include "Game/Player.h"
#include "Physics/CollisionObject.h"
#include "Networking/SerializableInterface.h"
#include "Objects/Polygon.h"
#include "SDL3/SDL_rect.h"

class Camera;

class ChaosJumpPlayer : public Polygon, public Player
{
    DECLARE_TYPE_REGISTER(ChaosJumpPlayer)

private:

    bool bDead = false;

    float mMinJumpVelocity = 500.f;

    float mSpeed = 5000.f;

    SDL_FPoint mInputMovement = { 0, 0 };

    std::shared_ptr<Camera> mCamera = nullptr;

protected:

    void handleCollisionHit(CollisionObject* collisionObject, const Vector2& collisionNormal) override;
    
public:

    Vector2 getViewLocation() const;
    bool isDead() const { return bDead; }
    
    ChaosJumpPlayer();
    ChaosJumpPlayer(const Vector2& size, const Vector2& position);
    
    void handleKeyPressed(SDL_Scancode scancode) override;
    void handleKeyTrigger(SDL_Scancode scancode, float deltaTime) override;
    void handleKeyReleased(SDL_Scancode scancode) override;

    // Begin TickableInterface
    void tick(float deltaTime) override;
    // End TickableInterface

    // Begin SerializableInterface
    std::string serialize() const override;
    void deserialize(std::string serialized) override;
    void setOwningConnection(HSteamNetConnection inOwningConnection) override;
    // End SerializableInterface
};
