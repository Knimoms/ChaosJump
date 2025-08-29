#pragma once
#include <memory>

#include "Core/TickableInterface.h"
#include "Math/Vector2.h"

class CollisionObject;

class Camera : public TickableInterface
{

private:

    Vector2 mCameraLocation = {};
    Vector2 mFollowOffset = {};

    CollisionObject* mFollowingObject = nullptr;

public:

    Vector2 getCameraLocation() const { return mCameraLocation + mFollowOffset; }

    Camera(const Vector2& inFollowOffset, CollisionObject* inFollowingObject);

    void tick(float deltaTime) override;
    
};
