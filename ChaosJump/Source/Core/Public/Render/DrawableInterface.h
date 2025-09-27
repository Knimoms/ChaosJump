#pragma once
#include <vector>

struct Vector2;

struct Color
{
    float r,g,b;
};

class SDL_Renderer;

class DrawableInterface
{

private:

    Color mColor = {.r = 0, .g = 0, .b = 0};
    
public:

    DrawableInterface(Color color);

    const Color& getColor() const { return mColor; }
    void setColor(Color color);

    static std::vector<DrawableInterface*> sDrawables;
    
    virtual ~DrawableInterface();

    virtual void draw(SDL_Renderer* renderer, const Vector2& viewLocation) = 0;
    virtual bool shouldBeCulled(const Vector2& viewLocation, const Vector2& windowSize) const = 0;
    
};
