#pragma once
#include <memory>

#include "SDL3/SDL_stdinc.h"

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

public:

    Application(const ApplicationParams& params);
    ~Application();

    void run();

protected:
    static void tickObjects(float deltaSeconds);
    void drawFrame();
    void pollEvents();
    void handleEvent(const SDL_Event& event);
    
};
