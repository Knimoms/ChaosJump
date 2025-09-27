#include "Application.h"
#include "GameMode/ChaosJumpGameMode.h"

constexpr const char* WINDOW_TITLE = "OpenWindow";
constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 720;

int main(int argc, char *argv[])
{
    ApplicationParams applicationParams;
    applicationParams.title = WINDOW_TITLE;
    applicationParams.width = WINDOW_WIDTH;
    applicationParams.height = WINDOW_HEIGHT;
    applicationParams.framesPerSecond = 0;
    
    Application& application = Application::initApplication(applicationParams);
    std::unique_ptr gameMode = std::make_unique<ChaosJumpGameMode>();
    application.getInputRouter()->addInputReceiver(gameMode.get());
    gameMode->registerObject();
    
    application.setGameMode(std::move(gameMode));
    application.run();
    
    return 0;
}