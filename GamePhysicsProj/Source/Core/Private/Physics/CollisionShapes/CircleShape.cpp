#include "Physics/CollisionShapes/CircleShape.h"

#include <cassert>

#include "Application.h"
#include "Physics/CollisionShapes/PolygonShape.h"

CircleShape::CircleShape(float radius) : mRadius(radius)
{
}

template<>
CollisionResult CollisionShapeInterface::getCollisionResultForShapes(const CircleShape* shape, const Vector2& shapeLocation, const CircleShape* otherShape, const Vector2& otherLocation)
{
    assert(otherShape);
    
    CollisionResult result;
    const Vector2 deltaLocation = otherLocation - shapeLocation;
    const float sqrdDistance = deltaLocation.squaredSize();

    const float radiiLength = shape->getRadius() + otherShape->getRadius();
    const float srqdRadiiLength = radiiLength * radiiLength;

    if (srqdRadiiLength > sqrdDistance)
    {
        result.collisionObject = otherShape->GetOwner();
        result.bCollided = true;
        result.collisionNormal = deltaLocation.getNormalized();
    }

    return result;
}

CollisionResult CircleShape::isCollidingWithShapeAtLocation(const Vector2& shapeLocation, const CollisionShapeInterface* otherShape, const Vector2& otherLocation)
{
    if (const CircleShape* otherCircleShape = dynamic_cast<const CircleShape*>(otherShape))
    {
        return getCollisionResultForShapes(this, shapeLocation, otherCircleShape, otherLocation);
    }

    if (const PolygonShape* polygonShape = dynamic_cast<const PolygonShape*>(otherShape))
    {
        return getCollisionResultForShapes(polygonShape, otherLocation, this, shapeLocation);
    }

    return {};
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
