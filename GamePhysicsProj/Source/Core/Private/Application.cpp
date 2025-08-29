#include "Application.h"

#include "iostream"
#include "format"
#include "SDL3/SDL.h"
#include "Input/InputRouter.h"
#include "Objects/Circle.h"
#include "Objects/Rectangle.h"
#include <random>
#include <algorithm>

#include "Core/TickableInterface.h"
#include "Debugging/DebugDefinitions.h"
#include "Game/GameMode.h"
#include "Objects/Polygon.h"
#include "Player/Player.h"

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

static float getFPSForFrameTimes(const std::multiset<float>& frameTimes)
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

    std::multiset lows (mFrameTimes.begin(), mFrameTimes.end());
    return getFPSForFrameTimes(lows);
}

void WindowDeleter::operator()(SDL_Window* rawWindow) const
{
    SDL_DestroyWindow(rawWindow);
}

void RendererDeleter::operator()(SDL_Renderer* rawRenderer) const
{
    SDL_DestroyRenderer(rawRenderer);
}

Application::Application(const ApplicationParams& params) : mGameMode(std::make_unique<GameMode>()), mInputRouter(std::make_unique<InputRouter>())
{
    mInputRouter->addInputReceiver(mGameMode.get());
    const auto [title, width, height, renderDriver, fps, bInDrawFPS] = params;

    mWindowSize = {.x = static_cast<float>(width), .y = static_cast<float>(height)};
    mFrameTime = fps ? 1000 / fps : 0;
    bDrawFPS = bInDrawFPS;

    if (!SDL_Init(SDL_FLAGS))
    {
        PRINT_SDL_ERROR("Error initializing SDL")
        return;
    }
    
    mWindow = std::unique_ptr<SDL_Window, WindowDeleter>(SDL_CreateWindow(title, width, height, 0));

    if (!mWindow)
    {
        PRINT_SDL_ERROR("Error creating Window")
        return;
    }

    mRenderer = std::unique_ptr<SDL_Renderer, RendererDeleter>(SDL_CreateRenderer(mWindow.get(), renderDriver));

    if (!mRenderer)
    {
        PRINT_SDL_ERROR("Error creating Renderer")
    }    
}

Application::~Application()
{
    mRenderer.reset();
    mWindow.reset();
    
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
    circle->setCanCollideWithWindowBorder(true, true);
    circle->setGravity({});
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
    std::mt19937 rng{dev()};
    std::uniform_int_distribution<> sizeDist(25, 200);
    std::uniform_int_distribution<> colorDist(0,SDL_MAX_UINT8);

    unsigned int width = sizeDist(rng);
    unsigned int height = sizeDist(rng);
    std::shared_ptr<Rectangle> rectangle = std::make_shared<Rectangle>(width, height);

    const Vector2& windowsSize = Application::getApplication().getWindowSize();

    const float r = static_cast<float>(colorDist(rng)) / SDL_MAX_UINT8;
    const float g = static_cast<float>(colorDist(rng)) / SDL_MAX_UINT8;
    const float b = static_cast<float>(colorDist(rng)) / SDL_MAX_UINT8;
    rectangle->setColor({.r = r, .g = g, .b = b});
    rectangle->setCanCollideWithWindowBorder(true, true);
    rectangle->setGravity({});
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
    //p2.setCanCollideWithWindowBorder(true, true);
    //p2.setGravity({});
    //
    //p2.setLocation({1000, 450});
    //p2.setVelocity({-500, 0});
    
    //constexpr uint32_t circleCount = 2;
    //std::vector<std::shared_ptr<Circle>> circles = createRandomCollisionObjects<Circle>(circleCount);
    //
    //constexpr uint32_t rectangleCount = 2;
    //std::vector<std::shared_ptr<Rectangle>> rectangles = createRandomCollisionObjects<Rectangle>(rectangleCount);

    //Rectangle rectangle {200, 200};
//
//    rectangle.setCanCollideWithWindowBorder(true, true);
//    rectangle.setGravity({});
//
//    rectangle.setLocation({400, 500});
//    rectangle.setVelocity({200, 0});

    /*Rectangle rectangle2 {200, 100};

    rectangle2.setCanCollideWithWindowBorder(true, true);
    rectangle2.setGravity({});

    rectangle2.setLocation({1200, 500});
    rectangle2.setVelocity({-200, 0});*/
    uint64_t now = SDL_GetPerformanceCounter();

    while (bRunning)
    {
        uint64_t last = now;
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

void Application::addDisplayText(const DisplayText& displayText)
{
    mDisplayTexts.push_back(displayText);
}

Vector2 Application::getCurrentViewLocation() const
{
    const Player* player = mGameMode ? mGameMode->getPlayer() : nullptr;
    return player ? player->getViewLocation() : Vector2{};
}

void Application::tickObjects(const float deltaSeconds)
{
    const auto tickables = TickableInterface::sTickables;
    for (TickableInterface* tickable : tickables)
    {
        tickable->tick(deltaSeconds);
    }
}

static void setRenderDrawColor(SDL_Renderer* renderer, const Color& color)
{
    const auto [r, g, b] = color;
    SDL_SetRenderDrawColorFloat(renderer, r, g, b, SDL_ALPHA_OPAQUE);
}

static void drawDisplayText(const DisplayText& displayText, SDL_Renderer* renderer, const Vector2& windowCenter)
{
    auto& [screenPosition, text, color, duration, anchor] = displayText;

    const char *message = text.c_str();

    const Vector2 position = windowCenter + windowCenter * screenPosition;

    const float halfTextWidth = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(message) / 2;
    const float halfTextHeight = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE /2;

    const float x = position.x - halfTextWidth - anchor.x * halfTextWidth;
    const float y = position.y - halfTextHeight - anchor.y * halfTextHeight;
                
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(renderer, x, y, message);
}

void Application::drawFrame(const float deltaTime)
{
    SDL_SetRenderDrawColor(mRenderer.get(), 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(mRenderer.get());

    const Vector2 viewLocation = getCurrentViewLocation();

    for (DrawableInterface* drawable : DrawableInterface::sDrawables)
    {
        if (drawable->shouldBeCulled(viewLocation, mWindowSize)) continue;
        drawable->draw(mRenderer.get(), viewLocation);
    }

#if DRAW_DEBUG_LINES

    for (DebugLine& debugLine : mDebugLines)
    {
        auto& [start, end, color, duration] = debugLine;
        duration -= deltaTime;

        const Vector2 screenStart = start - viewLocation;
        const Vector2 screenEnd = end - viewLocation;
        setRenderDrawColor(mRenderer.get(), color);
        SDL_RenderLine(mRenderer.get(), screenStart.x, screenStart.y, screenEnd.x, screenEnd.y);
    }

    std::erase_if(mDebugLines, [](const DebugLine& debugLine)
    {
        return debugLine.duration < 0.f;
    });
#endif

    
    if (bDrawFPS)
    {
        std::string messageStr = std::format("Current FPS: {:.2f}\n Average FPS: {:.2f}\n", mFrameTracker.getCurrentFPS(), mFrameTracker.getAverageFPS());

        DisplayText displayText;
        displayText.text = messageStr;
        displayText.screenPosition = {.x = 1, .y = -1};
        displayText.alignment ={.x = 1, .y = -1};

        addDisplayText(displayText);
    }
    
    for (DisplayText& displayText : mDisplayTexts)
    {
        displayText.duration -= deltaTime;

        drawDisplayText(displayText, mRenderer.get(), mWindowSize/2);
    }

    std::erase_if(mDisplayTexts, [](const DisplayText& displayText)
    {
        return displayText.duration < 0.f;
    });

    
    SDL_RenderPresent(mRenderer.get());
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