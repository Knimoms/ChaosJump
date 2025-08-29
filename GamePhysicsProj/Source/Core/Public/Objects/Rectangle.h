#pragma once
#include "Physics/CollisionObject.h"
#include "Render/DrawableInterface.h"

class Rectangle : public CollisionObject, public DrawableInterface
{

private:

    float mWidth = 0.f;
    float mHeight = 0.f;
    
    Vector2 mRectExtend = {};
    
public:

    Rectangle(float width, float height);

    //~ Begin DrawableInterface Interface
    void draw(SDL_Renderer* renderer, const Vector2& viewLocation) override;
    bool shouldBeCulled(const Vector2& viewLocation, const Vector2& windowSize) const override;
    //~ End DrawableInterface Interface
    
};
