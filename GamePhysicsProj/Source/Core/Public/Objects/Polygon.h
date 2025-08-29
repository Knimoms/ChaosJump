#pragma once
#include "Physics/CollisionObject.h"
#include "Render/DrawableInterface.h"
#include "SDL3/SDL_render.h"

class Polygon : public CollisionObject, public DrawableInterface
{

private:

    std::vector<SDL_Vertex> mVertices = {};
    std::vector<int> mIndices = {};

    float mFurthestVertexDistance = 0.f;

public:

    Polygon(const std::vector<Vector2>& vertices);

    //~ Begin DrawableInterface Interface
    void draw(SDL_Renderer* renderer, const Vector2& viewLocation) override;
    bool shouldBeCulled(const Vector2& viewLocation, const Vector2& windowSize) const override;
    //~ End DrawableInterface Interface
    
};
