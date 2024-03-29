//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////

#include "MWStageGameState.h"
#include "Objects/MWObjectFactory.h"
#include "Objects/MWTrigger.h"
#include <vector>

float MirrorWorld::StageGameState::m_WorldSize = 1e6;
const float OverlookHeight = 900;
const float timeLengthForOverlook = 4000;

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
    m_pSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE);
    m_pSceneMgr->setShadowTextureSettings(512, 1);
    m_pSceneMgr->setShadowCasterRenderBackFaces(false);
    m_pSceneMgr->setShadowColour(Ogre::ColourValue(0.6, 0.6, 0.6));
    m_pSceneMgr->setShadowFarDistance(1000);
    Ogre::LiSPSMShadowCameraSetup* shadowCameraSetup = new Ogre::LiSPSMShadowCameraSetup();
    shadowCameraSetup->setOptimalAdjustFactor(15);
    m_pSceneMgr->setShadowCameraSetup(Ogre::ShadowCameraSetupPtr(shadowCameraSetup));
//    m_pSceneMgr->setShadowTextureCasterMaterial("ShadowCaster");
//    m_pSceneMgr->setShadowTextureReceiverMaterial("ShadowReceiver");
//    m_pSceneMgr->setShadowTextureSelfShadow(true);
    
    m_pCamera = m_pSceneMgr->createCamera("GameCam");
//    m_pCamera->setPosition(Vector3(5, 60, 60));
//    m_pCamera->lookAt(Vector3(0, 0, 0));
    m_pCamera->setNearClipDistance(1);
    m_pCamera->setFarClipDistance(m_WorldSize);
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
    m_pPhyWorld->destroyAllBodies();
    delete m_pPhyWorld;
    m_pSceneMgr->destroyCamera(m_pCamera);
    if(m_pSceneMgr)
        GameFramework::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);

    delete m_pLogicMgr;
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
    m_pPhyWorld->setThreadCount(2);
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
    Ogre::Real lightPositions[][3] = { { -450, 250, -450 }, { -450, 250, 450 }, { 450, 250, -450 }, { 450, 250, 450 },
                                    { -450, 10, -450 }, { -450, 10, 450 }, { 450, 10, -450 }, { 450, 10, 450 } };

    for (int i = 0; i < 1; ++i) {
        Ogre::Light *light = m_pSceneMgr->createLight("Light" + Ogre::StringConverter::toString(i));
        light->setType(Ogre::Light::LT_POINT);
	    light->setPosition(lightPositions[i][0], lightPositions[i][1], lightPositions[i][2]);
//        light->setDirection(Vector3::)
	    light->setDiffuseColour(0.4, 0.4, 0.4);
	    light->setSpecularColour(0.5, 0.5, 0.5);
        light->setAttenuation(15000, 1.0, 0.00014, 0.0000007);
//        light->setAttenuation(4000, 1.0, 0.0, 0.0);
        light->setCastShadows(true);
    }

    m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.75, 0.75, 0.75));

    Vector3 triggerPos[5];
    triggerPos[0] = Vector3(490, 100, 425);
    triggerPos[1] = Vector3(-490, 100, 275);
    triggerPos[2] = Vector3(10, 100, 25);
    triggerPos[3] = Vector3(-450, 100, -375);
    triggerPos[4] = Vector3(400, 995, -400);

    Vector3 triggerNor[5];
    triggerNor[0] = Vector3::NEGATIVE_UNIT_X;
    triggerNor[1] = Vector3::UNIT_X;
    triggerNor[2] = Vector3::UNIT_X;
    triggerNor[3] = Vector3::UNIT_X;
    triggerNor[4] = Vector3::NEGATIVE_UNIT_Y;
    
    Vector3 triggerUp[5];
    triggerUp[0] = Vector3::UNIT_Y;
    triggerUp[1] = Vector3::UNIT_Y;
    triggerUp[2] = Vector3::UNIT_Y;
    triggerUp[3] = Vector3::UNIT_Y;
    triggerUp[4] = Vector3::UNIT_Z;

    Object *obj[5];
    obj[0] = NULL;
    obj[1] = ObjectFactory::getSingleton().getObjectByName("Wal21");
    obj[2] = ObjectFactory::getSingleton().getObjectByName("Msc06");
    obj[3] = ObjectFactory::getSingleton().getObjectByName("Wal23");
    obj[4] = NULL;

    Vector3 dir[5];
    dir[0] = 100 * Vector3::NEGATIVE_UNIT_Y;
    dir[1] = -200 * Vector3::NEGATIVE_UNIT_Y;
    dir[2] = 100 * Vector3::NEGATIVE_UNIT_Y;
    dir[3] = 100 * Vector3::NEGATIVE_UNIT_Y;
    dir[4] = 100 * Vector3::NEGATIVE_UNIT_Y;

    Trigger *trigger[5];
    for (int i = 0; i < 5; ++i) {
        trigger[i] = new Trigger(i);
        trigger[i]->setEntity(m_pSceneMgr, m_pPhyWorld);
        GameFramework::getSingleton().m_pLog->stream() << triggerPos[i];
        trigger[i]->initTrigger(triggerPos[i], triggerNor[i], triggerUp[i]);
        trigger[i]->setTarget(obj[i], dir[i], 1000);
        m_pLogicMgr->addTrigger(trigger[i]);
    }
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void StageGameState::setupPhyMaterialPairs()
{
    OgreNewt::MaterialID* matPlayer = m_pPlayer->getPhyMaterial();
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

    m_pPhyWorld->update(static_cast<Real>(timeSinceLastFrame/1000.0));
    m_pPlayer->update(timeSinceLastFrame);
    m_pLogicMgr->update(timeSinceLastFrame);
    m_CrossHair->rotate(Ogre::Radian(static_cast<Real>(timeSinceLastFrame / 500.0)));
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
    GameFramework::getSingleton().setDebugInfo(Ogre::StringConverter::toString(m_pCamera->getRealPosition()), 0);
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
    case OIS::KC_F3:
        if (m_pPhyWorldDebugger)
            m_bShowphyDebugger = !m_bShowphyDebugger;
        break;
    case OIS::KC_SPACE:
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
    case OIS::KC_SPACE:
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
}
