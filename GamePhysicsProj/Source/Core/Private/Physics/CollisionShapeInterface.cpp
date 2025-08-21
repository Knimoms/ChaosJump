#include "Physics/CollisionShapeInterface.h"

void CollisionShapeInterface::setOwner(CollisionObject* owner)
{
    mOwner = owner;
}