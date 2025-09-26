#include "GameMode/ChaosJumpGameMode.h"

#include <format>

#include "Application.h"
#include "Base/HelperDefinitions.h"
#include "Debugging/DebugDefinitions.h"
#include "GameMode/ChunkGenerator.h"
#include "Input/InputRouter.h"
#include "Networking/NetHandler.h"
#include "Objects/Platform.h"
#include "Player/Player.h"
#include "SDL3/SDL_render.h"

DEFINE_DEFAULT_DELETER(ChunkGenerator)
DEFINE_DEFAULT_DELETER(Player)

void ChaosJumpGameMode::clearDroppedPlatforms()
{
    std::vector<int> droppedPlatformIndices = {};
    const size_t platformCount = mPlatforms.size(); 
    for (size_t i = platformCount - 1; i < std::numeric_limits<size_t>::max(); --i)
    {
        auto& platform = mPlatforms[i];
        if (platform->getLocation().y > mLocalPlayer->getLocation().y + mChunkHeight)
        {
            mPlatforms.erase(mPlatforms.begin() + i);
        }
    }
}

void ChaosJumpGameMode::clearObstaclesOutOfRange()
{
    std::vector<int> outOfRangeObstaclesIndices = {};
    const size_t obstacleCount = mObstacles.size(); 
    for (size_t i = obstacleCount - 1; i < std::numeric_limits<size_t>::max(); --i)
    {
        auto& obstacle = mObstacles[i];
        if (obstacle->getLocation().y > mLocalPlayer->getLocation().y + 3 * mChunkHeight)
        {
            mObstacles.erase(mObstacles.begin() + i);
        }
    }
}

ChaosJumpGameMode::ChaosJumpGameMode() = default;
ChaosJumpGameMode::~ChaosJumpGameMode() = default;

void ChaosJumpGameMode::handleConnectionJoined(HSteamNetConnection connection)
{
    GameMode::handleConnectionJoined(connection);

    if (!getJoinedConnections().empty())
    {
        bWantsToStartGame = true;
        mSeed = std::random_device()();
    }
}

void ChaosJumpGameMode::startGame()
{
    mPlatforms.clear();
    mObstacles.clear();
    
    bGameInProgress = true;
    bGameOver = false;
    mReachedHeight = 0.f;
    
    constexpr Vector2 size {.x = 1, .y = 1};

    mLocalPlayer = std::unique_ptr<Player, PlayerDeleter>(new Player(size, mPlayerSpawnLocation));
    mLocalPlayer->registerObject();

    std::unique_ptr<Platform> platform = std::make_unique<Platform>();
    platform->setLocation(mPlayerSpawnLocation + Vector2{.x = 0, .y = 300});
    mPlatforms.push_back(std::move(platform));

    Application& app = Application::getApplication();
    const Vector2& windowSize = app.getWindowSize();
    mChunkGenerator = std::unique_ptr<ChunkGenerator, ChunkGeneratorDeleter>(new ChunkGenerator(windowSize, 8, mSeed));

    mChunkGenerator->generateChunk(0, mPlatforms, mObstacles);
    
    mChunkHeight = windowSize.y;
    app.getInputRouter()->addInputReceiver(getPlayer());
}

void ChaosJumpGameMode::gameOver()
{
    bGameOver = true;
    bGameInProgress = false;

    Application& app = Application::getApplication();
    app.getInputRouter()->removeInputReceiver(getPlayer());    
}

void ChaosJumpGameMode::hostSession()
{
    Application::getApplication().getNetHandler()->host();
}

void ChaosJumpGameMode::tick(const float deltaTime)
{
    if (bWantsToStartGame && !bGameInProgress)
    {
        startGame();
    }
    
    mGameTime += deltaTime;

    Application& app = Application::getApplication();

    if (!bGameInProgress)
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
            .text = "Press H to host Game or join a friend via the Steam Overlay.",
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
    
    if (!mLocalPlayer) return;

    if (mLocalPlayer->isDead())
    {
        gameOver();
        return;
    }

    clearDroppedPlatforms();
    clearObstaclesOutOfRange();

    const float currentPlayerHeight = -(mLocalPlayer->getLocation().y - mPlayerSpawnLocation.y);
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

void ChaosJumpGameMode::handleKeyPressed(const SDL_Scancode scancode)
{
    switch (scancode)
    {
    case SDL_SCANCODE_H:
        hostSession();
    default:
        ;
    }
}

std::string ChaosJumpGameMode::serialize() const
{
    std::string serialized;
    serialized.resize(sizeof(bWantsToStartGame) + sizeof(bGameInProgress) + sizeof(mSeed));

    void* destinationAddress = serialized.data();
    memcpy(destinationAddress, &bWantsToStartGame, sizeof(bWantsToStartGame));

    destinationAddress += sizeof(bWantsToStartGame);
    memcpy(destinationAddress, &bGameInProgress, sizeof(bGameInProgress));

    destinationAddress += sizeof(bGameInProgress);
    memcpy(destinationAddress, &mSeed, sizeof(mSeed));

    return serialized;
}

void ChaosJumpGameMode::deserialize(std::string serialized)
{
    if (!ensure(serialized.size() >= sizeof(bool) * 2 + sizeof(uint32_t))) return;

    void* sourceAddress = serialized.data();
    memcpy(&bWantsToStartGame, sourceAddress, sizeof(bool));
    sourceAddress += sizeof(bool);
    memcpy(&bQueueGameOver, sourceAddress, sizeof(bool));
    sourceAddress += sizeof(bool);
    memcpy(&mSeed, sourceAddress, sizeof(uint32_t));
}
