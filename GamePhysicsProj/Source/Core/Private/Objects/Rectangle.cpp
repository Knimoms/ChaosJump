#include "Objects/Rectangle.h"

#include "Physics/CollisionShapes/CircleShape.h"
#include "Physics/CollisionShapes/RectangleShape.h"
#include "SDL3/SDL_render.h"

Rectangle::Rectangle(const float width, const float height) : DrawableInterface({0, 1, 0}), mWidth(width), mHeight(height)
{
    mRectExtend = {.x = width/2.f, .y = height/2.f};
    setArea(mWidth * mHeight);
    setCollisionShape(new RectangleShape(mRectExtend));
}

void Rectangle::draw(SDL_Renderer* renderer)
{
    const auto [r, g, b] = getColor();
    SDL_SetRenderDrawColorFloat(renderer, r, g, b, SDL_ALPHA_OPAQUE);

    const SDL_FRect positionedRect = {mLocation.x - mRectExtend.x, mLocation.y - mRectExtend.y, mWidth, mHeight};
    SDL_RenderFillRect(renderer, &positionedRect);
}