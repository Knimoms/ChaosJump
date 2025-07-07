#include "Application.h"

#include "iostream"
#include "format"
#include "SDL3/SDL.h"
#include "Input/InputRouter.h"
#include "Objects/Circle.h"
#include "Objects/Rectangle.h"

#define PRINT_SDL_ERROR(ErrorContext) std::cout << (ErrorContext) << std::format(": %s\n", SDL_GetError());
#define SDL_FLAGS SDL_INIT_VIDEO

ApplicationParams Application::sApplicationParams;

Application::Application(const ApplicationParams& params) : mInputRouter(std::make_shared<InputRouter>())
{
    const auto [title, width, height, renderDriver, fps] = params;

    mWindowSize = {.x = static_cast<float>(width), .y = static_cast<float>(height)};
    mFrameTime = fps ? 1000 / fps : 0;

    if (!SDL_Init(SDL_FLAGS))
    {
        PRINT_SDL_ERROR("Error initializing SDL")
        return;
    }
    
    mWindow = SDL_CreateWindow(title, width, height, 0);

    if (!mWindow)
    {
        PRINT_SDL_ERROR("Error creating Window")
        return;
    }

    mRenderer = SDL_CreateRenderer(mWindow, renderDriver);

    if (!mRenderer)
    {
        PRINT_SDL_ERROR("Error creating Renderer")
    }    
}

Application::~Application()
{
    if (mRenderer)
    {
        SDL_DestroyRenderer(mRenderer);
        mRenderer = nullptr;
    }
    
    if (mWindow)
    {
        SDL_DestroyWindow(mWindow);
        mWindow = nullptr;
    }
    
    SDL_Quit();
}

Application& Application::initApplication(const ApplicationParams& params)
{
    sApplicationParams = params;
    return getApplication();
}

Application& Application::getApplication()
{
    static Application app(sApplicationParams);
    return app;
}

void Application::run()
{
    bRunning = true;

    Circle circle1(100);
    circle1.setVelocity({750, 25});
    circle1.setLocation({150, 400});
    circle1.setMass(100);
    circle1.setColor({255, 0, 0});

    Circle circle2(25);
    circle2.setVelocity({-100, 100});
    circle2.setLocation({400, 400});
    circle2.setMass(25);
    circle2.setColor({0, 25, 255});

    Circle circle3(10);
    circle3.setVelocity({-100, 100});
    circle3.setLocation({350, 400});
    circle3.setMass(10);
    circle3.setColor({10, 255, 255});

    Circle circle4(50);
    circle4.setVelocity({-100, 100});
    circle4.setLocation({300, 400});
    circle4.setMass(50);
    circle4.setColor({0, 255, 50});
    
    Circle circle5(75);
    circle5.setVelocity({-100, 100});
    circle5.setLocation({400, 300});
    circle5.setMass(75);
    circle5.setColor({0, 255, 75});

    uint64_t now = SDL_GetPerformanceCounter();
    uint64_t last = 0;

    while (bRunning)
    {
        ++frameCount;
        last = now;
        now = SDL_GetPerformanceCounter();
        const float deltaTime = static_cast<float>(now - last) / SDL_GetPerformanceFrequency();
        
        pollEvents();
        tickObjects(deltaTime);
        drawFrame();

        if (mFrameTime)
        {
            SDL_Delay(mFrameTime);
        }
    }
}

void Application::tickObjects(float deltaSeconds)
{
    
    for (CollisionObject* collisionObject : CollisionObject::sCollisionObjects)
    {
        collisionObject->tick(deltaSeconds);
    }
}

static void SetRenderDrawColor(SDL_Renderer* renderer, const Color& color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void Application::drawFrame()
{
    SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 0);
    SDL_RenderClear(mRenderer);
    
    for (DrawableInterface* drawables : DrawableInterface::sDrawables)
    {
        SetRenderDrawColor(mRenderer, drawables->getColor());
        drawables->draw(mRenderer);
    }
    
    SDL_RenderPresent(mRenderer);
}

void Application::pollEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        handleEvent(event);
    }
}

void Application::handleEvent(const SDL_Event& event)
{
    switch (const uint32_t eventType = event.type)
    {
    case SDL_EVENT_QUIT:
        bRunning = false;
        break;
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:
        mInputRouter->routeKeyEvent(event.key.scancode, SDL_EVENT_KEY_DOWN == eventType);
        break;
    default: ;
    }
}
