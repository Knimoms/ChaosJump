#pragma once
#include "Math/Vector2.h"


struct CollisionResult
{
    bool bCollided = false;
    Vector2 collisionNormal = {.x = 0, .y = 0};
};

class CollisionShapeInterface
{
    
public:
    
    virtual ~CollisionShapeInterface() = default;

    virtual CollisionResult isCollidingWithShapeAtLocation(const Vector2& shapeLocation, const CollisionShapeInterface* otherShape, const Vector2& otherLocation) = 0;
    virtual CollisionResult isCollidingWithWindowBorderAtLocation(const Vector2& shapeLocation, const Vector2& windowSize) = 0;
    
};
