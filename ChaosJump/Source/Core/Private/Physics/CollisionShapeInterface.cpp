#include "Physics/CollisionShapeInterface.h"

#include "Physics/CollisionObject.h"

CollisionResult CollisionResult::getInverted(CollisionObject* other) const
{
    CollisionResult invert = *this;

    CollisionObject* oldObject = invert.collisionObject;
    const CollisionResponse response = oldObject ? oldObject->getCollisionResponseForCategory(other->getCollisionCategory()) : CollisionResponse::Ignore;
    invert.bCollided = CollisionResponse::Ignore != response;
    invert.bBlocked = CollisionResponse::Block == response;
    invert.collisionObject = other;
    invert.collisionNormal *= -1 * invert.bCollided;

    return invert;
}

CollisionShapeInterface::CollisionShapeInterface(uint8_t inCollisionShapeID) : mCollisionShapeTypeID(inCollisionShapeID)
{
}

void CollisionShapeInterface::setOwner(CollisionObject* owner)
{
    mOwner = owner;
}