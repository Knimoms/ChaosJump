#include <algorithm>

#include "Physics/CollisionShapes/PolygonShape.h"

#include "Application.h"
#include "Debug/DebugDefinitions.h"

Vector2 getNormalForEdgeVector(const Vector2& edge)
{
    const Vector2 normal = {-edge.y, edge.x};
    return normal.getNormalized();
}

void PolygonShape::getNormalsForRotation(const float rotation, std::vector<Vector2>& outNormals) const
{
    Vector2 currentVertex = mVertices.back();
    for (const Vector2& vertex : mVertices)
    {
        const Vector2 edge = vertex - currentVertex;
        const Vector2 normal = getNormalForEdgeVector(edge);
        
        outNormals.push_back(normal);
        currentVertex = vertex;
    }
}

PolygonShape::Extremes PolygonShape::getExtremesOnNormal(const Vector2& location, float rotation, const Vector2& normal) const
{
    Extremes extremes {0, 0};

    Vector2 windowCenter = Application::getApplication().getWindowSize() / 2;

    bool bFirstVertex = true;
    for (const Vector2& vertex : mVertices)
    {
        const Vector2 translated = (vertex + location) - windowCenter;
        const float dot = translated.dot(normal);
        if (bFirstVertex)
        {
            extremes.min = dot;
            extremes.max = dot;
            
            bFirstVertex = false;
            
            continue;
        }
        
        extremes.min = std::min(extremes.min, dot);
        extremes.max = std::max(extremes.max, dot);
    }

    return extremes;
}

PolygonShape::PolygonShape(const std::vector<Vector2>& vertices) : mVertices(vertices)
{
}

CollisionResult PolygonShape::isCollidingWithShapeAtLocation(const Vector2& shapeLocation, const CollisionShapeInterface* otherShape, const Vector2& otherLocation)
{
    CollisionResult result;
    const PolygonShape* otherPolygon = dynamic_cast<const PolygonShape*>(otherShape);
    if (!otherPolygon)
    {
        return result;
    }

    result.bCollided = true;
    
    std::vector<Vector2> normals;
    getNormalsForRotation(0, normals);
    otherPolygon->getNormalsForRotation(0, normals);

    float smallestEdgeDistance = std::numeric_limits<float>::max();

    for (const Vector2& normal : normals)
    {
        auto [min, max] = getExtremesOnNormal(shapeLocation, 0, normal);
        auto [otherMin, otherMax] = otherPolygon->getExtremesOnNormal(otherLocation, 0, normal);

        const bool bColliding = (otherMin < max) == (min < otherMax);

#if DEBUG_COLLISION
        Application& application = Application::getApplication();

        const Vector2 windowCenter = application.getWindowSize() / 2;
        const DebugLine debugLine = {.start = windowCenter + normal * min, .end = windowCenter + normal * max, .color = {.r = 1.f * bColliding, .g = 0.f, .b = 0.5f}};
        application.addDebugLine(debugLine);
#endif
        
        if (!bColliding)
        {
            result.bCollided = false;
#if !DEBUG_COLLISION
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
        result.collisionObject = otherPolygon->GetOwner();
    }
    
    return result;
}

Vector2 GetBoundsCollideNormalForPoint(const Vector2& point, const Vector2& bounds)
{
    Vector2 result = {0, 0};
    if (point.x < 0 )
    {
        result += {-1, 0};
    }
    else if (point.x > bounds.x)
    {
        result += {1, 0};
    }

    if (point.y < 0 )
    {
        result += {0, -1};
    }
    else if (point.y > bounds.y)
    {
        result += {0, 1};
    }

    return result;
}

CollisionResult PolygonShape::isCollidingWithWindowBorderAtLocation(const Vector2& shapeLocation, const Vector2& windowSize)
{
    CollisionResult result;
    
    for (const Vector2& vertex : mVertices)
    {
        Vector2 normal = GetBoundsCollideNormalForPoint(vertex + shapeLocation, windowSize);
        if (!normal.isAlmostZero())
        {
            result.bCollided = true;
            result.collisionNormal = normal;
            break;
        }
    }
    
    return result;
}
