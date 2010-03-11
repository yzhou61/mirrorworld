//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

#include "MWMenuGameState.h"

namespace MirrorWorld{
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void MenuGameState::create(const Ogre::String name)
{
    MenuGameState* menuGameState = new MenuGameState();
    GameStateManager::getSingletonPtr()->addGameState(name, menuGameState);
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void MenuGameState::enter()
{
    GameFramework::getSingletonPtr()->m_pLog->logMessage("Entering MenuState...");

    m_pSceneMgr = GameFramework::getSingletonPtr()->m_pRoot->createSceneManager(Ogre::ST_GENERIC, "MenuSceneMgr");
    m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.7, 0.7, 0.7)); 

    m_pCamera = m_pSceneMgr->createCamera("MenuCam");
    m_pCamera->setPosition(Vector3(0, 25, -50));
    m_pCamera->lookAt(Vector3(0, 0, 0));
    m_pCamera->setNearClipDistance(1);

    m_pCamera->setAspectRatio(Real(GameFramework::getSingletonPtr()->m_pViewport->getActualWidth()) / 
        Real(GameFramework::getSingletonPtr()->m_pViewport->getActualHeight()));

    GameFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);

    GameFramework::getSingletonPtr()->m_pKeyboard->setEventCallback(this);
    GameFramework::getSingletonPtr()->m_pMouse->setEventCallback(this);

    // TO DO : Needed by CEGUI
    CEGUI::System::getSingletonPtr()->setDefaultMouseCursor((CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"MouseArrow");
    CEGUI::MouseCursor::getSingleton().setImage("TaharezLook", "MouseArrow");

    const OIS::MouseState state = GameFramework::getSingletonPtr()->m_pMouse->getMouseState();
    CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition(); 
    CEGUI::System::getSingleton().injectMouseMove(state.X.abs-mousePos.d_x, state.Y.abs-mousePos.d_y);

    CEGUI::Window* pMainWnd = CEGUI::WindowManager::getSingleton().getWindow("MenuGUI");
    CEGUI::System::getSingletonPtr()->setGUISheet(pMainWnd);

    CEGUI::PushButton* button = (CEGUI::PushButton*)pMainWnd->getChild("ExitButton");
    button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuGameState::onExitButton, this));
    button = (CEGUI::PushButton*)pMainWnd->getChild("EnterButton");
    button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuGameState::onEnterButton, this));

/*    if (!GameFramework::getSingletonPtr()->m_pSoundManager->init())
        GameFramework::getSingletonPtr()->m_pLog->logMessage("Failed to init sound manager in Menu Game State");*/

    m_bQuit = false;

    createScene();
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void MenuGameState::exit()
{
    GameFramework::getSingletonPtr()->m_pLog->logMessage("Leaving MenuState...");
    CEGUI::System::getSingletonPtr()->setGUISheet(NULL);
    m_pSceneMgr->destroyCamera(m_pCamera);
    if(m_pSceneMgr)
        GameFramework::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool MenuGameState::pause()
{
    GameFramework::getSingletonPtr()->m_pLog->logMessage("Pausing MenuState...");
    CEGUI::System::getSingletonPtr()->setGUISheet(NULL);

    return true;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void MenuGameState::resume()
{
    GameFramework::getSingletonPtr()->m_pLog->logMessage("Resuming MenuState...");
    GameFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);
    CEGUI::System::getSingletonPtr()->setGUISheet(CEGUI::WindowManager::getSingleton().getWindow("MenuGUI"));

    m_bQuit = false;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void MenuGameState::createScene()
{

}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void MenuGameState::update(double timeSinceLastFrame)
{
    if (m_bQuit == true)
    {
        m_pStateManager->popGameState();
        return;
    }
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool MenuGameState::onExitButton(const CEGUI::EventArgs &args)
{
    m_bQuit = true;
    return true;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool MenuGameState::onEnterButton(const CEGUI::EventArgs &args)
{
//    GameFramework::getSingleton().setDebugInfo("Loading...", 0);
    CEGUI::MouseCursor::getSingleton().hide();
    m_pStateManager->pushGameState(m_pStateManager->findByName("StageState"));
//    GameFramework::getSingleton().setDebugInfo("", 0);
    return true;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool MenuGameState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
    if (GameFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_ESCAPE))
    {
        m_bQuit = true;
        return true;
    }

    GameFramework::getSingletonPtr()->keyPressed(keyEventRef);

    return true;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool MenuGameState::keyReleased(const OIS::KeyEvent &keyEventRef)
{
    GameFramework::getSingletonPtr()->keyReleased(keyEventRef);
    return true;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool MenuGameState::mouseMoved(const OIS::MouseEvent &evt)
{
    CEGUI::System::getSingletonPtr()->injectMouseWheelChange((float)evt.state.Z.rel);
    CEGUI::System::getSingletonPtr()->injectMouseMove((float)evt.state.X.rel, (float)evt.state.Y.rel);

    return true;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool MenuGameState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    if (id == OIS::MB_Left)
        CEGUI::System::getSingletonPtr()->injectMouseButtonDown(CEGUI::LeftButton);

    return true;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool MenuGameState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    if (id == OIS::MB_Left)
        CEGUI::System::getSingletonPtr()->injectMouseButtonUp(CEGUI::LeftButton);

    return true;
}
}