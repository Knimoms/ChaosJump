#pragma once

#include "Math/Vector2.h"

class CollisionObject;

struct CollisionResult
{
    CollisionObject* collisionObject = nullptr;
    bool bCollided = false;
    bool bBlocked = false;
    Vector2 collisionNormal = {.x = 0, .y = 0};

    CollisionResult getInverted(CollisionObject* other) const;
};

template<uint8_t>
struct TIDtoCollisionShapeClass
{
    
    using Class = void;
};

class CollisionShapeInterface
{
    
private:

    uint8_t mCollisionShapeTypeID = 0;
    CollisionObject* mOwner = nullptr;
    
public:

    CollisionShapeInterface(uint8_t inCollisionShapeID);
    virtual ~CollisionShapeInterface() = default;

    uint8_t getCollisionShapeTypeID() const { return mCollisionShapeTypeID; }

    CollisionObject* getOwner() const { return mOwner; }
    void setOwner(CollisionObject* owner);

    template<class LShapeClass>
    static CollisionResult getCollisionResultForShapes(const LShapeClass* shape, const Vector2& shapeLocation, const CollisionShapeInterface* otherShape, const Vector2& otherLocation);
    
    template<class LShapeClass, class RShapeClass>
    static CollisionResult getCollisionResultForShapes(const LShapeClass* shape, const Vector2& shapeLocation, const RShapeClass* otherShape, const Vector2& otherLocation);

    CollisionResult isCollidingWithShapeAtLocation(const Vector2& shapeLocation, const CollisionShapeInterface* otherShape, const Vector2& otherLocation) const;
    virtual CollisionResult isCollidingWithWindowBorderAtLocation(const Vector2& shapeLocation, const Vector2& viewLocation, const Vector2& windowSize) = 0;
    
};