#pragma once
#include "Physics/CollisionObject.h"
#include "Render/DrawableInterface.h"
#include "SDL3/SDL_render.h"

class Polygon : public CollisionObject, public DrawableInterface
{

private:

    std::vector<SDL_Vertex> mVertices = {};
    std::vector<int> mIndices = {};

public:

    Polygon(const std::vector<Vector2>& vertices);

    //~ Begin DrawableInterface Interface
    void draw(SDL_Renderer* renderer) override;
    //~ End DrawableInterface Interface
    
};
