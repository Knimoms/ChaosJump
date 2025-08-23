#pragma once
#include <memory>
#include <vector>

#include "CollisionShapeInterface.h"
#include "Math/Vector2.h"

class CollisionShapeInterface;

class CollisionObject
{

private:

    int32_t mNoMoveCount = 0;
    bool bCanCollideWithWindowBorder = true;

    float mArea = 1.f;
    float mDensity = 1.f;

protected:

    Vector2 mVelocity = {.x = 0, .y = 0};
    Vector2 mLocation = {.x = 0, .y = 0};
    
    float mDampingPerSecond  = 0.f;
    
    std::unique_ptr<CollisionShapeInterface> mCollisionShape = nullptr;

    uint64_t mLastMoveFrame = 0 - 1;
    
public:

    static std::vector<CollisionObject*> sCollisionObjects;

    CollisionObject();
    virtual ~CollisionObject(); 
    
    void setCollisionShape(CollisionShapeInterface* inCollisionShape);
    CollisionShapeInterface* getCollisionShape() const { return mCollisionShape.get(); }

    Vector2 getMoveLocation(float deltaTime) const;

    virtual void tick(float deltaTime);
    void moveTick(float deltaTime);

    const Vector2& getLocation() const { return mLocation; }
    void setLocation(const Vector2& inLocation);
    
    const Vector2& getVelocity() const { return mVelocity; }
    void setVelocity(Vector2 inVelocity);

    void setArea(float inArea);
    void setDensity(float inDensity);

    float getMass() const { return mDensity * mArea; }

    CollisionResult getCurrentCollisionResult() const;

protected:

    CollisionResult getCollisionResultOnLocation(const Vector2& inLocation) const;
    CollisionResult getMoveCollisionResult(float deltaTime) const;
        
};
