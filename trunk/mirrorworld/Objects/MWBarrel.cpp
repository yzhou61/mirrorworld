//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#include "MWBarrel.h"

namespace MirrorWorld {

void BarrelMaker::setupEngine(Ogre::SceneManager* sceneMgr, OgreNewt::World* world /* = NULL */)
{
    m_pSceneMgr = sceneMgr;
    m_pPhyWorld = world;
}
}