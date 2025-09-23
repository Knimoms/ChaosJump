#include "Application.h"
#include "GameMode/GameMode.h"

constexpr const char* WINDOW_TITLE = "OpenWindow";
constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;

int main(int argc, char *argv[])
{
    ApplicationParams applicationParams;
    applicationParams.title = WINDOW_TITLE;
    applicationParams.width = WINDOW_WIDTH;
    applicationParams.height = WINDOW_HEIGHT;
    applicationParams.framesPerSecond = 0;
    
    Application& application = Application::initApplication(applicationParams);
    GameMode GameMode;
    application.getInputRouter()->addInputReceiver(&GameMode);
    application.run();
    
    return 0;
}