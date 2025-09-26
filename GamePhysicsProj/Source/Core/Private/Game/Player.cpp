#include "Game/Player.h"

#include "Application.h"

void Player::setOwningConnection(const HSteamNetConnection inOwningConnection)
{
    SerializableInterface::setOwningConnection(inOwningConnection);

    Application& app = Application::getApplication();
    GameMode* gameMode = app.getGameMode();
    InputRouter* inputRouter = app.getInputRouter();
    
    if (isLocallyOwned())
    {
        gameMode->setLocalPlayer(this);
        inputRouter->addInputReceiver(this);    
    }
    else
    {
        if (gameMode->getLocalPlayer() == this)
        {
            gameMode->setLocalPlayer(nullptr);
        }
        
        inputRouter->removeInputReceiver(this);    
    }
}
