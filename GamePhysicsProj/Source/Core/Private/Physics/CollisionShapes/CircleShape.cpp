#include "Physics/CollisionShapes/CircleShape.h"

#include "Application.h"
#include "Physics/CollisionShapes/PolygonShape.h"
#include "Physics/CollisionShapes/RectangleShape.h"

CircleShape::CircleShape(float radius) : mRadius(radius)
{
}

CollisionResult CircleShape::isCollidingWithShapeAtLocation(const Vector2& shapeLocation, const CollisionShapeInterface* otherShape, const Vector2& otherLocation)
{
    if (const PolygonShape* otherPolygon = dynamic_cast<const PolygonShape*>(otherShape))
    {
        return getCollisionResultForShapes(this, shapeLocation, otherPolygon, otherLocation);
    }
    
    if (const CircleShape* circle = dynamic_cast<const CircleShape*>(otherShape))
    {
        return getCollisionResultForShapes(this, shapeLocation, circle, otherLocation);
    }

    if (const RectangleShape* rectangleShape = dynamic_cast<const RectangleShape*>(otherShape))
    {
        return getCollisionResultForShapes(this, shapeLocation, rectangleShape, otherLocation);
    }

    return {};
}

CollisionResult CircleShape::isCollidingWithWindowBorderAtLocation(const Vector2& shapeLocation, const Vector2& viewLocation, const Vector2& windowSize)
{
    CollisionResult result;

    const auto [left, top] = viewLocation;
    const auto [right, bottom] = viewLocation + windowSize;

    if (shapeLocation.x + mRadius > right)
    {
        result = {.bCollided = true, .collisionNormal = {.x = -1, .y = 0}};
    }
    else if (shapeLocation.x - mRadius < left)
    {
        result = {.bCollided = true, .collisionNormal = {.x = 1, .y = 0}};
    }
    else if (shapeLocation.y + mRadius > bottom)
    {
        result = {.bCollided = true, .collisionNormal = {.x = 0, .y = -1}};
    }
    else if (shapeLocation.y - mRadius < top)
    {
        result = {.bCollided = true, .collisionNormal = {.x = 0, .y = 1}};
    }

    return result;
}
