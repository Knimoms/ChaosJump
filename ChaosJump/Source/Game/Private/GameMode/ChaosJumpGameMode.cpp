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

Vector2 ChaosJumpGameMode::sPlayerSpawnLocation = {.x = 0, .y = 200};

void ChaosJumpPlayerDeleter::operator()(ChaosJumpPlayer* player) const
{
    player->callOnDestroy();
    delete player;
}

void ChaosJumpGameMode::clearDroppedPlatforms(const float currentHeight)
{
    std::vector<int> droppedPlatformIndices = {};
    const size_t platformCount = mPlatforms.size(); 
    for (size_t i = platformCount - 1; i < std::numeric_limits<size_t>::max(); --i)
    {
        auto& platform = mPlatforms[i];
        if (platform->getLocation().y > currentHeight + mChunkHeight)
        {
            mPlatforms.erase(mPlatforms.begin() + i);
        }
    }
}

void ChaosJumpGameMode::clearObstaclesOutOfRange(const float currentHeight)
{
    std::vector<int> outOfRangeObstaclesIndices = {};
    const size_t obstacleCount = mObstacles.size(); 
    for (size_t i = obstacleCount - 1; i < std::numeric_limits<size_t>::max(); --i)
    {
        auto& obstacle = mObstacles[i];
        if (obstacle->getLocation().y > currentHeight + 3 * mChunkHeight)
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

void ChaosJumpGameMode::drawGameHUD(float deltaTime)
{
    Application& app = Application::getApplication();
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

    const auto [hostHeight, clientHeight] = [&]()
    {
        std::pair<float, float> heights;

        for (const ChaosJumpPlayer* player : mChaosJumpPlayers)
        {
            float* settingHeight;
            if (player->wasRemotelyCreated())
            {
                settingHeight = player->isLocallyOwned() ? &heights.second : &heights.first;
            }
            else
            {
                settingHeight = player->isLocallyOwned() ? &heights.first : &heights.second;
            }
            
            *settingHeight = player->getReachedHeight();
        }

        return heights;
    }();
    
    const DisplayText heightText
    {
        .screenPosition = {.x = 0, .y = -1},
        .text = std::format("{:.2f} : {:.2f}", hostHeight, clientHeight),
        .color = {.r = 1, .g = 1, .b = 1},
        .textScale = {.x = 2, .y = 2},
        .alignment = {.x = 0, .y = -1}
    };

    app.addDisplayText(heightText);

    if (mEndPhaseSeconds >= 0.f)
    {
        const uint64_t intEndSeconds = mEndPhaseSeconds;
        const DisplayText endPhaseSecondsText
        {
            .screenPosition = {.x = 0, .y = 1},
            .text = std::format("{:.2f}", mEndPhaseSeconds),
            .color = intEndSeconds % 2 ? Color{1, 1, 1} : Color{0, 1, 0},
            .textScale = {.x = 2, .y = 2},
            .alignment = {.x = 0, .y = 1}
        };

        app.addDisplayText(endPhaseSecondsText);
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

void ChaosJumpGameMode::addPlayer(Player* player)
{
    GameMode::addPlayer(player);
    mChaosJumpPlayers.push_back(static_cast<ChaosJumpPlayer*>(player));

}

void ChaosJumpGameMode::removePlayer(Player* player)
{
    GameMode::removePlayer(player);
    std::erase(mChaosJumpPlayers, static_cast<ChaosJumpPlayer*>(player));
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
            mPlayerMap[connection] = std::unique_ptr<ChaosJumpPlayer, ChaosJumpPlayerDeleter>(new ChaosJumpPlayer(size, sPlayerSpawnLocation));
            mPlayerMap[connection]->registerObject();
            mPlayerMap[connection]->transferOwnershipToConnection(connection);
        }
    }

    std::unique_ptr<Platform> platform = std::make_unique<Platform>();
    platform->setLocation(sPlayerSpawnLocation + Vector2{.x = 0, .y = 300});
    mPlatforms.push_back(std::move(platform));

    Application& app = Application::getApplication();
    const Vector2& windowSize = app.getWindowSize();
    mChunkGenerator = std::unique_ptr<ChunkGenerator, ChunkGeneratorDeleter>(new ChunkGenerator(windowSize, 8, mSeed));

    mChunkGenerator->generateChunk(0, mPlatforms, mObstacles);
    
    mChunkHeight = windowSize.y;
}

void ChaosJumpGameMode::restart()
{
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

    const uint8_t deadPlayerCount = [&]()
    {
        uint8_t count = 0;
        for (const ChaosJumpPlayer* player : mChaosJumpPlayers)
        {
            count += player->isDead();
        }

        return count;
    }();

    switch (deadPlayerCount)
    {
    case 0:
        break;
    case 1:
        if (mEndPhaseSeconds < 0.f)
        {
            mEndPhaseSeconds = 10.f;
        }
        else
        {
            mEndPhaseSeconds -= deltaTime;
        }

        if (mEndPhaseSeconds < 0.f)
        {
            gameOver();
        }
        
        break;
    default:
        mEndPhaseSeconds = -1.f;
    }

    drawGameHUD(deltaTime);
    
    const float viewHeight = app.getCurrentViewLocation().y;
    clearDroppedPlatforms(viewHeight);
    clearObstaclesOutOfRange(viewHeight);

    const int currentChunkHeightCoord = std::abs(static_cast<int>(viewHeight) / mChunkHeight);

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
