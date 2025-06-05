#pragma once
#include <memory>

#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"

class Application
{
    
private:

    SDL_Window* m_Window = nullptr;
    SDL_Renderer* m_Renderer = nullptr;

public:

    Application(const char* title, int width, int height);
    ~Application();
    
};
