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
        return getCollisionResultForShapes(polygonShape, otherLocation, this, shapeLocation);
    }
    
    if (const CircleShape* otherCircleShape = dynamic_cast<const CircleShape*>(otherShape))
    {
        return getCollisionResultForShapes(this, shapeLocation, otherCircleShape, otherLocation);
    }

    if (const RectangleShape* rectangleShape = dynamic_cast<const RectangleShape*>(otherShape))
    {
        return getCollisionResultForShapes(rectangleShape, otherLocation, this, shapeLocation);
    }

    return {};
}

CollisionResult RectangleShape::isCollidingWithWindowBorderAtLocation(const Vector2& shapeLocation, const Vector2& windowSize)
{

    CollisionResult result;

    const auto [left, top] = shapeLocation - mRectangleExtent;
    const auto [right, bottom] = shapeLocation + mRectangleExtent;

    if (left <= 0)
    {
        result.collisionNormal -= {.x = 1, .y = 0};
    }
    else if (right >= windowSize.x)
    {
        result.collisionNormal -= {.x = -1, .y = 0};
    }

    if (top <= 0)
    {
        result.collisionNormal -= {.x = 0, .y = 1};
    }
    else if (bottom >= windowSize.y)
    {
        result.collisionNormal -= {.x = 0, .y = -1};
    }

    if (!result.collisionNormal.isAlmostZero())
    {
        result.bCollided = true;
        result.collisionNormal.normalize();
    }

    return result;
}
