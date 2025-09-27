#pragma once
#include "Physics/CollisionShapeInterface.h"

class CircleShape : public CollisionShapeInterface
{

private:

    float mRadius = 0.f;
    
public:

    CircleShape(float radius);

    float getRadius() const { return mRadius; };

    //~ Being CollisionShapeInterface Interface
    CollisionResult isCollidingWithWindowBorderAtLocation(const Vector2& shapeLocation, const Vector2& viewLocation, const Vector2& windowSize) override;
    //~ End CollisionShapeInterface Interface
    
};
