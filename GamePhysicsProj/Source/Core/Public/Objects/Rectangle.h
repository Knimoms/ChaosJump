#pragma once
#include "Physics/CollisionObject.h"
#include "Render/DrawableInterface.h"
#include "SDL3/SDL_rect.h"

class Rectangle : public CollisionObject, public DrawableInterface
{

private:

    float mWidth = 0.f;
    float mHeight = 0.f;

    Vector2 mRectExtend;
    
public:

    Rectangle(float width, float height);

    //~ Begin DrawableInterface Interface
    void draw(SDL_Renderer* renderer) override;
    //~ End DrawableInterface Interface
    
};
