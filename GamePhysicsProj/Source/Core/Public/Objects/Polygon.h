#pragma once
#include "Physics/CollisionObject.h"
#include "Render/DrawableInterface.h"

class Polygon : public CollisionObject, public DrawableInterface
{

private:

    std::vector<Vector2> mVertices = {};

public:

    Polygon(const std::vector<Vector2>& vertices);

    //~ Begin DrawableInterface Interface
    void draw(SDL_Renderer* renderer) override;
    //~ End DrawableInterface Interface
    
};
