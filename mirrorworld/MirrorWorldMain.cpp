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
    ObjectFactory::getSingleton().registerObjType(new WallMaker(), "Wal");
    ObjectFactory::getSingleton().registerObjType(new WallMaker(), "Clg");
    ObjectFactory::getSingleton().registerObjType(new WallMaker(), "Grd");
    ObjectFactory::getSingleton().registerObjType(new BarrelMaker(), "Brl");
    ObjectFactory::getSingleton().registerObjType(new CrateMaker(), "Crt");
    ObjectFactory::getSingleton().registerObjType(new LaserMaker(), "Laser");
    ObjectFactory::getSingleton().registerObjType(new MirrorMgr(), "Mirror");
    ObjectFactory::getSingleton().registerObjType(new MirrorBallMaker(), "MirrorBall");

    GameFramework::getSingletonPtr()->m_pLog->logMessage("Mirror World initialized!");
    GameStateManager::getSingletonPtr()->start(GameStateManager::getSingletonPtr()->findByName("MenuState"));
}
