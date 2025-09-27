#include <algorithm>

#include "Objects/Polygon.h"

#include "Physics/CollisionShapes/PolygonShape.h"
#include "SDL3/SDL_render.h"

Polygon::Polygon(const std::vector<Vector2>& vertices) : DrawableInterface({.r = 0.f, .g = 1.f, .b = 0})
{
    const auto [r, g, b] = getColor();
    
    SDL_Vertex centerVertex {{0, 0}, {r, g, b, 0}, {0, 0}};
    centerVertex.position = {.x = 0, .y = 0};
    centerVertex.tex_coord = {.x = 0.f, .y = 0.f};
    
    mVertices = {centerVertex};

    float area = 0.f;

    const int vertexCount = vertices.size(); 
    for (int i = 0; i < vertexCount; ++i)
    {
        int j = (i + 1) % vertexCount;

        const Vector2& vertexVector = vertices[i];
        const Vector2& nextVector = vertices[j];
        
        area += vertexVector.x * nextVector.y;
        area -= nextVector.x * vertexVector.y;
        
        mFurthestVertexDistance = std::max(mFurthestVertexDistance, vertexVector.size());

        SDL_Vertex vertex;
        vertex.position = {vertexVector.x, vertexVector.y};
        vertex.color = {r, g, b};
        vertex.tex_coord = {0.f, 0.f};
        mVertices.push_back(vertex);

        if (!i) continue;

        mIndices.push_back(0);   
        mIndices.push_back(i + 1);   
        mIndices.push_back(i);
    }

    setArea(std::abs(area) * 0.5f);

    mIndices.push_back(0);   
    mIndices.push_back(vertexCount);     
    mIndices.push_back(1);
    
    setCollisionShape(new PolygonShape(vertices));
}

void Polygon::draw(SDL_Renderer* renderer, const Vector2& viewLocation)
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

bool Polygon::shouldBeCulled(const Vector2& viewLocation, const Vector2& windowSize) const
{
    const Vector2 screenLocation = getScreenLocationForView(viewLocation);

    const bool outOfXBounds = screenLocation.x + mFurthestVertexDistance < 0 || screenLocation.x - mFurthestVertexDistance > windowSize.x;
    const bool outOfYBounds = screenLocation.y + mFurthestVertexDistance < 0 || screenLocation.y - mFurthestVertexDistance > windowSize.y;

    return outOfXBounds || outOfYBounds;
}
