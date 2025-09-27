#include "GameMode/ChaosJumpGameMode.h"

#include <format>

#include "Application.h"
#include "Base/HelperDefinitions.h"
#include "Debugging/DebugDefinitions.h"
#include "GameMode/ChunkGenerator.h"
#include "Networking/NetHandler.h"
#include "Objects/Platform.h"
#include "Player/ChaosJumpPlayer.h"
#include "SDL3/SDL_render.h"

DEFINE_DEFAULT_DELETER(ChunkGenerator)

void ChaosJumpPlayerDeleter::operator()(ChaosJumpPlayer* player) const
{
    player->callOnDestroy();
    delete player;
}

void ChaosJumpGameMode::clearDroppedPlatforms()
{
    std::vector<int> droppedPlatformIndices = {};
    const size_t platformCount = mPlatforms.size(); 
    for (size_t i = platformCount - 1; i < std::numeric_limits<size_t>::max(); --i)
    {
        auto& platform = mPlatforms[i];
        if (platform->getLocation().y > mLocalChaosJumpPlayer->getLocation().y + mChunkHeight)
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
        if (obstacle->getLocation().y > mLocalChaosJumpPlayer->getLocation().y + 3 * mChunkHeight)
        {
            mObstacles.erase(mObstacles.begin() + i);
        }
    }
}

void ChaosJumpGameMode::drawMenuDisplayText() const
{
    Application& app = Application::getApplication();

    const uint64_t gameSeconds = mGameTime;

    if (app.getNetHandler()->isHosting())
    {
        const DisplayText infoDisplayText
        {
            .screenPosition = {.x = 0, .y = -0.25},
            .text = "Waiting for another player to join.",
            .color = {.r = 1, .g = 0, .b = 0},
            .textScale = {.x = 4, .y = 4}
        };

        const DisplayText actionDisplayText
        {
            .screenPosition = {.x = 0, .y = 0.1},
            .text = "Press I to open Invite Dialogue, press C to close Session.",
            .color = {.r = 1, .g = 0, .b = 0},
            .textScale = {.x = 3, .y = 3}
        };

        app.addDisplayText(infoDisplayText);
        app.addDisplayText(actionDisplayText);
    }
    else
    {
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
            .text = "Press H to host Game, press F to open friends list.",
            .color = gameSeconds % 2 ? Color{1, 1, 1} : Color{0, 1, 0},
            .textScale = {.x = 1.5, .y = 1.5}
        };
        
        app.addDisplayText(titleDisplayText);
        app.addDisplayText(infoDisplayText);
    }
}

ChaosJumpGameMode::ChaosJumpGameMode() = default;
ChaosJumpGameMode::~ChaosJumpGameMode() = default;

std::string ChaosJumpGameMode::handleJoiningConnection(const HSteamNetConnection connection)
{
    if (getJoinedConnections().size() >= 2)
    {
        return "Session full";
    }
    
    return GameMode::handleJoiningConnection(connection);
}

void ChaosJumpGameMode::handleConnectionJoined(const HSteamNetConnection connection)
{
    GameMode::handleConnectionJoined(connection);

    if (getJoinedConnections().size() >= 2)
    {
        bWantsToStartGame = true;
        mSeed = std::random_device()();
    }
}

void ChaosJumpGameMode::setLocalPlayer(Player* inLocalPlayer)
{
    GameMode::setLocalPlayer(inLocalPlayer);

    mLocalChaosJumpPlayer = dynamic_cast<ChaosJumpPlayer*>(inLocalPlayer);
}

void ChaosJumpGameMode::startGame()
{
    mPlatforms.clear();
    mObstacles.clear();
    
    bGameInProgress = true;
    bGameOver = false;
    mReachedHeight = 0.f;
    
    constexpr Vector2 size {.x = 1, .y = 1};

    if (isLocallyOwned())
    {
        for (HSteamNetConnection connection : getJoinedConnections())
        {
            mPlayers[connection] = std::unique_ptr<ChaosJumpPlayer, ChaosJumpPlayerDeleter>(new ChaosJumpPlayer(size, mPlayerSpawnLocation));
            mPlayers[connection]->registerObject();
            mPlayers[connection]->transferOwnershipToConnection(connection);
        }
    }

    std::unique_ptr<Platform> platform = std::make_unique<Platform>();
    platform->setLocation(mPlayerSpawnLocation + Vector2{.x = 0, .y = 300});
    mPlatforms.push_back(std::move(platform));

    Application& app = Application::getApplication();
    const Vector2& windowSize = app.getWindowSize();
    mChunkGenerator = std::unique_ptr<ChunkGenerator, ChunkGeneratorDeleter>(new ChunkGenerator(windowSize, 8, mSeed));

    mChunkGenerator->generateChunk(0, mPlatforms, mObstacles);
    
    mChunkHeight = windowSize.y;
}

void ChaosJumpGameMode::gameOver()
{
    bGameOver = true;
    bGameInProgress = false;
}

void ChaosJumpGameMode::hostSession()
{
    Application::getApplication().getNetHandler()->hostSession();
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
        drawMenuDisplayText();
        return;
    }

    const DisplayText obstacleAliveCount
    {
        .screenPosition = {.x = -0.95f, .y = -0.95f},
        .text = std::format("Obstacles alive: {}", mObstacles.size()),
        .color = {.r = 1, .g = 1, .b = 1},
        .textScale = {.x = 1.25, .y = 1.25},
        .alignment = {.x = -1, .y = -1}
    };

    const DisplayText scoreText
    {
        .screenPosition = {.x = 0, .y = -0.95f},
        .text = std::format("{} : {}", mHostScore, mClientScore),
        .color = {.r = 1, .g = 1, .b = 1},
        .textScale = {.x = 1.5, .y = 1.5},
        .alignment = {.x = 0, .y = -1}
    };

    app.addDisplayText(obstacleAliveCount);
    app.addDisplayText(scoreText);
    
    if (!mLocalChaosJumpPlayer) return;

    //if (mLocalChaosJumpPlayer->isDead())
    //{
    //    gameOver();
    //    return;
    //}

    clearDroppedPlatforms();
    clearObstaclesOutOfRange();

    const float currentPlayerHeight = -(mLocalChaosJumpPlayer->getLocation().y - mPlayerSpawnLocation.y);
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
    NetHandler* netHandler = Application::getApplication().getNetHandler();

    if (netHandler->isHosting())
    {
        switch (scancode)
        {
        case SDL_SCANCODE_I:
            netHandler->openInviteDialogue();
            break;
        case SDL_SCANCODE_F:
            NetHandler::openFriendslist();
            break;
        case SDL_SCANCODE_C:
            netHandler->closeSession();
            break;
        default:
            ;
        }
    }
    else
    {
        switch (scancode)                    
        {                                    
        case SDL_SCANCODE_H:                 
            netHandler->hostSession();       
            break;                           
        case SDL_SCANCODE_F:                 
            NetHandler::openFriendslist();   
            break;                           
        default:                             
            ;                                
        }                                    
    }
}

std::string ChaosJumpGameMode::serialize() const
{
    std::string serialized;
    serialized.resize(sizeof(bWantsToStartGame) + sizeof(bGameInProgress) + sizeof(mSeed));

    uint8_t* destinationAddress = reinterpret_cast<uint8_t*>(serialized.data());

    memcpy(destinationAddress, &bWantsToStartGame, sizeof(bWantsToStartGame));

    destinationAddress = destinationAddress + sizeof(bWantsToStartGame);
    memcpy(destinationAddress, &bGameInProgress, sizeof(bGameInProgress));

    destinationAddress = destinationAddress + sizeof(bQueueGameOver);
    memcpy(destinationAddress, &mSeed, sizeof(mSeed));

    destinationAddress = destinationAddress + sizeof(mSeed);
    memcpy(destinationAddress, &mHostScore, sizeof(mHostScore));

    destinationAddress = destinationAddress + sizeof(mHostScore);
    memcpy(destinationAddress, &mClientScore, sizeof(mClientScore));

    return serialized;
}

void ChaosJumpGameMode::deserialize(std::string serialized)
{
    if (!ensure(serialized.size() >= sizeof(bool) * 2 + sizeof(uint32_t))) return;

    uint8_t* sourceAddress = reinterpret_cast<uint8_t*>(serialized.data());
    memcpy(&bWantsToStartGame, sourceAddress, sizeof(bool));
    sourceAddress += sizeof(bool);
    memcpy(&bQueueGameOver, sourceAddress, sizeof(bool));
    sourceAddress += sizeof(bool);
    memcpy(&mSeed, sourceAddress, sizeof(uint32_t));
    sourceAddress += sizeof(uint32_t);
    memcpy(&mHostScore, sourceAddress, sizeof(uint32_t));
    sourceAddress += sizeof(uint32_t);
    memcpy(&mClientScore, sourceAddress, sizeof(uint32_t));
}
