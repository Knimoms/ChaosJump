#include "Application.h"

#include "iostream"
#include "format"
#include "SDL3/SDL.h"
#include "Input/InputRouter.h"
#include "Objects/Circle.h"
#include "Objects/Rectangle.h"
#include <random>
#include <algorithm>

#include "ControllableRectangle.h"
#include "Debugging/DebugDefinitions.h"

#define PRINT_SDL_ERROR(ErrorContext) std::cout << (ErrorContext) << std::format(": %s\n", SDL_GetError());
#define SDL_FLAGS SDL_INIT_VIDEO

ApplicationParams Application::sApplicationParams;

void FrameTracker::pushFrameTime(const float frameTime)
{
    lastFrameTime = frameTime;
    mFrameTimes.insert(frameTime);
    
    mTotalFrameTime += frameTime;
    ++mFrameCounter;
}

float getFPSForFrameTimes(const std::multiset<float>& frameTimes)
{
    float totalTime = 0.f;
    for (const float frameTime : frameTimes)
    {
        totalTime += frameTime;
    }

    return frameTimes.size() / totalTime;
}

float FrameTracker::getCurrentFPS() const
{
    return 1 / lastFrameTime;
}

float FrameTracker::getAverageFPS() const
{
    return mFrameCounter / mTotalFrameTime;
}

float FrameTracker::getLowestPercentageFPS(float fraction) const
{
    fraction = std::clamp(fraction, 0.f, 1.f);

    const size_t framesNum = mFrameTimes.size();
    const int32_t fractionNum = framesNum * fraction;

    std::multiset<float> lows (mFrameTimes.begin(), mFrameTimes.end());
    return getFPSForFrameTimes(lows);
}

Application::Application(const ApplicationParams& params) : mInputRouter(std::make_shared<InputRouter>())
{
    const auto [title, width, height, renderDriver, fps, bInDrawFPS] = params;

    mWindowSize = {.x = static_cast<float>(width), .y = static_cast<float>(height)};
    mFrameTime = fps ? 1000 / fps : 0;
    bDrawFPS = bInDrawFPS;

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
static std::shared_ptr<ObjectType> createRandomCollisionObject()
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

    const float r = static_cast<float>(colorDist(rng)) / SDL_MAX_UINT8;
    const float g = static_cast<float>(colorDist(rng)) / SDL_MAX_UINT8;
    const float b = static_cast<float>(colorDist(rng)) / SDL_MAX_UINT8;
    circle->setColor({.r = r, .g = g, .b = b});
    std::uniform_int_distribution<> locHeightDist(size,windowsSize.y - size);
    std::uniform_int_distribution<> locWidthDist(size,windowsSize.x - size);

    const Vector2 location = {static_cast<float>(locWidthDist(rng)), static_cast<float>(locHeightDist(rng))};
    circle->setLocation(location);

    std::uniform_int_distribution<> velocityDist(-1000,1000);
    circle->setVelocity({static_cast<float>(velocityDist(rng)), static_cast<float>(velocityDist(rng))});

    if (circle->getCurrentCollisionResult().bCollided)
    {
        return nullptr;
    }

    return circle;
}

template<>
std::shared_ptr<Rectangle> createRandomCollisionObject()
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<> sizeDist(25, 75);
    std::uniform_int_distribution<> colorDist(0,SDL_MAX_UINT8);

    unsigned int width = sizeDist(rng);
    unsigned int height = sizeDist(rng);
    std::shared_ptr<Rectangle> rectangle = std::make_shared<Rectangle>(width, height);

    const Vector2& windowsSize = Application::getApplication().getWindowSize();

    const float r = static_cast<float>(colorDist(rng)) / SDL_MAX_UINT8;
    const float g = static_cast<float>(colorDist(rng)) / SDL_MAX_UINT8;
    const float b = static_cast<float>(colorDist(rng)) / SDL_MAX_UINT8;
    rectangle->setColor({.r = r, .g = g, .b = b});
    std::uniform_int_distribution<> locHeightDist(height,windowsSize.y - height);
    std::uniform_int_distribution<> locWidthDist(width,windowsSize.x - width);

    const Vector2 location = {static_cast<float>(locWidthDist(rng)), static_cast<float>(locHeightDist(rng))};
    rectangle->setLocation(location);

    std::uniform_int_distribution<> velocityDist(-1000,1000);
    rectangle->setVelocity({static_cast<float>(velocityDist(rng)), static_cast<float>(velocityDist(rng))});

    if (rectangle->getCurrentCollisionResult().bCollided)
    {
        return nullptr;
    }

    return rectangle;
}

template<class ObjectType>
static std::vector<std::shared_ptr<ObjectType>> createRandomCollisionObjects(const uint32_t numObjects)
{
    std::vector<std::shared_ptr<ObjectType>> objects{};

    for (uint32_t i = 0; i < numObjects; ++i)
    {
        std::shared_ptr<ObjectType> object = createRandomCollisionObject<ObjectType>();

        if (!object)
        {
            --i;
            continue;
        }

        objects.push_back(object);
    }

    return objects;
}

void Application::run()
{
    bRunning = true;

    //Polygon p({{50, 50} , {50, -70}, {-50, -50}, {-50, 50}});
    //p.setLocation({400, 400});
    //p.setVelocity({1000, 0});
    //p.setMass(50);
    //
    //Polygon p2({{100, 50} , {50, -80}, {-50, 50}});
    //p2.setLocation({1000, 450});
    //p2.setVelocity({-500, 0});
    //p2.setMass(50);
    
    constexpr uint32_t circleCount = 1;
    std::vector<std::shared_ptr<Circle>> circles = createRandomCollisionObjects<Circle>(circleCount);
    
    constexpr uint32_t rectangleCount = 0;
    std::vector<std::shared_ptr<Rectangle>> rectangles = createRandomCollisionObjects<Rectangle>(rectangleCount);

    const Vector2 size {.x = 100, .y = 100};
    const Vector2 startPosition {.x = 100, .y = 100};
    std::shared_ptr<ControllableRectangle> controllableRect = std::make_shared<ControllableRectangle>(size, startPosition);

    mInputRouter->addInputReceiver(controllableRect);
    uint64_t now = SDL_GetPerformanceCounter();
    uint64_t last = 0;

    while (bRunning)
    {
        last = now;
        now = SDL_GetPerformanceCounter();
        const float deltaTime = static_cast<float>(now - last) / SDL_GetPerformanceFrequency();

        mFrameTracker.pushFrameTime(deltaTime);
        
        pollEvents();
        tickObjects(deltaTime);
        drawFrame(deltaTime);

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

static void setRenderDrawColor(SDL_Renderer* renderer, const Color& color)
{
    const auto [r, g, b] = color;
    SDL_SetRenderDrawColorFloat(renderer, r, g, b, SDL_ALPHA_OPAQUE);
}

void Application::drawFrame(const float deltaTime)
{
    SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(mRenderer);
    
    for (DrawableInterface* drawables : DrawableInterface::sDrawables)
    {
        drawables->draw(mRenderer);
    }

#if DRAW_DEBUG_LINES

    for (DebugLine& debugLine : mDebugLines)
    {
        auto& [start, end, color, duration] = debugLine;
        duration -= deltaTime;
        setRenderDrawColor(mRenderer, color);
        SDL_RenderLine(mRenderer, start.x, start.y, end.x, end.y);
    }

    std::erase_if(mDebugLines, [](const DebugLine& debugLine)
    {
        return debugLine.duration < 0.f;
    });

#endif

    
    if (bDrawFPS)
    {
        //SetRenderDrawColor(mRenderer, {1, 1, 1});
        //std::string messageStr = std::format("Current FPS: {:.2f}\n Average FPS: {:.2f}\n Low 1% FPS: {:.2f}\n Low 0.1 FPS: {:.2f}", mFrameTracker.getCurrentFPS(), mFrameTracker.getAverageFPS(), mFrameTracker.getLowestPercentageFPS(0.01f), mFrameTracker.getLowestPercentageFPS(0.001f));

        std::string messageStr = std::format("Current FPS: {:.2f}\n Average FPS: {:.2f}\n", mFrameTracker.getCurrentFPS(), mFrameTracker.getAverageFPS());
        //
        //const auto [width, height] = getWindowSize();
        //SDL_RenderDebugText(mRenderer, width / 2, height / 2, "Test Hello");

        const char *message = messageStr.c_str();
                
        const float x = mWindowSize.x - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(message);
        const float y = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE;

        SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
        SDL_RenderDebugText(mRenderer, x, y, message);
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