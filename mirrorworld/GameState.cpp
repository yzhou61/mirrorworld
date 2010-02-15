//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#include <OgreWindowEventUtilities.h>
#include "GameState.h"

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
GameStateManager::~GameStateManager()
{
    while (!m_ActiveStateStack.empty()) 
    {
        m_ActiveStateStack.top()->exit();
        m_ActiveStateStack.pop();
    }
    m_States.clear();
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void GameStateManager::addGameState(Ogre::String stateName, GameState* state)
{
    try
    {
        gameStateInfo newStateInfo;
        newStateInfo.name = stateName;
        newStateInfo.state = state;
        m_States.push_back(newStateInfo);		
    }
    catch(std::exception& e)
    {
        state->destroy();
        throw Ogre::Exception(Ogre::Exception::ERR_INTERNAL_ERROR, 
            "Error while trying to add a new GameState\n" + Ogre::String(e.what()), "GameStateManager.cpp");
    }
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
GameState* GameStateManager::findByName(Ogre::String stateName) const
{
    for(std::vector<gameStateInfo>::const_iterator it = m_States.begin(); it != m_States.end(); it++)
    {
        if (it->name == stateName)
            return it->state;
    }

    return NULL;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void GameStateManager::start(GameState* state)
{
    changeGameState(state);

    double timeSinceLastFrame = 1;
    double startTime = 0;

    GameFramework* framework = GameFramework::getSingletonPtr();
    while (!m_bShutdown) 
    {
        if (framework->m_pRenderWnd->isClosed())
            m_bShutdown = true;

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        Ogre::WindowEventUtilities::messagePump();
#endif	
        if(framework->m_pRenderWnd->isActive())
        {
            startTime = framework->m_pTimer->getMillisecondsCPU();

            framework->m_pKeyboard->capture();
            framework->m_pMouse->capture();

            m_ActiveStateStack.top()->update(timeSinceLastFrame);

            framework->updateOgre(timeSinceLastFrame);
            framework->m_pRoot->renderOneFrame();

            timeSinceLastFrame = framework->m_pTimer->getMillisecondsCPU() - startTime;
        }
        else
        {
            Sleep(1000);
        }
    }

    framework->m_pLog->logMessage("Main loop quit");
    framework->m_pLog->logMessage("Shutdown OGRE...");
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void GameStateManager::changeGameState(GameState* state)
{
    if (!m_ActiveStateStack.empty()) 
    {
        m_ActiveStateStack.top()->exit();
        m_ActiveStateStack.pop();
    }

    m_ActiveStateStack.push(state);
    init(state);
    m_ActiveStateStack.top()->enter();
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool GameStateManager::pushGameState(GameState* state)
{
    if (!m_ActiveStateStack.empty()) 
    {
        if (!m_ActiveStateStack.top()->pause())
            return false;
    }

    m_ActiveStateStack.push(state);
    init(state);
    m_ActiveStateStack.top()->enter();

    return true;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void GameStateManager::popGameState(void)
{
    if (!m_ActiveStateStack.empty()) 
    {
        m_ActiveStateStack.top()->exit();
        m_ActiveStateStack.pop();
    }

    if (!m_ActiveStateStack.empty()) 
    {
        init(m_ActiveStateStack.top());
        m_ActiveStateStack.top()->resume();
    }
    else
        shutdown();
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void GameStateManager::shutdown()
{
    m_bShutdown = true;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void GameStateManager::init(GameState *state)
{
    GameFramework::getSingletonPtr()->m_pKeyboard->setEventCallback(state);
    GameFramework::getSingletonPtr()->m_pMouse->setEventCallback(state);

    GameFramework::getSingletonPtr()->m_pRenderWnd->resetStatistics();
}




