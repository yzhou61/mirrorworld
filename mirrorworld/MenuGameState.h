//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#ifndef _MENU_GAME_STATE_H_
#define _MENU_GAME_STATE_H_

#include "GameState.h"

class MenuGameState : public GameState
{
public:
    static void create(GameStateManager* manager, const Ogre::String name);
    void enter();
    void exit();
    bool pause();
    void resume();
    void createScene();

    void update(double timeSinceLastFrame);

    bool keyPressed(const OIS::KeyEvent &keyEventRef);
    bool keyReleased(const OIS::KeyEvent &keyEventRef);

    bool mouseMoved(const OIS::MouseEvent &evt);
    bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id); 
    bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

    bool onExitButton(const CEGUI::EventArgs &args);
    bool onEnterButton(const CEGUI::EventArgs &args);
private:
    MenuGameState(GameStateManager* manager) : GameState(manager) {}
    bool    m_bQuit;
}; // End of MenuGameState


#endif