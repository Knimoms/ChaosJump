 #include "Game/Player.h"

#include "Application.h"

 Player::Player()
 {
     Application::getApplication().getGameMode()->addPlayer(this);
 }

 Player::~Player()
 {
     Application::getApplication().getGameMode()->removePlayer(this);
 }

void Player::setOwningConnection(const HSteamNetConnection inOwningConnection)
{
    SerializableInterface::setOwningConnection(inOwningConnection);

    InputRouter* inputRouter = Application::getApplication().getInputRouter();
    
    if (isLocallyOwned())
    {
        inputRouter->addInputReceiver(this);    
    }
    else
    {
        inputRouter->removeInputReceiver(this);    
    }
}
