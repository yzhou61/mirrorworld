//////////////////////////////////////////////////////////////////////////
//
//
//
//
//////////////////////////////////////////////////////////////////////////
#ifndef _MW_GAME_FRAMEWORK_H_
#define _MW_GAME_FRAMEWORK_H_

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreOverlay.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <CEGUI.h>
#include <RendererModules/Ogre/CEGUIOgreRenderer.h>

namespace MirrorWorld{
//////////////////////////////////////////////////////////////////////////
// GameFrame class definition
//////////////////////////////////////////////////////////////////////////
class GameFramework : public Ogre::Singleton<GameFramework>, OIS::KeyListener, OIS::MouseListener
{
public:
    static void create() { if (!getSingletonPtr()) new GameFramework(); }
    ~GameFramework();

    bool initOgre(Ogre::String wndTitle, OIS::KeyListener *pKeyListener = NULL, OIS::MouseListener *pMouseListener = NULL);
    void updateOgre(double timeSinceLastFrame);
    void updateStats();

    bool keyPressed(const OIS::KeyEvent &keyEventRef);
    bool keyReleased(const OIS::KeyEvent &keyEventRef);

    bool mouseMoved(const OIS::MouseEvent &evt);
    bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id); 
    bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

    void setDebugInfo(Ogre::String text, int index) { m_DebugInfo[index] = text; }

    Ogre::Root*                 m_pRoot;
    Ogre::RenderWindow*         m_pRenderWnd;
    Ogre::Viewport*			    m_pViewport;
    Ogre::Log*                  m_pLog;
    Ogre::Timer*			    m_pTimer;

    OIS::InputManager*		    m_pInputMgr;
    OIS::Keyboard*			    m_pKeyboard;
    OIS::Mouse*                 m_pMouse;

    CEGUI::OgreRenderer*	    m_pGUIRenderer;

private:
    GameFramework();
private:
    Ogre::Overlay*			    m_pDebugOverlay;
    int				            m_iNumScreenShots;

    Ogre::String				m_DebugInfo[5];
}; // End of GameFramework

} // End of namespace
#endif