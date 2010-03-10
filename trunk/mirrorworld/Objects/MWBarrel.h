//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#ifndef _MW_BARREL_H_
#define _MW_BARREL_H_

#include "MWStaticObject.h"

namespace MirrorWorld {
class Barrel : public StaticObject
{
public:
    Barrel(unsigned int id) : StaticObject(id, false, false) { }
    ~Barrel() {}
    Ogre::String name() const { return "Barrel"; }
}; // End of Wall

class BarrelMaker : public ObjectMaker
{
public:
    BarrelMaker(){}
    ~BarrelMaker(){}
    Barrel* create(unsigned int id) const 
    { Barrel* barrel = new Barrel(id); barrel->setPhyMaterial(m_pPhyMatID); return barrel; }
    void setupEngine(Ogre::SceneManager* sceneMgr, OgreNewt::World* world = NULL);
}; // End of BarrelMaker

} // End fo MirrorWorld

#endif