#include "Objects/Platform.h"

Platform::Platform() : Rectangle(200, 50)
{
    setCollisionCategory(CollisionCategory::Ground);
    setCanMove(false);
}