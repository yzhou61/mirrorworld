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
    ObjectFactory::getSingleton().registerObjType(new LaserMaker(), "Laser");
    ObjectFactory::getSingleton().registerObjType(new MirrorMgr(), "Mirror");
    ObjectFactory::getSingleton().registerObjType(new MirrorBallMaker(), "MirrorBall");


    GameFramework::getSingletonPtr()->m_pLog->logMessage("Mirror World initialized!");
    GameStateManager::getSingletonPtr()->start(GameStateManager::getSingletonPtr()->findByName("MenuState"));
}
