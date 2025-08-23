#include "Physics/CollisionObject.h"

#include <algorithm>
#include <cassert>
#include <cmath>

#include "Application.h"
#include "Debugging/DebugDefinitions.h"
#include "Physics/CollisionShapeInterface.h"

std::vector<CollisionObject*> CollisionObject::sCollisionObjects = {};

CollisionObject::CollisionObject()
{
    sCollisionObjects.push_back(this);
}

CollisionObject::~CollisionObject()
{
    const auto it = std::ranges::find(sCollisionObjects, this);
    assert(it != sCollisionObjects.end());

    sCollisionObjects.erase(it);
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

static Vector2 applyDamping(const Vector2 velocity, const float dampingPerSecond, const float deltaTime)
{
    if (dampingPerSecond <= 0.f) return velocity;

    const float factor = std::exp(-dampingPerSecond * deltaTime);
    return factor < 1e-4f ? Vector2{0.f, 0.f} : velocity * factor;
}

void CollisionObject::tick(float deltaTime)
{
    moveTick(deltaTime);
}

Vector2 computeElasticCollision(const float mass1, const float mass2, const Vector2& velocity1, const Vector2& velocity2, const Vector2& normal)
{
    const Vector2 relativeVelocity = velocity1 - velocity2;
    const float velocityAlongNormal = relativeVelocity.dot(normal);

    if (velocityAlongNormal <= 0.0f) return velocity1;

    const float impulseScalar = (2.0f * mass2 / (mass1 + mass2)) * velocityAlongNormal;

    return velocity1 - impulseScalar * normal;
}

void CollisionObject::moveTick(const float deltaTime)
{
    const Vector2 newLocation = getMoveLocation(deltaTime);

    const auto [collisionObject, bCollided, collisionNormal] = getMoveCollisionResult(deltaTime);

    mVelocity = applyDamping(mVelocity, mDampingPerSecond, deltaTime);
    
    if (!bCollided)
    {
        mNoMoveCount = 0;
        mLastMoveFrame = Application::getApplication().getFrameCount();
        mLocation = newLocation;
        return;
    }

    ensure(!collisionNormal.isAlmostZero());

    const float mass = getMass();
    const float counterMass = collisionObject ? collisionObject->getMass() : static_cast<float>(1LL << 63);
    Vector2 counterVeloctiy = collisionObject ? collisionObject->mVelocity : Vector2{.x = 0.f, .y = 0.f};

    Vector2 newVelocity = computeElasticCollision(mass, counterMass, mVelocity, counterVeloctiy, collisionNormal);
    const Vector2 otherVelocity = computeElasticCollision(counterMass, mass, counterVeloctiy, mVelocity, -1 * collisionNormal);

    if (++mNoMoveCount > 10)
    {
        Application::getApplication().addDebugLine({mLocation, mLocation + newVelocity, { 1, 1, 1}});
    }

    Application::getApplication().addDebugLine({mLocation, mLocation + collisionNormal * 100, { 0, 1, 0}, 2.f});
    mVelocity = newVelocity;

    if (collisionObject)
    {
        collisionObject->mVelocity = otherVelocity;
    }
}

void CollisionObject::setLocation(const Vector2& inLocation)
{
    mLocation = inLocation;
}

void CollisionObject::setVelocity(Vector2 inVelocity)
{
    mVelocity = inVelocity;
}

void CollisionObject::setArea(float inArea)
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

CollisionResult CollisionObject::getCollisionResultOnLocation(const Vector2& inLocation) const
{
    CollisionResult result;
    if (!mCollisionShape)
    {
        return result;
    }

    if (bCanCollideWithWindowBorder)
    {
        const Vector2& windowSize = Application::getApplication().getWindowSize();
        result = mCollisionShape->isCollidingWithWindowBorderAtLocation(inLocation, windowSize);
    }
    
    if (result.bCollided) return result;

    for (CollisionObject* collisionObject : sCollisionObjects)
    {
        if (collisionObject == this) continue;
        
        result = mCollisionShape->isCollidingWithShapeAtLocation(inLocation, collisionObject->getCollisionShape(), collisionObject->getLocation());
        if (result.bCollided)
        {
            break;
        }
    }

    return result;
}

CollisionResult CollisionObject::getMoveCollisionResult(const float deltaTime) const
{
    return getCollisionResultOnLocation(getMoveLocation(deltaTime));
}
