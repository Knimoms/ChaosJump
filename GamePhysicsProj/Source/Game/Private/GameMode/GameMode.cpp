#include "GameMode/GameMode.h"

#include <format>

#include "Application.h"
#include "GameMode/ChunkGenerator.h"
#include "Input/InputRouter.h"
#include "Objects/Platform.h"
#include "Player/Player.h"
#include "SDL3/SDL_render.h"

void GameMode::clearDroppedPlatforms()
{
    std::vector<int> droppedPlatformIndices = {};
    const size_t platformCount = mPlatforms.size(); 
    for (size_t i = platformCount - 1; i < std::numeric_limits<size_t>::max(); --i)
    {
        auto& platform = mPlatforms[i];
        if (platform->getLocation().y > mPlayer->getLocation().y + mChunkHeight)
        {
            mPlatforms.erase(mPlatforms.begin() + i);
        }
    }
}

void GameMode::clearObstaclesOutOfRange()
{
    std::vector<int> outOfRangeObstaclesIndices = {};
    const size_t obstacleCount = mObstacles.size(); 
    for (size_t i = obstacleCount - 1; i < std::numeric_limits<size_t>::max(); --i)
    {
        auto& obstacle = mObstacles[i];
        if (obstacle->getLocation().y > mPlayer->getLocation().y + 3 * mChunkHeight)
        {
            mObstacles.erase(mObstacles.begin() + i);
        }
    }
}

GameMode::GameMode() = default;

GameMode::~GameMode() = default;

void GameMode::startGame()
{
    mPlatforms.clear();
    mObstacles.clear();
    
    bStarted = true;
    bGameOver = false;
    mReachedHeight = 0.f;
    
    constexpr Vector2 size {.x = 1, .y = 1};
    
    mPlayer = std::make_unique<Player>(size, mPlayerSpawnLocation);

    std::unique_ptr<Platform> platform = std::make_unique<Platform>();
    platform->setLocation(mPlayerSpawnLocation + Vector2{.x = 0, .y = 300});
    mPlatforms.push_back(std::move(platform));

    Application& app = Application::getApplication();
    const Vector2& windowSize = app.getWindowSize();
    mChunkGenerator = std::make_unique<ChunkGenerator>(windowSize, 8);

    mChunkGenerator->generateChunk(0, mPlatforms, mObstacles);
    
    mChunkHeight = windowSize.y;
    app.getInputRouter()->addInputReceiver(getPlayer());
}

void GameMode::gameOver()
{
    bGameOver = true;
    bStarted = false;

    Application& app = Application::getApplication();
    app.getInputRouter()->removeInputReceiver(getPlayer());    
}

void GameMode::tick(const float deltaTime)
{
    mGameTime += deltaTime;

    Application& app = Application::getApplication();

    if (!bStarted)
    {
        const uint64_t gameSeconds = mGameTime;

        const DisplayText titleDisplayText
        {
            .screenPosition = {.x = 0, .y = -0.25},
            .text = bGameOver ? "Game Over": "Chaos Jump",
            .color = {.r = 1, .g = 0, .b = 0},
            .textScale = {.x = 4, .y = 4}
        };

        const DisplayText infoDisplayText
        {
            .screenPosition = {.x = 0, .y = 0.1},
            .text = "Press any button to start the game.",
            .color = gameSeconds % 2 ? Color{1, 1, 1} : Color{0, 1, 0},
            .textScale = {.x = 1.5, .y = 1.5}
        };

        app.addDisplayText(titleDisplayText);
        app.addDisplayText(infoDisplayText);

        
        return;
    }

    const DisplayText obstacleAliveCount
    {
        .screenPosition = {.x = 0, .y = -0.95},
        .text = std::format("Obstacles alive: {}", mObstacles.size()),
        .color = {.r = 1, .g = 1, .b = 1},
        .textScale = {.x = 1.5, .y = 1.5},
        .alignment = {.x = 0, .y = -1}
    };

    app.addDisplayText(obstacleAliveCount);
    
    if (!mPlayer) return;

    if (mPlayer->isDead())
    {
        gameOver();
        return;
    }

    clearDroppedPlatforms();
    clearObstaclesOutOfRange();

    const float currentPlayerHeight = -(mPlayer->getLocation().y - mPlayerSpawnLocation.y);
    mReachedHeight = std::max(mReachedHeight, currentPlayerHeight/100);

    const DisplayText heightText
    {
        .screenPosition = {.x = 0, .y = -1},
        .text = std::format("{:.2f}", mReachedHeight),
        .color = {.r = 1, .g = 1, .b = 1},
        .textScale = {.x = 2, .y = 2},
        .alignment = {.x = 0, .y = -1}
    };

    app.addDisplayText(heightText);
    
    const int currentChunkHeightCoord = std::abs(static_cast<int>(currentPlayerHeight / mChunkHeight));

    for (int i = mChunkGenerator->getChunkGenerationHeight() + 1; currentChunkHeightCoord + 3 > i; ++i)
    {
        mChunkGenerator->generateChunk(i, mPlatforms, mObstacles);
    }
}

void GameMode::handleKeyPressed(const SDL_Scancode scancode)
{
    if (!bStarted)
    {
        startGame();
    }
}