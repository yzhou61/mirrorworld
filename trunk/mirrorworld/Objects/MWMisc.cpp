//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#include "MWMisc.h"

namespace MirrorWorld {

    void MiscMaker::setupEngine(Ogre::SceneManager* sceneMgr, OgreNewt::World* world /* = NULL */)
    {
        m_pSceneMgr = sceneMgr;
        m_pPhyWorld = world;
    }
}