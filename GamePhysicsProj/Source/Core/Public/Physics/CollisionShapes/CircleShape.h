#pragma once
#include "Physics/CollisionShapeInterface.h"

class CircleShape : public CollisionShapeInterface
{

private:

    float mRadius = 0.f;
    
public:

    CircleShape(float radius);

    //~ Being CollisionShapeInterface Interface
    CollisionResult isCollidingWithShapeAtLocation(const Vector2& shapeLocation, const CollisionShapeInterface* otherShape, const Vector2& otherLocation) override;
    CollisionResult isCollidingWithWindowBorderAtLocation(const Vector2& shapeLocation, const Vector2& windowSize) override;
    //~ End CollisionShapeInterface Interface
    
};
