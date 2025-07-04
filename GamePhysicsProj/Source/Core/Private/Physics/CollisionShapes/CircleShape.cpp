#include "Physics/CollisionShapes/CircleShape.h"

#include <cassert>

CircleShape::CircleShape(float radius) : mRadius(radius)
{
}

CollisionResult CircleShape::isCollidingWithShapeAtLocation(Vector2 shapeLocation, const CollisionShapeInterface* otherShape, Vector2 otherLocation)
{
    assert(otherShape);
    
    CollisionResult result;
    if (const CircleShape* otherCircle = dynamic_cast<const CircleShape*>(otherShape))
    {
        const Vector2 deltaLocation = otherLocation - shapeLocation;
        const float sqrdDistance = deltaLocation.squaredSize();

        const float radiiLength = mRadius + otherCircle->mRadius;
        const float srqdRadiiLength = radiiLength * radiiLength;

        if (srqdRadiiLength > sqrdDistance)
        {
            result.bCollided = true;
            result.collisionNormal = deltaLocation.getNormalized();
        }
    }

    return result;
}