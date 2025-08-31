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

void Application::run()
{
    bRunning = true;
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

void Application::tickObjects(const float deltaSeconds) const
{
    if (bPaused) return;
    
    TickableInterface::bTickInProgress = true;
    std::vector<int> destroyedTickableIndices;

    const size_t tickableNum = TickableInterface::sTickables.size();
    for (size_t i = tickableNum - 1; i < std::numeric_limits<size_t>::max(); --i)
    {
        TickableInterface* tickable = TickableInterface::sTickables[i];
        if (!tickable)
        {
            destroyedTickableIndices.push_back(static_cast<int>(i));
            continue;
        }
        
        tickable->tick(deltaSeconds);
    }

    for (const int i: destroyedTickableIndices)
    {
        TickableInterface::sTickables.erase(TickableInterface::sTickables.begin() + i);
    }

    while (!TickableInterface::sTickablesCreatedInTick.empty())
    {
        TickableInterface::sTickables.push_back(TickableInterface::sTickablesCreatedInTick.top());
        TickableInterface::sTickablesCreatedInTick.pop();
    }

    TickableInterface::bTickInProgress = false;
}

static void setRenderDrawColor(SDL_Renderer* renderer, const Color& color)
{
    const auto [r, g, b] = color;
    SDL_SetRenderDrawColorFloat(renderer, r, g, b, SDL_ALPHA_OPAQUE);
}

static void drawDisplayText(const DisplayText& displayText, SDL_Renderer* renderer, Vector2 windowCenter)
{
    auto& [screenPosition, text, color, duration, textScale, anchor] = displayText;

    windowCenter /= textScale;

    const Vector2 position = windowCenter + windowCenter * screenPosition;

    const char* textCharPtr = text.c_str();
    const float halfTextWidth = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(textCharPtr) / 2;
    const float halfTextHeight = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE / 2;

    const float x = position.x - halfTextWidth - anchor.x * halfTextWidth;
    const float y = position.y - halfTextHeight - anchor.y * halfTextHeight;
                
    SDL_SetRenderDrawColor(renderer, color.r * 255, color.g * 255, color.b * 255, 255);
    SDL_SetRenderScale(renderer, textScale.x, textScale.y);
    SDL_RenderDebugText(renderer, x, y, textCharPtr);
    SDL_SetRenderScale(renderer, 1.f, 1.f);
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
        displayText.text = messageStr.c_str();
        displayText.screenPosition = {.x = 1, .y = -1};
        displayText.alignment ={.x = 1, .y = -1};

        addDisplayText(displayText);
    }

    if (bPaused)
    {
        DisplayText pausedText;
        pausedText.text = "PAUSED";
        pausedText.screenPosition = {.x = -1, .y = -1};
        pausedText.alignment ={.x = -1, .y = -1};
        pausedText.textScale ={.x = 1.25, .y = 1.25};

        addDisplayText(pausedText);
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
        {
            const SDL_Scancode scancode = event.key.scancode;
            const bool bPressed = SDL_EVENT_KEY_DOWN == eventType;

            if (scancode == SDL_SCANCODE_ESCAPE && bPressed)
            {
                bPaused = !bPaused;
            }
            else if (!bPaused)
            {
                mInputRouter->routeKeyEvent(scancode, bPressed);
            }
        }
        break;
    default: ;
    }
}