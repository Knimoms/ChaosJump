#include "Objects/Circle.h"
#define _USE_MATH_DEFINES

#include <corecrt_math_defines.h>

#include "Physics/CollisionShapes/CircleShape.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"

Circle::Circle(float radius) : DrawableInterface({.r = 1.f, .g = 0, .b = 0}), mRadius(radius)
{
    setCollisionShape(new CircleShape(mRadius));
    setArea(M_PI * (mRadius * mRadius));

    const auto [r, g, b] = getColor();
    
    SDL_Vertex centerVertex;
    centerVertex.position = {0, 0};
    centerVertex.color = {r, g, b, 0};
    centerVertex.tex_coord = {0.f, 0.f};
    
    mVertices = { centerVertex };

    constexpr int steps = 16;
    constexpr float anglePerStep = 2.f * static_cast<float>((M_PI / steps));
    
    for(unsigned int i = 0; i < steps + 1; ++i)
    {
        const float angle = i * anglePerStep;
        float xCoord = SDL_cosf(angle) * mRadius + mLocation.x;
        float yCoord = SDL_sinf(angle) * mRadius + mLocation.y;

        SDL_Vertex vertex1 = centerVertex;
        vertex1.position = {xCoord, yCoord};

        mVertices.push_back(vertex1);

        int verticesSize = static_cast<int>(mVertices.size());
        if (verticesSize < 3) continue;

        mIndices.push_back(0);
        mIndices.push_back(verticesSize - 1);
        mIndices.push_back(verticesSize - 2);
    }
}

void Circle::draw(SDL_Renderer* renderer, const Vector2& viewLocation)
{
    const auto [r, g, b] = getColor();
    
    std::vector translatedVertices = mVertices;

    const Vector2 screenLocation = getScreenLocationForView(viewLocation);

    for(SDL_Vertex& vertex : translatedVertices)
    {
        vertex.color = {r, g, b, 0};
        vertex.position.x += screenLocation.x;
        vertex.position.y += screenLocation.y;
    }
    
    SDL_RenderGeometry(renderer, nullptr, translatedVertices.data(), static_cast<int>(translatedVertices.size()), mIndices.data(), static_cast<int>(mIndices.size()));

}

bool Circle::shouldBeCulled(const Vector2& viewLocation, const Vector2& windowSize) const
{
    const Vector2 screenLocation = getScreenLocationForView(viewLocation);

    const bool outOfXBounds = screenLocation.x + mRadius < 0 || screenLocation.x - mRadius > windowSize.x;
    const bool outOfYBounds = screenLocation.y + mRadius < 0 || screenLocation.y - mRadius > windowSize.y;

    return outOfXBounds || outOfYBounds;
}
