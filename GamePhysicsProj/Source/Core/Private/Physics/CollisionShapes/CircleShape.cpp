#include "Physics/CollisionShapes/CircleShape.h"

#include "Application.h"
#include "Physics/CollisionShapes/PolygonShape.h"
#include "Physics/CollisionShapes/RectangleShape.h"

CircleShape::CircleShape(float radius) : CollisionShapeInterface(1), mRadius(radius)
{
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
