#include "Game/GameMode.h"

#include <format>

#include "Application.h"
#include "Game/ChunkGenerator.h"
#include "Input/InputRouter.h"
#include "Objects/Platform.h"
#include "Player/Player.h"
#include "SDL3/SDL_render.h"

GameMode::GameMode() = default;

GameMode::~GameMode() = default;

void GameMode::startGame()
{
    bStarted = true;
    constexpr Vector2 size {.x = 100, .y = 100};
    constexpr Vector2 startPosition {.x = 0, .y = 500};
    
    mPlayer = std::make_unique<Player>(size, startPosition);

    std::unique_ptr<Platform> platform = std::make_unique<Platform>();
    platform->setLocation(startPosition + Vector2{.x = 0, .y = 300});
    mPlatforms.push_back(std::move(platform));

    Application& app = Application::getApplication();
    const Vector2& windowSize = app.getWindowSize();
    mChunkGenerator = std::make_unique<ChunkGenerator>(windowSize, 8);

    mChunkGenerator->generateChunk(0, mPlatforms);
    mChunkGenerator->generateChunk(1, mPlatforms);
    mChunkGenerator->generateChunk(2, mPlatforms);
    mChunkGenerator->generateChunk(3, mPlatforms);

    mChunkHeight = windowSize.y;
    app.getInputRouter()->addInputReceiver(getPlayer());
}

void GameMode::tick(const float deltaTime)
{
    if (!bStarted)
    {
        Application& app = Application::getApplication();

        DisplayText displayText;
        displayText.text = "Press any button to start game.";
        app.addDisplayText(displayText);
        return;
    }
    
    if (!mPlayer) return;

    mCurrentChunkHeightCoord = std::abs(static_cast<int>(mPlayer->getLocation().y / mChunkHeight)) + 3;

    for (int i = mChunkGenerator->getChunkGenerationHeight() + 1; mCurrentChunkHeightCoord > i; ++i)
    {
        mChunkGenerator->generateChunk(i, mPlatforms);
    }
}

void GameMode::handleKeyPressed(const SDL_Scancode scancode)
{
    if (!bStarted)
    {
        startGame();
    }
}