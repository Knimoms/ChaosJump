#pragma once
#include <memory>
#include <vector>

#include "Math/Vector2.h"

class CollisionShapeInterface;

class CollisionObject
{

protected:

    Vector2 mVelocity = {.x = 0, .y = 0};
    Vector2 mLocation = {.x = 0, .y = 0};
    float mMass = 1.f;
    
    std::unique_ptr<CollisionShapeInterface> mCollisionShape = nullptr;
    
public:

    static std::vector<CollisionObject*> sCollisionObjects;

    CollisionObject();
    virtual ~CollisionObject(); 
    
    void setCollisionShape(CollisionShapeInterface* inCollisionShape);
    const CollisionShapeInterface* getCollisionShape() const { return mCollisionShape.get(); }
    
    virtual void tick(float deltaTime);
    void moveTick(float deltaTime);

    const Vector2& getLocation() const { return mLocation; }
    void setLocation(const Vector2& inLocation);
    
    const Vector2& getVelocity() const { return mVelocity; }
    void setVelocity(Vector2 inVelocity);

    float getMass() const { return mMass; }
    void setMass(float mass);
        
};
