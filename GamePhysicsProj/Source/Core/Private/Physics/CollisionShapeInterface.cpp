#include "Physics/CollisionShapeInterface.h"

CollisionResult CollisionResult::getInverted(CollisionObject* other) const
{
    CollisionResult invert = *this;
    invert.collisionObject = other;
    invert.collisionNormal *= -1;

    return invert;
}

CollisionShapeInterface::CollisionShapeInterface(uint8_t inCollisionShapeID) : mCollisionShapeTypeID(inCollisionShapeID)
{
}

void CollisionShapeInterface::setOwner(CollisionObject* owner)
{
    mOwner = owner;
}