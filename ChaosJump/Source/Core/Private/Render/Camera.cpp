#include "Render/Camera.h"

#include "Physics/CollisionObject.h"

void Camera::setCameraHeight(const float inHeight)
{
    mCameraLocation.y = inHeight;
}

Camera::Camera(const Vector2& inFollowOffset, CollisionObject* inFollowingObject) : mFollowOffset(inFollowOffset), mFollowingObject(inFollowingObject)
{
}

void Camera::tick(const float deltaTime)
{
    if (!mFollowingObject) return;

    Vector2 followLocation = mFollowingObject->getLocation();
    mCameraLocation.y = std::min(followLocation.y, mCameraLocation.y);

}
