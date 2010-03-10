//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////

#include "MWStageGameState.h"
#include "Objects/MWObjectFactory.h"

float MirrorWorld::StageGameState::m_WorldSize = 1e6;

namespace MirrorWorld{
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void StageGameState::create(const Ogre::String name)
{
    StageGameState* stageGameState = new StageGameState();
    GameStateManager::getSingletonPtr()->addGameState(name, stageGameState);
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void StageGameState::enter()
{
    GameFramework::getSingletonPtr()->m_pLog->logMessage("Entering GameState...");

    m_pSceneMgr = GameFramework::getSingletonPtr()->m_pRoot->createSceneManager(Ogre::ST_GENERIC, "GameSceneMgr");
    Ogre::AxisAlignedBox worldBox(-m_WorldSize, -m_WorldSize, -m_WorldSize, m_WorldSize, m_WorldSize, m_WorldSize);
    m_pSceneMgr->setOption("Size", &worldBox);
    m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.7, 0.7, 0.7));

    m_pCamera = m_pSceneMgr->createCamera("GameCam");
//    m_pCamera->setPosition(Vector3(5, 60, 60));
//    m_pCamera->lookAt(Vector3(0, 0, 0));
    m_pCamera->setNearClipDistance(5);

    m_pCamera->setAspectRatio(Real(GameFramework::getSingletonPtr()->m_pViewport->getActualWidth()) / 
        Real(GameFramework::getSingletonPtr()->m_pViewport->getActualHeight()));

    GameFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);

    GameFramework::getSingletonPtr()->m_pKeyboard->setEventCallback(this);
    GameFramework::getSingletonPtr()->m_pMouse->setEventCallback(this);

    CEGUI::System::getSingletonPtr()->setDefaultMouseCursor((CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"MouseArrow"); 
    CEGUI::MouseCursor::getSingleton().setImage("TaharezLook", "MouseArrow"); 
    const OIS::MouseState state = GameFramework::getSingletonPtr()->m_pMouse->getMouseState();
    CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition(); 
    CEGUI::System::getSingleton().injectMouseMove(state.X.abs-mousePos.d_x,state.Y.abs-mousePos.d_y);

/*    m_pMainWnd = CEGUI::WindowManager::getSingleton().getWindow("AOF_GUI_GAME");

    GameFramework::getSingletonPtr()->m_pGUISystem->setGUISheet(m_pMainWnd);

    CEGUI::PushButton* pExitButton = (CEGUI::PushButton*)m_pMainWnd->getChild("ExitButton_Game");
    pExitButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameState::onExitButtonGame, this));*/

    m_bLMouseDown = m_bRMouseDown = false;
    m_bQuit = false;

    createScene();
    m_AcumulatedTime = 0.0f;

    m_CrossHair = Ogre::OverlayManager::getSingleton().getByName("Game/CrossHair");
    m_CrossHair->setScale(0.6, 0.6);
    m_CrossHair->show();
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void StageGameState::exit()
{
    GameFramework::getSingletonPtr()->m_pLog->logMessage("Leaving StageState...");
//    CEGUI::System::getSingletonPtr()->setGUISheet(NULL);
    m_pSceneMgr->destroyCamera(m_pCamera);
    if(m_pSceneMgr)
        GameFramework::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);
    delete m_pPhyWorld;

    m_CrossHair->hide();
    CEGUI::MouseCursor::getSingleton().show();
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool StageGameState::pause()
{
    GameFramework::getSingletonPtr()->m_pLog->logMessage("Pausing StageState...");
//    CEGUI::System::getSingletonPtr()->setGUISheet(NULL);

    return true;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void StageGameState::resume()
{
    GameFramework::getSingletonPtr()->m_pLog->logMessage("Resuming GameState...");
    GameFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);

//    CEGUI::System::getSingletonPtr()->setGUISheet(CEGUI::WindowManager::getSingleton().getWindow("AOF_GUI_GAME"));
    m_bQuit = false;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void StageGameState::createScene()
{
    m_pSceneLoader = new DotSceneLoader();
    m_pPhyWorld = new OgreNewt::World(100.0, 1);
    m_pPhyWorld->setThreadCount(1);
    m_pPhyWorld->setWorldSize(Ogre::AxisAlignedBox(-m_WorldSize, -m_WorldSize, -m_WorldSize, m_WorldSize, m_WorldSize, m_WorldSize));
    ObjectFactory::getSingleton().setupEngineAll(m_pSceneMgr, m_pPhyWorld);
    m_pSceneLoader->parseDotScene(m_SceneFile, "General", m_pSceneMgr, m_pPhyWorld);
    m_pPlayer = new Player(0);
    m_pPlayer->init(m_pSceneMgr, m_pPhyWorld, m_pCamera);
    setupPhyMaterialPairs();
    m_pPhyWorldDebugger = &m_pPhyWorld->getDebugger();
    m_pPhyWorldDebugger->init(m_pSceneMgr);
    m_pLogicMgr = new LogicManager();
    m_pLogicMgr->init(m_pSceneMgr, m_pPhyWorld, GameFramework::getSingletonPtr()->m_pRenderWnd, 4, m_pCamera);
	Ogre::Light *light = m_pSceneMgr->createLight("Light1");
	light->setType(Ogre::Light::LT_POINT);
	light->setPosition(Vector3(0, 300, 0));
	light->setDiffuseColour(1.0, 1.0, 1.0);
	light->setSpecularColour(1.0, 1.0, 1.0);
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void StageGameState::setupPhyMaterialPairs()
{
    OgreNewt::MaterialID* matWall = ObjectFactory::getSingleton().getPhyMaterial("Wall");
    OgreNewt::MaterialID* matPlayer = m_pPlayer->getPhyMaterial();
    OgreNewt::MaterialPair* materialPairPlayerWall = new OgreNewt::MaterialPair(m_pPhyWorld, matPlayer, matWall);

    materialPairPlayerWall->setDefaultFriction(0.0f, 0.0f);
    materialPairPlayerWall->setDefaultSoftness(1);
    materialPairPlayerWall->setDefaultElasticity(0);
    materialPairPlayerWall->setContinuousCollisionMode(0);

    OgreNewt::MaterialPair* materialPairPlayerDefault = new OgreNewt::MaterialPair(m_pPhyWorld, matPlayer, m_pPhyWorld->getDefaultMaterialID());
    materialPairPlayerDefault->setDefaultFriction(0.0f, 0.0f);
    materialPairPlayerDefault->setDefaultSoftness(0);
    materialPairPlayerDefault->setDefaultElasticity(0);
    materialPairPlayerDefault->setContinuousCollisionMode(0);
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void StageGameState::update(double timeSinceLastFrame)
{
    if (m_bQuit)
    {
        m_pStateManager->popGameState();
        return;
    }

    handleInput();
    m_pPlayer->update(timeSinceLastFrame);
    m_pPhyWorld->update(static_cast<Real>(timeSinceLastFrame/1000.0));
    m_pLogicMgr->update(timeSinceLastFrame);
    if (m_bShowphyDebugger)
    {
        m_pPhyWorldDebugger->startRaycastRecording();
        m_pPhyWorldDebugger->clearRaycastsRecorded();
        m_pPhyWorldDebugger->showDebugInformation();
    }
    else
    {
        m_pPhyWorldDebugger->clearRaycastsRecorded();
        m_pPhyWorldDebugger->stopRaycastRecording();
        m_pPhyWorldDebugger->hideDebugInformation();
    }
    m_CrossHair->rotate(Ogre::Radian(static_cast<Real>(timeSinceLastFrame / 500.0)));
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool StageGameState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
    static int pos = -200;
    if (GameFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_ESCAPE))
    {
        m_bQuit = true;
        return true;
    }

    switch (keyEventRef.key)
    {
    case OIS::KC_W:
        m_pPlayer->up();
    	break;
    case OIS::KC_S:
        m_pPlayer->down();
        break;
    case OIS::KC_A:
        m_pPlayer->left();
        break;
    case OIS::KC_D:
        m_pPlayer->right();
        break;
    case OIS::KC_SPACE:
        m_pPlayer->jump();
        break;
    case OIS::KC_F3:
        if (m_pPhyWorldDebugger)
            m_bShowphyDebugger = !m_bShowphyDebugger;
        break;
    case OIS::KC_L:
        m_pPlayer->test();
        break;
    case OIS::KC_M:
        m_pPlayer->test2();
    case OIS::KC_N:
//        m_pLogicMgr->getMirror(1)->suspend();
//        m_pLogicMgr->getMirror(0)->activate(Vector3(1, 0, 0), Vector3(-100, 100, 0), Vector3(0, 1, 1));
//        m_pLogicMgr->getMirror(0)->reactivate();
//        m_pLogicMgr->getMirror(1)->reactivate();
//        pos += 100;
//        m_pLogicMgr->showMirror(Vector3(pos, 100, 0), Vector3(-1, 0, 0), Vector3(0.3, 0.4, 0.5));
        break;
    case OIS::KC_B:
//        m_pLogicMgr->getMirror(0)->suspend();
//        m_pLogicMgr->getMirror(1)->activate(Vector3(-1, 0, 0), Vector3(100, 100, 0), Vector3(0, 1, 1));
    default:
        GameFramework::getSingletonPtr()->keyPressed(keyEventRef);
        break;
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool StageGameState::keyReleased(const OIS::KeyEvent &keyEventRef)
{
    switch (keyEventRef.key)
    {
    case OIS::KC_W:
        m_pPlayer->upRelease();
        break;
    case OIS::KC_S:
        m_pPlayer->downRelease();
        break;
    case OIS::KC_A:
        m_pPlayer->leftRelease();
        break;
    case OIS::KC_D:
        m_pPlayer->rightRelease();
        break;
    default:
        GameFramework::getSingletonPtr()->keyReleased(keyEventRef);
        break;
    }
    
    return true;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool StageGameState::mouseMoved(const OIS::MouseEvent &evt)
{
    CEGUI::System::getSingletonPtr()->injectMouseWheelChange((float)evt.state.Z.rel);
    CEGUI::System::getSingletonPtr()->injectMouseMove((float)evt.state.X.rel, (float)evt.state.Y.rel);

    m_pPlayer->moveCamera(evt.state.X.rel, evt.state.Y.rel);

    return true;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool StageGameState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    if (id == OIS::MB_Left)
    {
        CEGUI::System::getSingletonPtr()->injectMouseButtonDown(CEGUI::LeftButton);
        if (m_bLMouseDown)
            return true;
        m_bLMouseDown = true;
        m_pLogicMgr->triggerLaser();
    } 
    else if (id == OIS::MB_Right)
    {
        m_bRMouseDown = true;
        m_pLogicMgr->shootMirrorBall();
    } 

    return true;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool StageGameState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    if (id == OIS::MB_Left)
    {
        m_bLMouseDown = false;
        CEGUI::System::getSingletonPtr()->injectMouseButtonUp(CEGUI::LeftButton);
        m_pLogicMgr->triggerLaser();
    } 
    else if (id == OIS::MB_Right)
    {
        m_bRMouseDown = false;
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void StageGameState::handleInput()
{
    
}
}