#pragma once
#include <memory>
#include <set>
#include <vector>

#include "Input/InputRouter.h"
#include "Math/Vector2.h"
#include "Render/Camera.h"
#include "Render/DrawableInterface.h"
#include "SDL3/SDL_stdinc.h"

class NetHandler;
class GameMode;
class CollisionShapeInterface;
class SDL_Window;
class SDL_Renderer;
class InputRouter;
union SDL_Event;

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

struct DebugLine
{
    Vector2 start;
    Vector2 end;
    Color color;
    float duration = 0.f;
};

struct DisplayText
{
    /*TopLeft window border = {-1, -1}, bottom right = {1, 1}*/
    Vector2 screenPosition = {};
    std::string text;
    Color color = {1, 1, 1};
    float duration = 0.f;
    Vector2 textScale = {1, 1}; 
    
    /* TopLeft Text border = {-1, -1}, bottom right = {1, 1}
     * This is the position in the DisplayText that aligns with the screenPosition.
     */
    Vector2 alignment = {};
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

struct WindowDeleter
{
    void operator()(SDL_Window* rawWindow) const;
};


struct RendererDeleter
{
    void operator()(SDL_Renderer* rawRenderer) const;
};

class Application
{
private:

    std::unique_ptr<SDL_Window, WindowDeleter> mWindow = nullptr;
    std::unique_ptr<SDL_Renderer, RendererDeleter> mRenderer = nullptr;

    std::unique_ptr<InputRouter> mInputRouter = nullptr;
    std::unique_ptr<NetHandler> mNetHandler = nullptr;
    std::weak_ptr<Camera> mRenderCamera;
    
    bool bRunning = false;

    /* measured in ms */
    uint32_t mFrameTime = 16;
    Vector2 mWindowSize;

    bool bDrawFPS = true;

    static ApplicationParams sApplicationParams;

    FrameTracker mFrameTracker = {};
    std::vector<DebugLine> mDebugLines = {};
    std::vector<DisplayText> mDisplayTexts = {};

    bool bPaused = false;

protected:
    
    Application() = default;

    Application(const ApplicationParams& params);
    ~Application();

public:

    InputRouter* getInputRouter() const { return mInputRouter.get(); }

    uint64_t getFrameCount() const { return mFrameTracker.getFrameCounter(); }

    static Application& initApplication(const ApplicationParams& params);
    
    static Application& getApplication();

    void run();

    const Vector2& getWindowSize() const { return mWindowSize; }
    void addDebugLine(const DebugLine& debugLine);
    void addDisplayText(const DisplayText& displayText);
    Vector2 getCurrentViewLocation() const;
    void setRenderCamera(std::weak_ptr<Camera> inCamera);
    
    void tickObjects(float deltaSeconds) const;
    
protected:

    void drawFrame(float deltaTime);
    void pollEvents();
    void handleEvent(const SDL_Event& event);
    
};
