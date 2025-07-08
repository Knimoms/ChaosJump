#include "Physics/CollisionShapes/CircleShape.h"

#include <cassert>

#include "Application.h"

CircleShape::CircleShape(float radius) : mRadius(radius)
{
}

CollisionResult CircleShape::isCollidingWithShapeAtLocation(const Vector2& shapeLocation, const CollisionShapeInterface* otherShape, const Vector2& otherLocation)
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
            result.collisionObject = otherCircle->GetOwner();
            result.bCollided = true;
            result.collisionNormal = deltaLocation.getNormalized();
        }
    }

    return result;
}

CollisionResult CircleShape::isCollidingWithWindowBorderAtLocation(const Vector2& shapeLocation, const Vector2& windowSize)
{
    CollisionResult result;

    if (shapeLocation.x + mRadius > windowSize.x)
    {
        result = {.bCollided = true, .collisionNormal = {.x = 1, .y = 0}};
    }
    else if (shapeLocation.x - mRadius < 0)
    {
        result = {.bCollided = true, .collisionNormal = {.x = -1, .y = 0}};
    }
    else if (shapeLocation.y + mRadius > windowSize.y)
    {
        result = {.bCollided = true, .collisionNormal = {.x = 0, .y = 1}};
    }
    else if (shapeLocation.y - mRadius < 0)
    {
        result = {.bCollided = true, .collisionNormal = {.x = 0, .y = -1}};
    }

    return result;
}
