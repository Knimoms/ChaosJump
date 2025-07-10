#include "Objects/Polygon.h"

#include "Physics/CollisionShapes/PolygonShape.h"
#include "SDL3/SDL_render.h"

Polygon::Polygon(const std::vector<Vector2>& vertices) : DrawableInterface({.r = 0.f, .g = 1.f, .b = 0}), mVertices(vertices)
{
    setCollisionShape(new PolygonShape(vertices));
}

void Polygon::draw(SDL_Renderer* renderer)
{
    const auto [r, g, b] = getColor();
    
    SDL_Vertex centerVertex;
    centerVertex.position = {mLocation.x, mLocation.y};
    centerVertex.color = {r, g, b};
    centerVertex.tex_coord = {0.f, 0.f};
    
    std::vector vertices = {centerVertex};
    std::vector<int> indices;

    const int vertexCount = mVertices.size(); 
    for (int i = 0; i < vertexCount; ++i)
    {
        const Vector2& vertexVector = mLocation + mVertices[i];
        
        SDL_Vertex vertex;
        vertex.position = {vertexVector.x, vertexVector.y};
        vertex.color = {r, g, b};
        vertex.tex_coord = {0.f, 0.f};
        vertices.push_back(vertex);

        if (!i) continue;

        indices.push_back(0);   
        indices.push_back(i + 1);   
        indices.push_back(i);
    }

    indices.push_back(0);   
    indices.push_back(vertexCount);     
    indices.push_back(1);
    
    SDL_RenderGeometry(renderer, nullptr, vertices.data(), static_cast<int>(vertices.size()), indices.data(), static_cast<int>(indices.size()));
}
