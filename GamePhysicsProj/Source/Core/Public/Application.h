#pragma once
#include <memory>
#include <set>
#include <vector>

#include "Math/Vector2.h"
#include "Render/DrawableInterface.h"
#include "SDL3/SDL_stdinc.h"

class CollisionShapeInterface;

struct ApplicationParams
{

    const char* title = "";
    int width = 1280;
    int height = 720;
    const char* renderDriver = "opengl";
    // 0 unlimits FPS
    uint32_t framesPerSecond = 60;

    bool bDrawFPS = true;
    
};

class SDL_Window;
class SDL_Renderer;
class InputRouter;
union SDL_Event;

struct DebugLine
{
    Vector2 start;
    Vector2 end;
    Color color;
    float duration = 0.f;
};

class FrameTracker
{
    
private:
    float mTotalFrameTime = 0.f;
    uint64_t mFrameCounter = 0;
    
    float lastFrameTime = 0.f;
    mutable std::multiset<float> mFrameTimes = {};
    
public:

    uint64_t getFrameCounter() const { return mFrameCounter; }
    
    void pushFrameTime(float frameTime);

    float getCurrentFPS() const;
    float getAverageFPS() const;

    /**
     * \param fraction expects value 0-1
     */
    float getLowestPercentageFPS(float fraction) const;
    
};

class Application
{
private:

    SDL_Window* mWindow = nullptr;
    SDL_Renderer* mRenderer = nullptr;
    std::shared_ptr<InputRouter> mInputRouter;
    
    bool bRunning = false;

    /* measured in ms */
    uint32_t mFrameTime = 16;
    Vector2 mWindowSize;

    bool bDrawFPS = true;

    static ApplicationParams sApplicationParams;

    FrameTracker mFrameTracker = {};
    std::vector<DebugLine> mDebugLines = {};

protected:
    
    Application() = default;
    Application(const ApplicationParams& params);
    ~Application();

public:

    uint64_t getFrameCount() const { return mFrameTracker.getFrameCounter(); }

    static Application& initApplication(const ApplicationParams& params);
    static Application& getApplication();

    void run();

    const Vector2& getWindowSize() const { return mWindowSize; }
    void addDebugLine(const DebugLine& debugLine);
    
protected:

    static void tickObjects(float deltaSeconds);
    void drawFrame(float deltaTime);
    void pollEvents();
    void handleEvent(const SDL_Event& event);
    
};
