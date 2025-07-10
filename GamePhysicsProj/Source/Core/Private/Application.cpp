#include "Application.h"

#include "iostream"
#include "format"
#include "SDL3/SDL.h"
#include "Input/InputRouter.h"
#include "Objects/Circle.h"
#include "Objects/Rectangle.h"
#include <random>

#include "Objects/Polygon.h"

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

template<class ObjectType>
std::shared_ptr<ObjectType> createRandomCollisionObject()
{
    return std::make_shared<ObjectType>();
}

template<>
std::shared_ptr<Circle> createRandomCollisionObject()
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<> sizeDist(25, 75);
    std::uniform_int_distribution<> colorDist(0,SDL_MAX_UINT8);

    unsigned int size = sizeDist(rng);
    std::shared_ptr<Circle> circle = std::make_shared<Circle>(size);

    const Vector2& windowsSize = Application::getApplication().getWindowSize();

    float r = static_cast<float>(colorDist(rng)) / SDL_MAX_UINT8;
    float g = static_cast<float>(colorDist(rng)) / SDL_MAX_UINT8;
    float b = static_cast<float>(colorDist(rng)) / SDL_MAX_UINT8;
    circle->setColor({.r = r, .g = g, .b = b});
    std::uniform_int_distribution<> locHeightDist(size,windowsSize.y - size);
    std::uniform_int_distribution<> locWidthDist(size,windowsSize.x - size);

    Vector2 location = {static_cast<float>(locWidthDist(rng)), static_cast<float>(locHeightDist(rng))};
    circle->setLocation(location);

    std::uniform_int_distribution<> velocityDist(-1000,1000);
    circle->setVelocity({static_cast<float>(velocityDist(rng)), static_cast<float>(velocityDist(rng))});

    if (circle->getCurrentCollisionResult().bCollided)
    {
        return nullptr;
    }

    return circle;
}

void Application::run()
{
    bRunning = true;
    std::vector<std::shared_ptr<Circle>> circles;

    constexpr uint32_t circleCount = 0;

    for (uint32_t i = 0; i < circleCount; ++i)
    {
        std::shared_ptr<Circle> circle = createRandomCollisionObject<Circle>();

        if (!circle)
        {
            --i;
            continue;
        }

        circles.push_back(circle);
    }

    
    Polygon p({{50, 50} , {50, -70}, {-50, -50}, {-50, 50}});
    p.setLocation({400, 400});
    p.setVelocity({1000, 0});
    p.setMass(50);
    
    Polygon p2({{100, 50} , {50, -80}, {-50, 50}});
    p2.setLocation({1000, 450});
    p2.setVelocity({-500, 0});
    p2.setMass(50);
    
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

void Application::addDebugLine(const DebugLine& debugLine)
{
    mDebugLines.push_back(debugLine);
}

void Application::tickObjects(float deltaSeconds)
{
    
    for (CollisionObject* collisionObject : CollisionObject::sCollisionObjects)
    {
        collisionObject->tick(deltaSeconds);
    }
}

void SetRenderDrawColor(SDL_Renderer* renderer, const Color& color)
{
    const auto [r, g, b] = color;
    SDL_SetRenderDrawColorFloat(renderer, r, g, b, SDL_ALPHA_OPAQUE);
}

void Application::drawFrame()
{
    SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(mRenderer);
    
    for (DrawableInterface* drawables : DrawableInterface::sDrawables)
    {
        drawables->draw(mRenderer);
    }

    for (const DebugLine& debugLine : mDebugLines)
    {
        const auto [start, end, color] = debugLine;
        SetRenderDrawColor(mRenderer, color);
        SDL_RenderLine(mRenderer, start.x, start.y, end.x, end.y);
    }

    mDebugLines.clear();
    
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
