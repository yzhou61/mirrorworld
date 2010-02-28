//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#ifndef _MW_WALL_H_
#define _MW_WALL_H_

#include "MWStaticObject.h"

namespace MirrorWorld {
class Wall : public StaticObject
{
public:
    Wall() {}
    ~Wall() {}
    Ogre::String name() { return "Wall"; }
}; // End of Wall

class WallMaker : public ObjectMaker
{
public:
    WallMaker(){}
    ~WallMaker(){}
    Wall* create() const { Wall* wall = new Wall(); wall->setPhyMaterial(m_pPhyMatID); return wall; };
    void setupEngine(Ogre::SceneManager* sceneMgr, OgreNewt::World* world = NULL);
}; // End of WallMaker

} // End fo MirrorWorld

#endif