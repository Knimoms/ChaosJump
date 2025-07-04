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

    virtual CollisionResult isCollidingWithShapeAtLocation(Vector2 shapeLocation, const CollisionShapeInterface* otherShape, Vector2 otherLocation) = 0;
    
};
