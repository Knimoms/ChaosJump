#include "Objects/Circle.h"
#define _USE_MATH_DEFINES

#include "Physics/CollisionShapes/CircleShape.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"

Circle::Circle(float radius) : DrawableInterface({.r = 255, .g = 0, .b = 0, .a = 0}), mRadius(radius)
{
    setCollisionShape(new CircleShape(mRadius));
}

void Circle::draw(SDL_Renderer* renderer)
{
    constexpr float stepsize = 0.1f;
    constexpr double twoPI = M_PI * 2;

    float prevX = mLocation.x, prevY = mLocation.y;
    const auto [r, g, b, a] = getColor();
    
    SDL_Vertex centerVertex;
    centerVertex.position = {mLocation.x, mLocation.y};
    centerVertex.color = {static_cast<float>(r), static_cast<float>(g), static_cast<float>(b), static_cast<float>(a)};
    centerVertex.tex_coord = {0.f, 0.f};
    
    std::vector vertices = {centerVertex};
    std::vector<int> indices;

    constexpr int steps = 360;
    constexpr float anglePerStep = 2.f * static_cast<float>((M_PI / steps));
    
    for(unsigned int i = 0; i < steps + 1; ++i)
    {
        const float angle = i * anglePerStep;
        float xCoord = SDL_cosf(angle) * mRadius + mLocation.x;
        float yCoord = SDL_sinf(angle) * mRadius + mLocation.y;

        SDL_Vertex vertex1 = centerVertex;
        vertex1.position = {xCoord, yCoord};

        vertices.push_back(vertex1);

        int verticesSize = static_cast<int>(vertices.size());
        if (verticesSize < 3) continue;

        indices.push_back(0);
        indices.push_back(verticesSize - 1);
        indices.push_back(verticesSize - 2);
    }
    
    SDL_RenderGeometry(renderer, nullptr, vertices.data(), static_cast<int>(vertices.size()), indices.data(), static_cast<int>(indices.size()));
}