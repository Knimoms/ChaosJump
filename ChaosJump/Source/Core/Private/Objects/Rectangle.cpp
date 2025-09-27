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

void Rectangle::draw(SDL_Renderer* renderer, const Vector2& viewLocation)
{
    const auto [r, g, b] = getColor();
    SDL_SetRenderDrawColorFloat(renderer, r, g, b, SDL_ALPHA_OPAQUE);

    const Vector2 screenLocation = getScreenLocationForView(viewLocation);
    const SDL_FRect positionedRect = {screenLocation.x - mRectExtend.x, screenLocation.y - mRectExtend.y, mWidth, mHeight};
    SDL_RenderFillRect(renderer, &positionedRect);
}

bool Rectangle::shouldBeCulled(const Vector2& viewLocation, const Vector2& windowSize) const
{
    const Vector2 screenLocation = getScreenLocationForView(viewLocation);

    const bool outOfXBounds = screenLocation.x + mWidth < 0 || screenLocation.x - mWidth > windowSize.x;
    const bool outOfYBounds = screenLocation.y + mHeight < 0 || screenLocation.y - mHeight > windowSize.y;

    return outOfXBounds || outOfYBounds;
}
