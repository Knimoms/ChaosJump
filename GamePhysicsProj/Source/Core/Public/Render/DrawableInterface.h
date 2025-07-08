#pragma once
#include <vector>

struct Color
{
    float r,g,b;
};

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

    virtual void draw(class SDL_Renderer* renderer) = 0;
    
};
