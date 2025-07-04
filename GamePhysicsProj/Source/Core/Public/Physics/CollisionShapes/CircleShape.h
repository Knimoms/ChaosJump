#pragma once
#include "Physics/CollisionShapeInterface.h"

class CircleShape : public CollisionShapeInterface
{

private:

    float mRadius = 0.f;
    
public:

    CircleShape(float radius);

    //~ Being CollisionShapeInterface Interface
    CollisionResult isCollidingWithShapeAtLocation(Vector2 shapeLocation, const CollisionShapeInterface* otherShape, Vector2 otherLocation) override;
    //~ End CollisionShapeInterface Interface
    
};
