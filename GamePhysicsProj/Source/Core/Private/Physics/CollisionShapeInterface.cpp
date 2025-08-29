#include "Physics/CollisionShapeInterface.h"

CollisionResult CollisionResult::getInverted(CollisionObject* other) const
{
    CollisionResult invert = *this;
    invert.collisionObject = other;
    invert.collisionNormal *= -1;

    return invert;
}

void CollisionShapeInterface::setOwner(CollisionObject* owner)
{
    mOwner = owner;
}
