#pragma once
#include "Math/Vector2.h"


class CollisionObject;

struct CollisionResult
{
    CollisionObject* collisionObject = nullptr;
    bool bCollided = false;
    Vector2 collisionNormal = {.x = 0, .y = 0};
};

class CollisionShapeInterface
{
private:

    CollisionObject* mOwner = nullptr;
    
public:
    
    virtual ~CollisionShapeInterface() = default;

    CollisionObject* GetOwner() const { return mOwner; }
    void setOwner(CollisionObject* owner);

    template<class LShapeClass, class RShapeClass>
    static CollisionResult getCollisionResultForShapes(const LShapeClass* shape, const Vector2& shapeLocation, const RShapeClass* otherShape, const Vector2& otherLocation);

    virtual CollisionResult isCollidingWithShapeAtLocation(const Vector2& shapeLocation, const CollisionShapeInterface* otherShape, const Vector2& otherLocation) = 0;
    virtual CollisionResult isCollidingWithWindowBorderAtLocation(const Vector2& shapeLocation, const Vector2& windowSize) = 0;
    
};
