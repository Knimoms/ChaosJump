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
class Player;

DECLARE_DEFAULT_DELETER(ChunkGenerator)
DECLARE_DEFAULT_DELETER(Player)

class ChaosJumpGameMode : public GameMode, public TickableInterface, public InputReceiverInterface
{
private:

    Vector2 mPlayerSpawnLocation = {.x = 0, .y = 200};

    std::unique_ptr<Player, PlayerDeleter> mLocalPlayer;
    std::vector<std::unique_ptr<Player, PlayerDeleter>> mPlayers;
    std::unique_ptr<ChunkGenerator, ChunkGeneratorDeleter> mChunkGenerator = nullptr;

    float mChunkHeight = 0.f;

    std::vector<std::unique_ptr<Platform>> mPlatforms = {};
    std::vector<std::unique_ptr<CollisionObject>> mObstacles = {};

    bool bStarted = false;
    bool bGameOver = false;

    float mGameTime = 0.f;

    float mReachedHeight = 0.f;

protected:

    void clearDroppedPlatforms();
    void clearObstaclesOutOfRange();

public:

    ChaosJumpGameMode();
    ~ChaosJumpGameMode() override;

    //~ Begin GameMode Interface
    void handleConnectionJoined(HSteamNetConnection connection) override;
    //~ End GameMode Interface
    
    virtual void startGame();
    virtual void gameOver();

    static void hostSession();

    //~Begin TickableInterface
    void tick(float deltaTime) override;
    //~End TickableInterface

    Player* getPlayer() const { return mLocalPlayer.get(); }

    //~Begin InputReceiverInterface
    void handleKeyPressed(SDL_Scancode scancode) override;
    //~End InputReceiverInterface
    
};
