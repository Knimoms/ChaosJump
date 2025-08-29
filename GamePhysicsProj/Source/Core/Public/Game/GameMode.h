#pragma once
#include <memory>

#include "Core/TickableInterface.h"
#include "Input/InputReceiverInterface.h"
#include "Math/Vector2.h"
#include "Render/DrawableInterface.h"

class ChunkGenerator;
class Rectangle;
class Player;

class GameMode : public TickableInterface, public InputReceiverInterface
{

private:

    std::unique_ptr<Player> mPlayer = nullptr;
    std::unique_ptr<ChunkGenerator> mChunkGenerator = nullptr;

    
    float mChunkHeight = 0.f;
    int mCurrentChunkHeightCoord = 0;

    std::vector<std::unique_ptr<Rectangle>> mPlatforms = {};

    bool bStarted = false;
    
public:

    GameMode();
    ~GameMode() override;

    virtual void startGame();

    //~Begin TickableInterface
    void tick(float deltaTime) override;
    //~End TickableInterface

    Player* getPlayer() const { return mPlayer.get(); }

    //~Begin InputReceiverInterface
    void handleKeyPressed(SDL_Scancode scancode) override;
    //~End InputReceiverInterface
    
};
