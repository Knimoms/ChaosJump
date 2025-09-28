#pragma once
#include <memory>
#include <vector>

#include "Base/HelperDefinitions.h"
#include "Game/GameMode.h"
#include "Input/InputReceiverInterface.h"
#include "Math/Vector2.h"
#include "Objects/Platform.h"
#include "SDL3/SDL_scancode.h"

class ChunkGenerator;
class ChaosJumpPlayer;

DECLARE_DEFAULT_DELETER(ChunkGenerator)
DECLARE_DEFAULT_DELETER(ChaosJumpPlayer)

class ChaosJumpGameMode : public GameMode, public TickableInterface, public InputReceiverInterface
{
private:

    static Vector2 sPlayerSpawnLocation;

    std::vector<ChaosJumpPlayer*> mChaosJumpPlayers;
    std::map<HSteamNetConnection, std::unique_ptr<ChaosJumpPlayer, ChaosJumpPlayerDeleter>> mPlayerMap;
    std::unique_ptr<ChunkGenerator, ChunkGeneratorDeleter> mChunkGenerator = nullptr;

    float mChunkHeight = 0.f;

    std::vector<std::unique_ptr<Platform>> mPlatforms = {};
    std::vector<std::unique_ptr<CollisionObject>> mObstacles = {};

    bool bGameOver = false;

    float mGameTime = 0.f;

    float mReachedHeight = 0.f;
    float mEndPhaseSeconds = -1.f;

    struct //Replicated Properties
    {
        bool bWantsToStartGame = false;
        bool bQueueGameOver = false;
    
        uint32_t mSeed = 0;

        uint32_t mHostScore = 0;
        uint32_t mClientScore = 0;
    };

protected:

    void clearDroppedPlatforms(float currentHeight);
    void clearObstaclesOutOfRange(float currentHeight);

    void drawMenuDisplayText() const;
    void drawGameHUD(float deltaTime);

public:

    ChaosJumpGameMode();
    ~ChaosJumpGameMode() override;

    //~ Begin GameMode Interface
    std::string handleJoiningConnection(HSteamNetConnection connection) override;
    void handleConnectionJoined(HSteamNetConnection connection) override;
    void addPlayer(Player* player) override;
    void removePlayer(Player* player) override;
    //~ End GameMode Interface
    
    virtual void startGame();
    virtual void restart();
    virtual void gameOver();

    static void hostSession();

    //~ Begin TickableInterface
    void tick(float deltaTime) override;
    //~ End TickableInterface

    //~ Begin InputReceiverInterface
    void handleKeyPressed(SDL_Scancode scancode) override;
    //~ End InputReceiverInterface

    //~ Begin SerializableInterface
    std::string serialize() const override;
    void deserialize(std::string serialized) override;
    uint8_t getTypeID() const override { return 200; }
    //~ End SerializableInterface

    
    static Vector2 getPlayerSpawnLocation() { return sPlayerSpawnLocation; }
};
