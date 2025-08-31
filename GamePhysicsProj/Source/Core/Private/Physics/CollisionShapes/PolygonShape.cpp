#include <algorithm>

#include "Physics/CollisionShapes/PolygonShape.h"

#include "Application.h"
#include "Physics/CollisionShapes/CircleShape.h"

Vector2 getNormalForEdgeVector(const Vector2& edge)
{
    const Vector2 normal = {-edge.y, edge.x};
    return normal.getNormalized();
}

Vector2 PolygonShape::getVertexClosestToRelativeLocation(const Vector2& location) const
{

    if (mVertices.empty()) return Vector2{0.f, 0.f};

    const size_t vertexCount = mVertices.size();
    Vector2 closestVertex = mVertices[0];
    float closestDistanceSquared = (location - closestVertex).squaredSize();

    for (size_t i = 1; i < vertexCount; ++i)
    {
        const Vector2& vertex = mVertices[i];
        const float distanceSquared = (location - vertex).squaredSize();
        
        if (distanceSquared > closestDistanceSquared)
        {
            closestVertex = vertex;
            closestDistanceSquared = distanceSquared;
        }
    }

    return closestVertex;
}

void PolygonShape::getNormals(std::vector<Vector2>& outNormals) const
{
    if (!mNormalCache.empty())
    {
        outNormals.insert(outNormals.end(), mNormalCache.begin(), mNormalCache.end());
        return;    
    }
    
    Vector2 currentVertex = mVertices.back();
    for (const Vector2& vertex : mVertices)
    {
        const Vector2 edge = vertex - currentVertex;
        const Vector2 normal = getNormalForEdgeVector(edge);

        mNormalCache.push_back(normal);
        outNormals.push_back(normal);
        currentVertex = vertex;
    }
}

PolygonShape::Extremes PolygonShape::getExtremesOnNormal(const Vector2& location, const Vector2& normal) const
{
    Extremes extremes {0, 0};

    bool bFirstVertex = true;
    for (const Vector2& vertex : mVertices)
    {
        const Vector2 translated = (vertex + location);
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

PolygonShape::PolygonShape(const std::vector<Vector2>& vertices) : CollisionShapeInterface(2), mVertices(vertices)
{
}

static Vector2 GetBoundsCollideNormalForPoint(const Vector2& point, const Vector2& boundsLocation, const Vector2& bounds)
{
    const auto [left, top] = boundsLocation;
    const auto [right, bottom] = boundsLocation + bounds;
    
    Vector2 result = {0, 0};
    if (point.x < left)
    {
        result += {-1, 0};
    }
    else if (point.x > right)
    {
        result += {1, 0};
    }

    if (point.y < top)
    {
        result += {0, -1};
    }
    else if (point.y > bottom)
    {
        result += {0, 1};
    }

    return result;
}

CollisionResult PolygonShape::isCollidingWithWindowBorderAtLocation(const Vector2& shapeLocation, const Vector2& viewLocation, const Vector2& windowSize)
{
    CollisionResult result;
    
    for (const Vector2& vertex : mVertices)
    {
        Vector2 normal = GetBoundsCollideNormalForPoint(vertex + shapeLocation, viewLocation, windowSize);
        if (!normal.isAlmostZero())
        {
            result.bCollided = true;
            result.collisionNormal = -normal;
            break;
        }
    }
    
    return result;
}
