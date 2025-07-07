#include "Application.h"

constexpr const char* WINDOW_TITLE = "OpenWindow";
constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

int main(int argc, char *argv[])
{
    ApplicationParams applicationParams;
    applicationParams.title = WINDOW_TITLE;
    applicationParams.width = WINDOW_WIDTH;
    applicationParams.height = WINDOW_HEIGHT;
    
    Application& application = Application::initApplication(applicationParams);
    application.run();
    
    return 0;
}