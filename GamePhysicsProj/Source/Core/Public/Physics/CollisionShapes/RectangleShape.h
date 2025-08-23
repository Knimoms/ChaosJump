#pragma once
#include "Physics/CollisionShapeInterface.h"

class RectangleShape : public CollisionShapeInterface
{
private:
    
    Vector2 mRectangleExtent = {.x = 1, .y = 1};

public:

    RectangleShape(const Vector2& inRectangleExtent);

    const Vector2& getExtent() const { return mRectangleExtent; }

    //~ Being CollisionShapeInterface Interface
    CollisionResult isCollidingWithShapeAtLocation(const Vector2& shapeLocation, const CollisionShapeInterface* otherShape, const Vector2& otherLocation) override;
    CollisionResult isCollidingWithWindowBorderAtLocation(const Vector2& shapeLocation, const Vector2& windowSize) override;
    //~ End CollisionShapeInterface Interface
};
