#include "Physics/CollisionShapes/RectangleShape.h"

#include "Physics/CollisionShapes/CircleShape.h"
#include "Physics/CollisionShapes/PolygonShape.h"

RectangleShape::RectangleShape(const Vector2& inRectangleExtent) : CollisionShapeInterface(3), mRectangleExtent(inRectangleExtent)
{
}

CollisionResult RectangleShape::isCollidingWithWindowBorderAtLocation(const Vector2& shapeLocation, const Vector2& viewLocation, const Vector2& windowSize)
{

    CollisionResult result;

    const auto [left, top] = shapeLocation - mRectangleExtent;
    const auto [right, bottom] = shapeLocation + mRectangleExtent;

    const auto [windowLeft, windowTop] = viewLocation;
    const auto [windowRight, windowBottom] = viewLocation + windowSize;

    if (left < windowLeft)
    {
        result.collisionNormal += {.x = 1, .y = 0};
    }
    else if (right > windowRight)
    {
        result.collisionNormal += {.x = -1, .y = 0};
    }

    if (top < windowTop)
    {
        result.collisionNormal += {.x = 0, .y = 1};
    }
    else if (bottom > windowBottom)
    {
        result.collisionNormal += {.x = 0, .y = -1};
    }

    if (!result.collisionNormal.isAlmostZero())
    {
        result.bCollided = true;
        result.collisionNormal.normalize();
    }

    return result;
}
