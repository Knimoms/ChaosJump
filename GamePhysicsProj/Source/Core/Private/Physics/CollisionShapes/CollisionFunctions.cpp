#include <algorithm>

#include "Application.h"
#include "Debugging/DebugDefinitions.h"
#include "Physics/CollisionShapes/PolygonShape.h"
#include "Physics/CollisionShapes/CircleShape.h"
#include "Physics/CollisionShapes/RectangleShape.h"

#define INVERT_COLLISION_DEFINITION(leftType, rightType)\
template<>\
CollisionResult CollisionShapeInterface::getCollisionResultForShapes(const leftType* shape, const Vector2& shapeLocation, const rightType* otherShape, const Vector2& otherLocation)\
{\
    CollisionResult result = getCollisionResultForShapes(otherShape, otherLocation, shape, shapeLocation);\
    return result.getInverted(otherShape->getOwner());\
}

template<>
CollisionResult CollisionShapeInterface::getCollisionResultForShapes(const CircleShape* shape, const Vector2& shapeLocation, const CircleShape* otherShape, const Vector2& otherLocation)
{
    ensure(otherShape);
    
    CollisionResult result;
    const Vector2 deltaLocation = shapeLocation - otherLocation;
    const float sqrdDistance = deltaLocation.squaredSize();

    const float radiiLength = shape->getRadius() + otherShape->getRadius();
    const float srqdRadiiLength = radiiLength * radiiLength;

    if (srqdRadiiLength > sqrdDistance)
    {
        result.collisionObject = otherShape->getOwner();
        result.bCollided = true;
        result.collisionNormal = deltaLocation.getNormalized();
    }

    return result;
}

template<>
CollisionResult CollisionShapeInterface::getCollisionResultForShapes(const PolygonShape* shape, const Vector2& shapeLocation, const PolygonShape* otherShape, const Vector2& otherLocation)
{
    CollisionResult result;

    result.bCollided = true;
    
    std::vector<Vector2> normals;
    shape->getNormalsForRotation(0, normals);
    otherShape->getNormalsForRotation(0, normals);

    float smallestEdgeDistance = std::numeric_limits<float>::max();

    for (const Vector2& normal : normals)
    {
        auto [min, max] = shape->getExtremesOnNormal(shapeLocation, 0, normal);
        auto [otherMin, otherMax] = otherShape->getExtremesOnNormal(otherLocation, 0, normal);

        const bool bColliding = (otherMin < max) == (min < otherMax);

#if DEBUG_SAT_POLYGON_COLLISION
        Application& application = Application::getApplication();

        const Vector2 windowCenter = application.getWindowSize() / 2;
        const DebugLine debugLine = {.start = windowCenter + normal * min, .end = windowCenter + normal * max, .color = {.r = 1.f * bColliding, .g = 0.f, .b = 0.5f}};
        application.addDebugLine(debugLine);
#endif
        
        if (!bColliding)
        {
            result.bCollided = false;
#if !DEBUG_SAT_POLYGON_COLLISION
            break;
#endif
        }

        const float vertexDistance1 = otherMin - max;
        const float vertexDistance2 = min - otherMax;

        float smallestVertexDistance = std::min(std::abs(vertexDistance1), std::abs(vertexDistance2));

        if (smallestVertexDistance < smallestEdgeDistance)
        {
            result.collisionNormal = normal;
            smallestEdgeDistance = smallestVertexDistance;
        }
    }
    
    if (!result.bCollided)
    {
        result.collisionNormal = {.x = 0, .y = 0};
    }
    else
    {
        result.collisionObject = otherShape->getOwner();
    }
    
    return result;
}

template<>
CollisionResult CollisionShapeInterface::getCollisionResultForShapes(const PolygonShape* shape, const Vector2& shapeLocation, const CircleShape* otherShape, const Vector2& otherLocation)
{
    CollisionResult result;

    result.bCollided = true;
    
    std::vector<Vector2> normals;
    shape->getNormalsForRotation(0, normals);

    normals.push_back(((shapeLocation + shape->getVertexClosestToRelativeLocation(otherLocation - shapeLocation) - otherLocation).getNormalized()));

    float smallestEdgeDistance = std::numeric_limits<float>::max();

    Application& application = Application::getApplication();
    const Vector2 windowCenter = application.getWindowSize() / 2;
    
    for (const Vector2& normal : normals)
    {
        auto [min, max] = shape->getExtremesOnNormal(shapeLocation, 0, normal);

        const Vector2 translatedCircleCenter = otherLocation - windowCenter;
        const float locationNormalProjection = translatedCircleCenter.dot(normal);
        const float radius = otherShape->getRadius();
        const float otherMin = locationNormalProjection - radius;
        const float otherMax = locationNormalProjection + radius;

        const bool bColliding = !(otherMax < min || max < otherMin);

#if DEBUG_SAT_CIRCLE_COLLISION

        const DebugLine debugLine = {.start = windowCenter + normal * otherMin, .end = windowCenter + normal * otherMax, .color = {.r = 1.f * bColliding, .g = 0.f, .b = 0.5f}};
        application.addDebugLine(debugLine);
#endif
        
        if (!bColliding)
        {
            result.bCollided = false;
#if !DEBUG_SAT_CIRCLE_COLLISION
            break;
#endif
        }

        const float vertexDistance1 = otherMin - max;
        const float vertexDistance2 = min - otherMax;

        float smallestVertexDistance = std::min(std::abs(vertexDistance1), std::abs(vertexDistance2));

        if (smallestVertexDistance < smallestEdgeDistance)
        {
            result.collisionNormal = normal;
            smallestEdgeDistance = smallestVertexDistance;
        }
    }
    
    if (!result.bCollided)
    {
        result.collisionNormal = {.x = 0, .y = 0};
    }
    else
    {
        result.collisionObject = otherShape->getOwner();
    }
    
    return result;
}

INVERT_COLLISION_DEFINITION(CircleShape, PolygonShape)

template<>
CollisionResult CollisionShapeInterface::getCollisionResultForShapes(const RectangleShape* shape, const Vector2& shapeLocation, const RectangleShape* otherShape, const Vector2& otherLocation)
{
    CollisionResult result;

    const auto& [x, y] = shapeLocation;
    const auto& [otherX, otherY] = otherLocation;
    const auto& [halfWidth, halfHeight] = shape->getExtent();
    const auto& [otherHalfWidth, otherHalfHeight] = otherShape->getExtent();

    const float deltaX = otherX - x;
    const float xPenetration = (halfWidth + otherHalfWidth) - std::abs(deltaX);

    if (xPenetration <= 0) return result;

    const float deltaY = otherY - y;
    const float yPenetration = (halfHeight + otherHalfHeight) - std::abs(deltaY);

    if (yPenetration <= 0) return result; 
    
    result.bCollided = true;
    result.collisionObject = otherShape->getOwner();

    if (xPenetration < yPenetration)
    {
        result.collisionNormal = {.x = (deltaX < 0) ? -1.f : 1.f, .y = 0.f}; 
    }
    else
    {
        result.collisionNormal = {.x = 0.f, .y = (deltaY < 0) ? 1.f : -1.f}; 
    }


    Application& application = Application::getApplication();

    const DebugLine debugLine = {.start = otherLocation, .end = otherLocation + result.collisionNormal * 100, .color = {.r = 1.f, .g = 0.f, .b = 0.5f}};
    application.addDebugLine(debugLine);

    return result;
}

template<>
CollisionResult CollisionShapeInterface::getCollisionResultForShapes(const RectangleShape* shape, const Vector2& shapeLocation, const PolygonShape* otherShape, const Vector2& otherLocation)
{
    CollisionResult result;
    result.bCollided = true;

    std::vector<Vector2> normals;
    otherShape->getNormalsForRotation(0, normals);

    normals.push_back({1.f, 0.f});
    normals.push_back({0.f, 1.f});

    const auto& [halfWidth, halfHeight] = shape->getExtent();

    const auto getRectangleExtremesOnNormal = [&](const Vector2& location, const Vector2& normal) -> std::pair<float,float>
    {
        const Vector2 corners[4] = {
            {.x = location.x - halfWidth, .y = location.y - halfHeight},
            {.x = location.x + halfWidth, .y = location.y - halfHeight},
            {.x = location.x + halfWidth, .y = location.y + halfHeight},
            {.x = location.x - halfWidth, .y = location.y + halfHeight}
        };

        float min = std::numeric_limits<float>::infinity();
        float max = -std::numeric_limits<float>::infinity();

        for (const Vector2& corner : corners)
        {
            const float projection = corner.dot(normal);
            min = std::min(projection, min);
            max = std::max(projection, max);
        }

        return {min, max};
    };

    float smallestEdgeDistance = std::numeric_limits<float>::max();

    for (Vector2 normal : normals)
    {
        normal = normal.getNormalized();

        auto [minRect, maxRect] = getRectangleExtremesOnNormal(shapeLocation, normal);
        auto [minPoly, maxPoly] = otherShape->getExtremesOnNormal(otherLocation, 0, normal);

        const bool bColliding = !(maxPoly < minRect || maxRect < minPoly);

#if DEBUG_SAT_RECTANGLE_COLLISION
        Application& application = Application::getApplication();
        const Vector2 windowCenter = application.getWindowSize() / 2;
        const DebugLine debugLine = {.start = windowCenter + normal * minRect, .end = windowCenter + normal * maxRect, .color = {.r = 1.f * bColliding, .g = 0.f, .b = 0.5f}};
        const DebugLine debugLine2 = {.start = normal * minPoly, .end = normal * maxPoly, .color = {.r = 1.f * bColliding, .g = 0.f, .b = 0.5f}};
        application.addDebugLine(debugLine);
        application.addDebugLine(debugLine2);
#endif

        if (!bColliding)
        {
            result.bCollided = false;
#if !DEBUG_SAT_RECTANGLE_COLLISION
            break;
#endif
        }

        const float vertexDistance1 = minPoly - maxRect;
        const float vertexDistance2 = minRect - maxPoly;
        const float smallestVertexDistance = std::min(std::abs(vertexDistance1), std::abs(vertexDistance2));

        if (smallestVertexDistance < smallestEdgeDistance)
        {
            result.collisionNormal = normal;
            smallestEdgeDistance = smallestVertexDistance;
        }
    }

    if (!result.bCollided)
    {
        result.collisionNormal = {.x = 0.f, .y = 0.f};
    }
    else
    {
        const Vector2 delta = shapeLocation - otherLocation;
        if (result.collisionNormal.dot(delta) < 0.f)
        {
            result.collisionNormal *= -1.f;
        }
        result.collisionObject = otherShape->getOwner();
    }

    return result;
}

INVERT_COLLISION_DEFINITION(PolygonShape, RectangleShape)

/*template<>
CollisionResult CollisionShapeInterface::getCollisionResultForShapes(const PolygonShape* shape, const Vector2& shapeLocation, const RectangleShape* otherShape, const Vector2& otherLocation)
{
    return {};
}*/

template<>
CollisionResult CollisionShapeInterface::getCollisionResultForShapes(const RectangleShape* shape, const Vector2& shapeLocation, const CircleShape* otherShape, const Vector2& otherLocation)
{
    CollisionResult result;

    const auto [halfWidth, halfHeight] = shape->getExtent();
    const float radius = otherShape->getRadius();

    const Vector2 delta = otherLocation - shapeLocation;

    const float clampedX = std::max(-halfWidth, std::min(delta.x, halfWidth));
    const float clampedY = std::max(-halfHeight, std::min(delta.y, halfHeight));
    const Vector2 closestPoint = {clampedX, clampedY};

    const Vector2 difference = closestPoint - delta;
    const float distanceSquared = difference.squaredSize();

    if (distanceSquared > radius * radius)
    {
        return result;
    }

    result.bCollided = true;
    result.collisionObject = otherShape->getOwner();

    if (distanceSquared > 0.000001f)
    {
        result.collisionNormal = difference.getNormalized();
    }
    else
    {
        const float xPenetration = (halfWidth + radius) - std::abs(delta.x);
        const float yPenetration = (halfHeight + radius) - std::abs(delta.y);

        if (xPenetration < yPenetration)
        {
            result.collisionNormal = {.x = (delta.x < 0) ? 1.f : -1.f, .y = 0.f}; 
        }
        else
        {
            result.collisionNormal = {.x = 0.f, .y = (delta.y < 0) ? 1.f : -1.f}; 
        }
    }

    return result;
}

INVERT_COLLISION_DEFINITION(CircleShape, RectangleShape)
