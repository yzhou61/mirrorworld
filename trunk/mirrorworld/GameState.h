//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////
#ifndef _GAME_STATE_H_
#define _GAME_STATE_H_

#include <stack>
#include "GameFramework.h"

using Ogre::Vector3;
using Ogre::Real;
using Ogre::Degree;

class GameStateManager;
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
class GameState : public OIS::KeyListener, public OIS::MouseListener
{
public:
    static void create(const Ogre::String name) {}
    void destroy() { delete this; };
    virtual void enter() = 0;
    virtual void exit() = 0;
    virtual bool pause() { return false; }
    virtual void resume() {}
    virtual void update(double timeSinceLastFrame) = 0;
protected:
    GameState();
    virtual ~GameState() {}

    GameStateManager*       m_pStateManager;
    Ogre::Camera*           m_pCamera;
    Ogre::SceneManager*		m_pSceneMgr;
};// End of GameState

typedef struct 
{
    Ogre::String name;
    GameState* state;
} gameStateInfo;

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
class GameStateManager  : public Ogre::Singleton<GameStateManager>
{
public:
    static void create() { if (!getSingletonPtr()) new GameStateManager(); }
    ~GameStateManager();
    void addGameState(Ogre::String stateName, GameState* state);
    GameState* findByName(Ogre::String stateName) const;
    void start(GameState* state);
    void changeGameState(GameState* state);
    bool pushGameState(GameState* state);
    void popGameState();
    void shutdown();

private:
    GameStateManager() : m_bShutdown(false){}
    void init(GameState* initState);
    std::stack<GameState*> m_ActiveStateStack;
    std::vector<gameStateInfo> m_States;
    bool m_bShutdown;
};// End of GameStateManager

#endif