//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////
#ifndef _MW_STAGE_GAME_STATE_H_
#define _MW_STATE_GAME_STATE_H_

#include "MWGameState.h"
#include "Scene/DotSceneLoader.h"

namespace MirrorWorld{

class StageGameState : public GameState
{
public:
    static void create(const Ogre::String name);
    void enter();
    void createScene();
    void exit();
    bool pause();
    void resume();

    void update(double timeSinceLastFrame);

    bool keyPressed(const OIS::KeyEvent &keyEventRef);
    bool keyReleased(const OIS::KeyEvent &keyEventRef);

    bool mouseMoved(const OIS::MouseEvent &evt);
    bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id); 
    bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

    void moveCamera();
    void handleInput();

private:
    StageGameState() : GameState(), m_SceneFile("TestStage.xml") {}
    ~StageGameState() { delete m_pSceneLoader; }
    bool            m_bQuit;
    // Mouse State
    bool            m_bRMouseDown, m_bLMouseDown;
    Ogre::String    m_SceneFile;
    DotSceneLoader* m_pSceneLoader;
}; // End of StageGameState

} // End of namespace

#endif