#include "Physics/CollisionObject.h"
#include "../../Public/Physics/CollisionObject.h"
#include <cassert>

#include "Application.h"
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

void CollisionObject::moveTick(float deltaTime)
{
    const Vector2 newLocation = mVelocity * deltaTime + mLocation;

    CollisionObject* collidedObject = nullptr;

    const Vector2& windowSize = Application::getApplication().getWindowSize();
    CollisionResult result = mCollisionShape->isCollidingWithWindowBorderAtLocation(newLocation, windowSize);

    if (!result.bCollided)
    {
        for (CollisionObject* collisionObject : sCollisionObjects)
        {
            if (collisionObject == this) continue;
        
            result = mCollisionShape->isCollidingWithShapeAtLocation(newLocation, collisionObject->getCollisionShape(), collisionObject->getLocation());
            if (result.bCollided)
            {
                collidedObject = collisionObject;
                break;
            }
        }
    }
    
    if (!result.bCollided)
    {
        mLocation = newLocation;
        return;
    }
    
    const float counterMass = collidedObject ? collidedObject->mMass : static_cast<float>(1LL << 63);
    Vector2 counterVeloctiy = collidedObject ? collidedObject->mVelocity : Vector2{.x = 0.f, .y = 0.f};

    const Vector2 newVelocity = computeElasticCollision(mMass, counterMass, mVelocity, counterVeloctiy, result.collisionNormal);
    const Vector2 otherVelocity = computeElasticCollision(counterMass, mMass, counterVeloctiy, mVelocity, -1 * result.collisionNormal);
    
    setVelocity(newVelocity);

    if (collidedObject)
    {
        collidedObject->setVelocity(otherVelocity);
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

void CollisionObject::setMass(float mass)
{
    mMass = mass;
}
