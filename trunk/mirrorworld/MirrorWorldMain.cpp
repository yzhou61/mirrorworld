//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////

#include "MirrorWorldMain.h"
#include "Objects/MWObjectFactory.h"

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
MirrorWorldMain::~MirrorWorldMain()
{
    delete GameFramework::getSingletonPtr();
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void MirrorWorldMain::start()
{
    GameFramework::create();
    if (!GameFramework::getSingletonPtr()->initOgre("MirrorWorld", NULL, NULL))
        return;

    GameStateManager::create();
    MenuGameState::create("MenuState");
    StageGameState::create("StageState");

    ObjectFactory::create();
    ObjectFactory::getSingleton().registerObjType(new WallMaker(), "Wall");

    GameFramework::getSingletonPtr()->m_pLog->logMessage("Mirror World initialized!");
    GameStateManager::getSingletonPtr()->start(GameStateManager::getSingletonPtr()->findByName("MenuState"));
}
