#include "Physics/CollisionObject.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <expected>

#include "Application.h"
#include "Debugging/DebugDefinitions.h"
#include "Physics/CollisionShapeInterface.h"

std::map<CollisionCategory, std::vector<CollisionObject*>> CollisionObject::sCollisionCategoryBuckets = {};

std::map<CollisionCategory, CollisionResponseConfig> CollisionObject::sDefaultCollisionResponseConfigs = {
    {
        CollisionCategory::Player,
        {{
            { CollisionCategory::Ground, CollisionResponse::Overlap },
            { CollisionCategory::Player, CollisionResponse::Block },
            { CollisionCategory::Obstacle, CollisionResponse::Block }
        }}
    },
    { CollisionCategory::Obstacle,
        {{
            { CollisionCategory::Ground, CollisionResponse::Ignore },
            { CollisionCategory::Player, CollisionResponse::Block },
            { CollisionCategory::Obstacle, CollisionResponse::Block }
        }}
    },
    { CollisionCategory::Ground,
        {{
            { CollisionCategory::Ground, CollisionResponse::Ignore },
            { CollisionCategory::Player, CollisionResponse::Ignore },
            { CollisionCategory::Obstacle, CollisionResponse::Ignore }
        }}
    }
};

CollisionResponse& CollisionResponseConfig::operator[](CollisionCategory collisionCategory)
{
    return collisionResponseMap[collisionCategory];
}

const CollisionResponse& CollisionResponseConfig::operator[](CollisionCategory collisionCategory) const
{
    return collisionResponseMap[collisionCategory];
}

CollisionObject::CollisionObject()
{
    sCollisionCategoryBuckets[mCollisionCategory].push_back(this);
    mCollisionResponseConfig = sDefaultCollisionResponseConfigs[mCollisionCategory];
}

CollisionObject::~CollisionObject()
{
    mOnDestroyed.broadcast(this);
    removeFromBucket();
}

void CollisionObject::setCollisionShape(CollisionShapeInterface* inCollisionShape)
{
    mCollisionShape = std::unique_ptr<CollisionShapeInterface>(inCollisionShape);
    mCollisionShape->setOwner(this);
}

Vector2 CollisionObject::getMoveLocation(const float deltaTime) const
{
    const uint64_t frameCount = Application::getApplication().getFrameCount();
    if (frameCount == mLastMoveFrame) return mLocation;
    
    return mVelocity * deltaTime + mLocation;
}

static Vector2 applyDamping(const Vector2 velocity, const Vector2 dampingPerSecond, const float deltaTime)
{
    if (dampingPerSecond.isAlmostZero()) return velocity;

    const Vector2 factor ={ std::exp(-dampingPerSecond.x * deltaTime), std::exp(-dampingPerSecond.y * deltaTime) };
    return factor.isAlmostZero(1e-4f) ? Vector2{0.f, 0.f} : velocity * factor;
}

void CollisionObject::tick(const float deltaTime)
{
    updateCollision(deltaTime);
    if (bCanMove)
    {
        moveTick(deltaTime);
    }
}

Vector2 computeElasticCollision(const float mass1, const float mass2, const Vector2& velocity1, const Vector2& velocity2, const Vector2& normal)
{
    const Vector2 relativeVelocity = velocity1 - velocity2;
    const float velocityAlongNormal = relativeVelocity.dot(normal);

    if (velocityAlongNormal >= 0.0f) return velocity1;

    const float impulseScalar = (2.0f * mass2 / (mass1 + mass2)) * velocityAlongNormal;

    return velocity1 - impulseScalar * normal;
}

template<typename T>
struct TScopedValueChange
{
    T* PropertyPtr = nullptr;
    T OldValue;
    
    TScopedValueChange(T& Property, T NewValue)
    {
        OldValue = Property;
        PropertyPtr = &Property;
        Property = NewValue;
    }

    ~TScopedValueChange()
    {
        *PropertyPtr = OldValue;
    }
};

void CollisionObject::moveTick(float deltaTime)
{
    constexpr float maxMoveDeltaTime = 1.f;

    if (deltaTime > maxMoveDeltaTime)
    {
        deltaTime = 0.f;
    }

    const Vector2 newLocation = getMoveLocation(deltaTime);

    const CollisionResult result = getMoveCollisionResult(deltaTime);
    auto& [collisionObject, bCollided, bBlocked, collisionNormal] = result;

    mVelocity = applyDamping(mVelocity, mDampingPerSecond, deltaTime);

    mLastMoveFrame = Application::getApplication().getFrameCount();

    if (!bBlocked)
    {
        mLocation = newLocation;
        mVelocity += mGravity * deltaTime;
    }    
    
    if (!bCollided) return;

    float oldDensity = 0.f;
    Vector2 oldVelocity{};
    
    if (collisionObject)
    {
        oldDensity = collisionObject->mDensity;
        oldVelocity = collisionObject->getVelocity();

        const CollisionResult inverted = result.getInverted(this);

        if (inverted.bCollided)
        {
            collisionObject->handleCollision(result.getInverted(this));
        }
    }

    float fallbackDensity;
    Vector2 fallbackVeloc;
    const TScopedValueChange ScopedVelocityChange(collisionObject ? collisionObject->mVelocity : fallbackVeloc, oldVelocity);
    const TScopedValueChange ScopedDensityChange(collisionObject ? collisionObject->mDensity : fallbackDensity, oldDensity);

    handleCollision(result);

    if (bBlocked && (!collisionObject || !collisionObject->bCanMove))
    {
        if (collisionNormal.x > 0.f && mVelocity.x < 0.f || collisionNormal.x < 0.f && mVelocity.x > 0.f)
        {
            mVelocity.x = 0.f;
        }
    
        if (collisionNormal.y > 0.f && mVelocity.y < 0.f || collisionNormal.y < 0.f && mVelocity.y > 0.f)
        {
            mVelocity.y = 0.f;
        }
    }
}

void CollisionObject::setCollisionCategory(const CollisionCategory inCollisionCategory)
{
    removeFromBucket();
    
    mCollisionCategory = inCollisionCategory;
    sCollisionCategoryBuckets[mCollisionCategory].push_back(this);
    mCollisionResponseConfig = sDefaultCollisionResponseConfigs[mCollisionCategory];
}

void CollisionObject::setGravity(const Vector2& inGravity)
{
    mGravity = inGravity;
}

void CollisionObject::setCanMove(const bool inCanMove)
{
    bCanMove = inCanMove;
}

void CollisionObject::setLocation(const Vector2& inLocation)
{
    mLocation = inLocation;
}

void CollisionObject::setVelocity(Vector2 inVelocity)
{
    mVelocity = inVelocity;
}

void CollisionObject::setArea(const float inArea)
{
    mArea = std::max(inArea, 0.0f);
}

void CollisionObject::setDensity(float inDensity)
{
    mArea = std::max(inDensity, 0.0f);
}

CollisionResult CollisionObject::getCurrentCollisionResult() const
{
    return getCollisionResultOnLocation(mLocation);
}

void CollisionObject::setCanCollideWithWindowBorder(const bool inCollideX, const bool inCollideY)
{
    bCollideWindowX = inCollideX;
    bCollideWindowY = inCollideY;
}

CollisionResponse CollisionObject::getCollisionResponseForCategory(const CollisionCategory inCollisionCategory) const
{
    return mCollisionResponseConfig[inCollisionCategory];
}

void CollisionObject::insertOverlappingObject(CollisionObject* inCollisionObject)
{
    mOverlappingObjects.insert(inCollisionObject);

    if (inCollisionObject)
    {
        mOverlapDestroyEventIds[inCollisionObject] = inCollisionObject->mOnDestroyed.subscribe([this](CollisionObject* collisionObject)
        {
            removeOverlappingObject(collisionObject);
        });
    }
}

void CollisionObject::removeOverlappingObject(CollisionObject* inCollisionObject)
{
    mOverlappingObjects.erase(inCollisionObject);
    if (inCollisionObject)
    {
        inCollisionObject->mOnDestroyed.unsubscribe(mOverlapDestroyEventIds[inCollisionObject]);
        mOverlapDestroyEventIds.erase(inCollisionObject);
    }
}

void CollisionObject::removeFromBucket()
{
    auto& collisionCategoryBuckets = sCollisionCategoryBuckets[mCollisionCategory];
    const auto it = std::ranges::find(collisionCategoryBuckets, this);
    assert(it != collisionCategoryBuckets.end());

    collisionCategoryBuckets.erase(it);
}

void CollisionObject::handleCollision(const CollisionResult& collisionResult)
{
    const auto [collisionObject, bCollided, bBlocked, collisionNormal] = collisionResult;
    
    if (collisionObject)
    {
        if (mOverlappingObjects.contains(collisionObject)) return;

        if (bBlocked)
        {
            insertOverlappingObject(collisionObject);
        }
    }

    if (bBlocked)
    {
        handleCollisionHit(collisionObject, collisionNormal);
    }
    else
    {
        insertOverlappingObject(collisionObject);
        handleCollisionBegin(collisionObject, collisionNormal);
    }
    
}

void CollisionObject::updateCollision(const float deltaTime)
{
    std::vector<CollisionObject*> stoppedOverlappingObjects = {};

    for (CollisionObject* collisionObject : mOverlappingObjects)
    {
        const Vector2 moveLocation = getMoveLocation(deltaTime);
        const CollisionResult result = [&]()
        {
            if (collisionObject)
            {
                const Vector2 otherLocation = collisionObject->getLocation();
                return mCollisionShape->isCollidingWithShapeAtLocation(moveLocation, collisionObject->getCollisionShape(), otherLocation);
            }
            
            Application& app = Application::getApplication();
            const Vector2& windowSize = app.getWindowSize();
            const Vector2 currentViewLocation = app.getCurrentViewLocation();
            return mCollisionShape->isCollidingWithWindowBorderAtLocation(moveLocation, currentViewLocation, windowSize);
        }();
        
        if (!result.bCollided)
        {
            stoppedOverlappingObjects.push_back(collisionObject);
        }
        else
        {
            handleCollisionUpdate(result.collisionObject, result.collisionNormal);
        }
    }

    for (CollisionObject* collisionObject : stoppedOverlappingObjects)
    {
        removeOverlappingObject(collisionObject);
        handleCollisionEnd(collisionObject);
    }
}

void CollisionObject::handleCollisionHit(CollisionObject* collisionObject, const Vector2& collisionNormal)
{
    const bool bIsCounterObjectStatic = !collisionObject || !collisionObject->bCanMove;
    const float counterMass = bIsCounterObjectStatic ? std::numeric_limits<float>::max()/2 : collisionObject->getMass();
    const Vector2 counterVeloctiy = bIsCounterObjectStatic ? Vector2{.x = 0.f, .y = 0.f} : collisionObject->mVelocity;
        
    mVelocity = computeElasticCollision(getMass(), counterMass, mVelocity, counterVeloctiy, collisionNormal);

    Application::getApplication().addDebugLine({mLocation, mLocation + collisionNormal * 100, { 0, 1, 0}, 2.f});
}

void CollisionObject::handleCollisionBegin(CollisionObject* collisionObject, const Vector2& collisionNormal)
{
    if (!collisionObject)
    {
        handleCollisionHit(collisionObject, collisionNormal);
    }
}

void CollisionObject::handleCollisionUpdate(CollisionObject* collisionObject, const Vector2& collisionNormal)
{

}

void CollisionObject::handleCollisionEnd(CollisionObject* collisionObject)
{
}

CollisionResult CollisionObject::getCollisionResultOnLocation(const Vector2& inLocation) const
{
    CollisionResult result;
    if (!mCollisionShape)
    {
        return result;
    }

    if (bCollideWindowX || bCollideWindowY)
    {
        Application& app = Application::getApplication();
        const Vector2& windowSize = app.getWindowSize();
        const Vector2 currentViewLocation = app.getCurrentViewLocation();
        result = mCollisionShape->isCollidingWithWindowBorderAtLocation(inLocation, currentViewLocation, windowSize);

        Vector2& collisionNormal = result.collisionNormal;
        if (!bCollideWindowX) collisionNormal.x = 0.0f;
        if (!bCollideWindowY) collisionNormal.y = 0.0f;

        result.bCollided = !collisionNormal.isAlmostZero();

        if (result.bCollided && !collisionNormal.isNormalized())
        {
            collisionNormal.normalize();
        }
    }
    
    if (result.bCollided) return result;

    const auto& CollisionResponseConfig = sDefaultCollisionResponseConfigs[mCollisionCategory];


    std::vector<const std::vector<CollisionObject*>*> collidableBucketsPtrs;
    for (const auto& [collisionCategory, collisionResponse] : CollisionResponseConfig)
    {
        if (collisionResponse != CollisionResponse::Ignore)
        {
            sCollisionCategoryBuckets[collisionCategory];
            collidableBucketsPtrs.push_back(&sCollisionCategoryBuckets[collisionCategory]);
        }
    }

    for (const auto bucketPtr: collidableBucketsPtrs)
    {
        for (CollisionObject* collisionObject : *bucketPtr)
        {
            if (collisionObject == this) continue;
        
            result = mCollisionShape->isCollidingWithShapeAtLocation(inLocation, collisionObject->getCollisionShape(), collisionObject->getLocation());
            if (result.bCollided)
            {
                result.bBlocked = CollisionResponse::Block == mCollisionResponseConfig[collisionObject->getCollisionCategory()] && !mOverlappingObjects.contains(collisionObject);
                return result;
            }
        }
    }

    return result;
}

CollisionResult CollisionObject::getMoveCollisionResult(const float deltaTime) const
{
    return getCollisionResultOnLocation(getMoveLocation(deltaTime));
}
