#pragma once
#include <map>
#include <memory>
#include <unordered_set>
#include <vector>

#include "CollisionShapeInterface.h"
#include "Core/TickableInterface.h"
#include "Math/Vector2.h"

class CollisionShapeInterface;

enum class CollisionCategory : uint32_t
{
    Ground   = 1 << 0,
    Obstacle = 1 << 1,
    Player   = 1 << 2
};

enum class CollisionResponse : uint32_t
{
    Overlap,
    Block,
    Ignore
};

struct CollisionResponseConfig
{
    mutable std::map<CollisionCategory, CollisionResponse> collisionResponseMap;

    CollisionResponse& operator[] (CollisionCategory collisionCategory);
    const CollisionResponse& operator[] (CollisionCategory collisionCategory) const;

    auto begin() const {
        return collisionResponseMap.begin();
    }

    auto end() const {
        return collisionResponseMap.end();
    }
};

class CollisionObject : public TickableInterface
{

private:

    bool bCanMove = true;

    struct
    {
        bool bCollideWindowX = true;
        bool bCollideWindowY = true;
    };

    float mArea = 1.f;
    float mDensity = 1.f;

    CollisionCategory mCollisionCategory = CollisionCategory::Obstacle;

protected:

    Vector2 mVelocity = {.x = 0, .y = 0};
    Vector2 mLocation = {.x = 0, .y = 0};
    Vector2 mGravity = {.x = 0, .y = 981.f};
    
    Vector2 mDampingPerSecond = {.x = 0, .y = 0};
    
    std::unique_ptr<CollisionShapeInterface> mCollisionShape = nullptr;

    uint64_t mLastMoveFrame = 0 - 1;

    static std::map<CollisionCategory, CollisionResponseConfig> sDefaultCollisionResponseConfigs;
    static std::map<CollisionCategory, std::vector<CollisionObject*>> sCollisionCategoryBuckets;

    CollisionResponseConfig mCollisionResponseConfig = {};
    
public:

    CollisionObject();
    ~CollisionObject() override; 

    CollisionShapeInterface* getCollisionShape() const { return mCollisionShape.get(); }
    void setCollisionShape(CollisionShapeInterface* inCollisionShape);

    Vector2 getMoveLocation(float deltaTime) const;

    void tick(float deltaTime) override;
    void moveTick(float deltaTime);

    CollisionCategory getCollisionCategory() const { return mCollisionCategory; }
    void setCollisionCategory(CollisionCategory inCollisionCategory);

    void setGravity(const Vector2& inGravity);
    void setCanMove(bool inCanMove);

    const Vector2& getLocation() const { return mLocation; }
    void setLocation(const Vector2& inLocation);
    
    const Vector2& getVelocity() const { return mVelocity; }
    void setVelocity(Vector2 inVelocity);

    void setArea(float inArea);
    void setDensity(float inDensity);

    float getMass() const { return mDensity * mArea; }
    
    CollisionResult getCurrentCollisionResult() const;
    void setCanCollideWithWindowBorder(bool inCollideX, bool inCollideY);

private:

    std::unordered_set<CollisionObject*> mOverlappingObjects;
    std::unordered_set<CollisionObject*> mBlockingObjects;

protected:

    void removeFromBucket();

    void handleCollision(const CollisionResult& collisionResult);
    void updateCollision(float deltaTime);

    virtual void handleCollisionHit(CollisionObject* collisionObject, const Vector2& collisionNormal);
    
    virtual void handleCollisionBegin(CollisionObject* collisionObject, const Vector2& collisionNormal);
    virtual void handleCollisionUpdate(CollisionObject* collisionObject, const Vector2& collisionNormal);
    virtual void handleCollisionEnd(CollisionObject* collisionObject);

    Vector2 getScreenLocationForView(const Vector2& viewLocation) const { return getLocation() - viewLocation; }

    virtual CollisionResult getCollisionResultOnLocation(const Vector2& inLocation) const;
    virtual CollisionResult getMoveCollisionResult(float deltaTime) const;
        
};
