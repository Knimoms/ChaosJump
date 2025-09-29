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
uint32_t ChaosJumpGameMode::sPlatformsPerChunk = 8;

void ChaosJumpPlayerDeleter::operator()(ChaosJumpPlayer* player) const
{
    player->callOnDestroy();
    delete player;
}

void ChaosJumpGameMode::setSeed(const uint32_t inSeed)
{
    mSeed = inSeed;

    mPlatforms.clear();
    mObstacles.clear();

    std::unique_ptr<Platform> platform = std::make_unique<Platform>();
    platform->setLocation(sPlayerSpawnLocation + Vector2{.x = 0, .y = 300});
    mPlatforms.push_back(std::move(platform));

    const Vector2& windowSize = Application::getApplication().getWindowSize();
    mChunkGenerator = std::unique_ptr<ChunkGenerator, ChunkGeneratorDeleter>(new ChunkGenerator(windowSize, sPlatformsPerChunk, mSeed));

    mChunkGenerator->generateChunk(0, mPlatforms, mObstacles);
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
            .text = "Chaos Jump",
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
    }
}

void ChaosJumpGameMode::handleNetworkError()
{
    GameMode::handleNetworkError();

    endGame();
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

void ChaosJumpGameMode::evaluateScoringPlayer()
{
    if (!ensure(isLocallyOwned())) return;
    
    ChaosJumpPlayer* highestPlayer = nullptr;
    float highestReachedHeight = 0.f;
    for (ChaosJumpPlayer* player : mChaosJumpPlayers)
    {
        const float currentReachedHeight = player->getReachedHeight();
        
        if (!highestPlayer || currentReachedHeight > highestReachedHeight)
        {
            highestPlayer = player;
            highestReachedHeight = player->getReachedHeight();
        }
    }

    if (highestReachedHeight > 0.f)
    {
        highestPlayer->isLocallyOwned() ? ++mHostScore : ++mClientScore;
    }

    bWantsToReset = true;
    const NetPacket replicatePacket(this);

    for (HSteamNetConnection connection : getJoinedConnections())
    {
        if (connection)
        {
            constexpr bool bReliable = true;
            NetHandler::sendPacketToConnection(replicatePacket, connection, bReliable);
        }
    }

    bWantsToReset = false;
    
    reset();
}

void ChaosJumpGameMode::startGame()
{
    bGameInProgress = true;

    if (isLocallyOwned())
    {
        for (HSteamNetConnection connection : getJoinedConnections())
        {
            constexpr Vector2 size {.x = 1, .y = 1};
            mPlayerMap[connection] = std::unique_ptr<ChaosJumpPlayer, ChaosJumpPlayerDeleter>(new ChaosJumpPlayer(size, sPlayerSpawnLocation));
            mPlayerMap[connection]->registerObject();
            mPlayerMap[connection]->transferOwnershipToConnection(connection);
        }
    }

    Application& app = Application::getApplication();
    const Vector2& windowSize = app.getWindowSize();
    
    mChunkHeight = windowSize.y;

    reset();
}

void ChaosJumpGameMode::reset()
{
    mEndPhaseSeconds = -1.f;
    
    if (isLocallyOwned())
    {
        setSeed(std::random_device()());
    }

    for (ChaosJumpPlayer* player : mChaosJumpPlayers)
    {
        player->reset();
    }
}

void ChaosJumpGameMode::endGame()
{
    Application& app = Application::getApplication();
    NetHandler* netHandler = app.getNetHandler();

    if (netHandler->isHosting())
    {
        netHandler->closeSession();
    }
    else if (netHandler->isConnectedAsClient())
    {
        netHandler->closeServerConnection();
    }

    std::unique_ptr gameMode = std::make_unique<ChaosJumpGameMode>();
    app.getInputRouter()->addInputReceiver(gameMode.get());
    app.getInputRouter()->removeInputReceiver(this);
    gameMode->registerObject();

    for (Player* player : getPlayers())
    {
        app.getInputRouter()->removeInputReceiver(player);
    }

    app.setGameMode(std::move(gameMode));
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

        if (mEndPhaseSeconds > 0.f) break;
        
    default:
        if (isLocallyOwned())
        {
            evaluateScoringPlayer();
        }

        return;
    }

    drawGameHUD(deltaTime);
    
    const float viewHeight = Application::getApplication().getCurrentViewLocation().y;
    clearDroppedPlatforms(viewHeight);
    clearObstaclesOutOfRange(viewHeight);

    const int currentChunkHeightCoord = std::abs(static_cast<int>(viewHeight) / mChunkHeight);

    if (!mChunkGenerator) return;
    
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
        case SDL_SCANCODE_L:
            bWantsToStartGame = true;
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

    if (scancode == SDL_SCANCODE_ESCAPE)
    {
        endGame();
    }
}

constexpr int expectedPacketSize = sizeof(bool) * 2 + 3 * sizeof(uint32_t);
std::string ChaosJumpGameMode::serialize() const
{
    std::string serialized;
    serialized.resize(expectedPacketSize);

    uint8_t* destinationAddress = reinterpret_cast<uint8_t*>(serialized.data());

    memcpy(destinationAddress, &bWantsToStartGame, sizeof(bWantsToStartGame));

    destinationAddress = destinationAddress + sizeof(bWantsToStartGame);
    memcpy(destinationAddress, &bWantsToReset, sizeof(bWantsToReset));

    destinationAddress = destinationAddress + sizeof(bWantsToReset);
    memcpy(destinationAddress, &mSeed, sizeof(mSeed));
    
    destinationAddress = destinationAddress + sizeof(mSeed);
    memcpy(destinationAddress, &mHostScore, sizeof(mHostScore));

    destinationAddress = destinationAddress + sizeof(mHostScore);
    memcpy(destinationAddress, &mClientScore, sizeof(mClientScore));

    return serialized;
}

void ChaosJumpGameMode::deserialize(std::string serialized)
{
    if (!ensure(serialized.size() >= expectedPacketSize)) return;

    const uint8_t* sourceAddress = reinterpret_cast<uint8_t*>(serialized.data());
    memcpy(&bWantsToStartGame, sourceAddress, sizeof(bool));
    sourceAddress += sizeof(bool);
    memcpy(&bWantsToReset, sourceAddress, sizeof(bool));
    sourceAddress += sizeof(bool);

    if (bWantsToReset)
    {
        reset();
        bWantsToReset = false;
    }

    uint32_t newSeed;
    memcpy(&newSeed, sourceAddress, sizeof(uint32_t));

    if (newSeed != mSeed)
    {
        setSeed(newSeed);
    }

    sourceAddress += sizeof(uint32_t);
    memcpy(&mHostScore, sourceAddress, sizeof(uint32_t));

    sourceAddress += sizeof(uint32_t);
    memcpy(&mClientScore, sourceAddress, sizeof(uint32_t));
}
