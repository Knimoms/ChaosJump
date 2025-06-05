#include "Application.h"

#include "iostream"
#include "format"
#include "SDL3/SDL.h"

#define PRINT_SDL_ERROR(ErrorContext) std::cout << (ErrorContext) << std::format(": %s\n", SDL_GetError());
#define SDL_FLAGS SDL_INIT_VIDEO

Application::Application(const char* title, const int width, const int height)
{
    if (!SDL_Init(SDL_FLAGS))
    {
        PRINT_SDL_ERROR("Error initializing SDL")
        return;
    }
    
    m_Window = SDL_CreateWindow(title, width, height, 0);

    if (!m_Window)
    {
        PRINT_SDL_ERROR("Error creating Window")
        return;
    }

    m_Renderer = SDL_CreateRenderer(m_Window, nullptr);

    if (!m_Renderer)
    {
        PRINT_SDL_ERROR("Error creating Renderer")
        return;
    }

    SDL_Delay(5000);

    
}

Application::~Application()
{
    if (m_Renderer)
    {
        SDL_DestroyRenderer(m_Renderer);
        m_Renderer = nullptr;
    }
    
    if (m_Window)
    {
        SDL_DestroyWindow(m_Window);
        m_Window = nullptr;
    }
    
    SDL_Quit();
}
