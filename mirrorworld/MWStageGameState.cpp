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
    m_pPhyWorld = new OgreNewt::World();
    m_pPhyWorld->setWorldSize(Ogre::AxisAlignedBox(-m_WorldSize, -m_WorldSize, -m_WorldSize, m_WorldSize, m_WorldSize, m_WorldSize));
    ObjectFactory::getSingleton().setupEngineAll(m_pSceneMgr, m_pPhyWorld);
    m_pSceneLoader->parseDotScene(m_SceneFile, "General", m_pSceneMgr, m_pPhyWorld);
    m_pFPSCamera = new FPSCamera(m_pSceneMgr, m_pPhyWorld, m_pCamera);
    setupPhyMaterialPairs();

    m_pPhyWorldDebugger = &m_pPhyWorld->getDebugger();
    m_pPhyWorldDebugger->init(m_pSceneMgr);

    m_pLogicMgr = new LogicManager();
    m_pLogicMgr->init(m_pSceneMgr, m_pPhyWorld, 3, m_pCamera);
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void StageGameState::setupPhyMaterialPairs()
{
    OgreNewt::MaterialID* matWall = ObjectFactory::getSingleton().getPhyMaterial("Wall");
    OgreNewt::MaterialID* matCam = m_pFPSCamera->getPhyMaterial();
    OgreNewt::MaterialPair* materialPairCamWall = new OgreNewt::MaterialPair(m_pPhyWorld, matCam, matWall);

    materialPairCamWall->setDefaultFriction(0.0f, 0.0f);
    materialPairCamWall->setDefaultSoftness(1);
    materialPairCamWall->setDefaultElasticity(0);
    materialPairCamWall->setContinuousCollisionMode(0);
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
    m_pFPSCamera->update();
    m_pPhyWorld->update(static_cast<Ogre::Real>(timeSinceLastFrame));
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
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool StageGameState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
    if (GameFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_ESCAPE))
    {
        m_bQuit = true;
        return true;
    }

    switch (keyEventRef.key)
    {
    case OIS::KC_W:
        m_pFPSCamera->up();
    	break;
    case OIS::KC_S:
        m_pFPSCamera->down();
        break;
    case OIS::KC_A:
        m_pFPSCamera->left();
        break;
    case OIS::KC_D:
        m_pFPSCamera->right();
        break;
    case OIS::KC_SPACE:
        m_pFPSCamera->jump();
        break;
    case OIS::KC_F3:
        if (m_pPhyWorldDebugger)
            m_bShowphyDebugger = !m_bShowphyDebugger;
        break;
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
        m_pFPSCamera->upRelease();
        break;
    case OIS::KC_S:
        m_pFPSCamera->downRelease();
        break;
    case OIS::KC_A:
        m_pFPSCamera->leftRelease();
        break;
    case OIS::KC_D:
        m_pFPSCamera->rightRelease();
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

    m_pFPSCamera->moveCamera(evt.state.X.rel, evt.state.Y.rel);

    return true;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool StageGameState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    if (id == OIS::MB_Left)
    {
        m_bLMouseDown = true;
        CEGUI::System::getSingletonPtr()->injectMouseButtonDown(CEGUI::LeftButton);
        m_pLogicMgr->triggerLaser();
    } 
    else if (id == OIS::MB_Right)
    {
        m_bRMouseDown = true;
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