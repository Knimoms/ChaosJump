#include "Physics/CollisionObject.h"

#include <cassert>

#include "Physics/CollisionShapeInterface.h"

std::vector<CollisionObject*> CollisionObject::sCollisionObjects = {};

CollisionObject::CollisionObject()
{
    sCollisionObjects.push_back(this);
}

CollisionObject::~CollisionObject()
{
    const auto it = std::ranges::find(sCollisionObjects, this);
    assert(it != sCollisionObjects.end());

    sCollisionObjects.erase(it);
}

void CollisionObject::setCollisionShape(CollisionShapeInterface* inCollisionShape)
{
    mCollisionShape = std::unique_ptr<CollisionShapeInterface>(inCollisionShape);
}

void CollisionObject::tick(float deltaTime)
{
    moveTick(deltaTime);
}

void CollisionObject::moveTick(float deltaTime)
{
    const Vector2 newLocation = mVelocity * deltaTime + mLocation;

    CollisionObject* collidedObject = nullptr;
    CollisionResult result;

    for (CollisionObject* collisionObject : sCollisionObjects)
    {
        if (collisionObject == this) continue;
        
        result = mCollisionShape->isCollidingWithShapeAtLocation(newLocation, collisionObject->getCollisionShape(), collisionObject->getLocation());
        if (result.bCollided)
        {
            collidedObject = collisionObject;
            break;
        }
    }


    if (!result.bCollided)
    {
        mLocation = newLocation;
        return;
    }

    const float newVelocity = ((mMass - collidedObject->mMass) * mVelocity.x + 2 * collidedObject->mMass * collidedObject->mVelocity.x) / (mMass + collidedObject->mMass);
    const float otherVelocity = ((collidedObject->mMass - mMass) * collidedObject->mVelocity.x + 2 * mMass * mVelocity.x) / (mMass + collidedObject->mMass);

    setVelocity({newVelocity, 0});
    collidedObject->setVelocity({otherVelocity, 0});
}

void CollisionObject::setLocation(const Vector2& inLocation)
{
    mLocation = inLocation;
}

void CollisionObject::setVelocity(Vector2 inVelocity)
{
    mVelocity = inVelocity;
}

void CollisionObject::setMass(float mass)
{
    mMass = mass;
}
