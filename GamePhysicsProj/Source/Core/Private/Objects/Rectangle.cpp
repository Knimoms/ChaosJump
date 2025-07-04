#include "Objects/Rectangle.h"

#include "Physics/CollisionShapes/CircleShape.h"
#include "SDL3/SDL_render.h"

Rectangle::Rectangle(const float width, const float height) : DrawableInterface({0, 255, 0, 0}), mWidth(width), mHeight(height)
{
    mRectExtend = {.x = width, .y = height };
    setCollisionShape(new CircleShape(width/2));
}

void Rectangle::draw(SDL_Renderer* renderer)
{
    const SDL_FRect positionedRect = {mLocation.x, mLocation.y, mRectExtend.x, mRectExtend.y};
    SDL_RenderFillRect(renderer, &positionedRect);
}