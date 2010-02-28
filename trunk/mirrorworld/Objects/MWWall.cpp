//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#include "MWWall.h"

namespace MirrorWorld {

void WallMaker::setupEngine(Ogre::SceneManager* sceneMgr, OgreNewt::World* world /* = NULL */)
{
    m_pSceneMgr = sceneMgr; 
    m_pPhyWorld = world;
    if (m_pPhyWorld)
        m_pPhyMatID = new OgreNewt::MaterialID(m_pPhyWorld);
}
}