#pragma once
#include "Physics/CollisionObject.h"
#include "Render/DrawableInterface.h"
#include "SDL3/SDL_render.h"

class Circle : public CollisionObject, public DrawableInterface
{

private:

    float mRadius = 0.f;
    
    std::vector<SDL_Vertex> mVertices = {};
    std::vector<int> mIndices = {};
    
public:

    Circle(float radius);

    //~ Begin DrawableInterface Interface
    void draw(SDL_Renderer* renderer, const Vector2& viewLocation) override;
    bool shouldBeCulled(const Vector2& viewLocation, const Vector2& windowSize) const override;
    //~ End DrawableInterface Interface
    
};
