#include "Physics/CollisionShapes/RectangleShape.h"

#include "Physics/CollisionShapes/CircleShape.h"
#include "Physics/CollisionShapes/PolygonShape.h"

RectangleShape::RectangleShape(const Vector2& inRectangleExtent) : mRectangleExtent(inRectangleExtent)
{
}

CollisionResult RectangleShape::isCollidingWithShapeAtLocation(const Vector2& shapeLocation, const CollisionShapeInterface* otherShape, const Vector2& otherLocation)
{
    if (const PolygonShape* polygonShape = dynamic_cast<const PolygonShape*>(otherShape))
    {
        return getCollisionResultForShapes(this, shapeLocation, polygonShape, otherLocation);
    }
    
    if (const CircleShape* otherCircleShape = dynamic_cast<const CircleShape*>(otherShape))
    {
        return getCollisionResultForShapes(this, shapeLocation, otherCircleShape, otherLocation);
    }

    if (const RectangleShape* rectangleShape = dynamic_cast<const RectangleShape*>(otherShape))
    {
        return getCollisionResultForShapes(this, shapeLocation, rectangleShape, otherLocation);
    }

    return {};
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
