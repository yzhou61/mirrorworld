//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////
#ifndef _MW_STAGE_GAME_STATE_H_
#define _MW_STATE_GAME_STATE_H_

#include "MWGameState.h"
#include "MWLogicManager.h"
#include "Scene/DotSceneLoader.h"
#include "Objects/MWPlayer.h"

namespace MirrorWorld{

class StageGameState : public GameState
{
public:
    static void create(const Ogre::String name);
    void enter();
    void exit();
    bool pause();
    void resume();

    void update(double timeSinceLastFrame);

    bool keyPressed(const OIS::KeyEvent &keyEventRef);
    bool keyReleased(const OIS::KeyEvent &keyEventRef);

    bool mouseMoved(const OIS::MouseEvent &evt);
    bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id); 
    bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

private:
    StageGameState() : GameState(), m_SceneFile("Stage1.xml"), m_bShowphyDebugger(false) {}
    ~StageGameState() { delete m_pSceneLoader; }
private:
    void                createScene();
    void                setupPhyMaterialPairs();
private:
    static float        m_WorldSize;
    double              m_AcumulatedTime;
    bool                m_bQuit;
    // Mouse State
    bool                m_bRMouseDown, m_bLMouseDown;
    Ogre::String        m_SceneFile;
    DotSceneLoader*     m_pSceneLoader;
    OgreNewt::World*    m_pPhyWorld;
    OgreNewt::Debugger* m_pPhyWorldDebugger;
    bool                m_bShowphyDebugger;
    Player*             m_pPlayer;
//    FPSCamera*          m_pFPSCamera;
    LogicManager*       m_pLogicMgr;
    Ogre::Overlay*      m_CrossHair;
}; // End of StageGameState

} // End of namespace

#endif