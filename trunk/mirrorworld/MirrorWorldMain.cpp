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
    GameFramework::create();
    if (!GameFramework::getSingletonPtr()->initOgre("MirrorWorld", NULL, NULL))
        return;

    GameFramework::getSingletonPtr()->m_pLog->logMessage("Mirror World initialized!");

    GameStateManager::create();

    MenuGameState::create("MenuState");
    StageGameState::create("StageState");

    GameFramework::getSingletonPtr()->m_pLog->logMessage("Mirror World initialized!");
    GameStateManager::getSingletonPtr()->start(GameStateManager::getSingletonPtr()->findByName("MenuState"));
}