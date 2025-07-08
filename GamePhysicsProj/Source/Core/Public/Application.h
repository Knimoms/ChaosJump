#pragma once
#include <memory>

#include "Math/Vector2.h"
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
    
};

class SDL_Window;
class SDL_Renderer;
class InputRouter;
union SDL_Event;

class Application
{
    
private:

    SDL_Window* mWindow = nullptr;
    SDL_Renderer* mRenderer = nullptr;
    std::shared_ptr<InputRouter> mInputRouter;
    
    bool bRunning = false;

    /* measured in ms */
    uint32_t mFrameTime = 16;

    uint64_t frameCount = 0;

    Vector2 mWindowSize;

    static ApplicationParams sApplicationParams;

protected:
    
    Application() = default;
    Application(const ApplicationParams& params);
    ~Application();

public:

    static Application& initApplication(const ApplicationParams& params);
    static Application& getApplication();

    void run();

    const Vector2& getWindowSize() const { return mWindowSize; }

protected:

    static void tickObjects(float deltaSeconds);
    void drawFrame() const;
    void pollEvents();
    void handleEvent(const SDL_Event& event);
    
};
