#pragma once
#include <vector>

#include "Physics/CollisionShapeInterface.h"

class PolygonShape : public CollisionShapeInterface
{

private:

    std::vector<Vector2> mVertices = {};

private:

    mutable std::vector<Vector2> mNormalCache = {};

public:

    
    Vector2 getVertexClosestToRelativeLocation(const Vector2& location) const;
    void getNormals(std::vector<Vector2>& outNormals) const;

    struct Extremes
    {
        float min;
        float max;
    };
    
    Extremes getExtremesOnNormal(const Vector2& location, const Vector2& normal) const;
    
    PolygonShape(const std::vector<Vector2>& vertices);

    //~ Being CollisionShapeInterface Interface
    CollisionResult isCollidingWithWindowBorderAtLocation(const Vector2& shapeLocation, const Vector2& viewLocation, const Vector2& windowSize) override;
    //~ End CollisionShapeInterface Interface
    
};
