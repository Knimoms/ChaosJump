#pragma once
#include <memory>
#include <vector>

#include "Base/TickableInterface.h"
#include "Input/InputReceiverInterface.h"
#include "Math/Vector2.h"
#include "SDL3/SDL_scancode.h"

class CollisionObject;
class Platform;
class ChunkGenerator;
class Player;

class GameMode : public TickableInterface, public InputReceiverInterface
{
private:

    Vector2 mPlayerSpawnLocation = {.x = 0, .y = 500};

    std::unique_ptr<Player> mPlayer = nullptr;
    std::unique_ptr<ChunkGenerator> mChunkGenerator = nullptr;

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

    GameMode();
    ~GameMode() override;

    virtual void startGame();
    virtual void gameOver();

    //~Begin TickableInterface
    void tick(float deltaTime) override;
    //~End TickableInterface

    Player* getPlayer() const { return mPlayer.get(); }

    //~Begin InputReceiverInterface
    void handleKeyPressed(SDL_Scancode scancode) override;
    //~End InputReceiverInterface
    
};
