//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////

#include "MirrorWorldMain.h"
#include "Objects/MWObjectFactory.h"
#include "Objects/MWTestObj.h"

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

    GameFramework::getSingletonPtr()->m_pLog->logMessage("Mirror World initialized!");

    GameStateManager::create();
    MenuGameState::create("MenuState");
    StageGameState::create("StageState");

    ObjectFactory::create();
    ObjectFactory::getSingleton().registerObjType(new TestObjMaker(), "TestObj");

    GameFramework::getSingletonPtr()->m_pLog->logMessage("Mirror World initialized!");
    GameStateManager::getSingletonPtr()->start(GameStateManager::getSingletonPtr()->findByName("MenuState"));
}
