#pragma once
#include "Physics/CollisionObject.h"
#include "Render/DrawableInterface.h"

class Circle : public CollisionObject, public DrawableInterface
{

private:

    float mRadius = 0.f;
    
public:

    Circle(float radius);

    //~ Begin DrawableInterface Interface
    void draw(SDL_Renderer* renderer) override;
    //~ End DrawableInterface Interface
    
};
