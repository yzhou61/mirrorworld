//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////

#include "MirrorWorldMain.h"

MirrorWorld::~MirrorWorld()
{
    delete GameFramework::getSingletonPtr();
}

void MirrorWorld::start()
{
    new GameFramework();
    if (!GameFramework::getSingletonPtr()->initOgre("MirrorWorld", NULL, NULL))
        return;

    GameFramework::getSingletonPtr()->m_pLog->logMessage("Mirror World initialized!");

    m_pGameStateManager = new GameStateManager();

    MenuGameState::create(m_pGameStateManager, "MenuState");
//    GameState::create(m_pGameStateManager, "GamingState");

    m_pGameStateManager->start(m_pGameStateManager->findByName("MenuState"));

}