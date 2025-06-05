#include "Application.h"

constexpr const char* WINDOW_TITLE = "OpenWindow";
constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

int main(int argc, char *argv[])
{
    Application application(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT);
    return 0;
}