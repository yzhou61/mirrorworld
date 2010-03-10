//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#ifndef _MW_CRATE_H_
#define _MW_CRATE_H_

#include "MWStaticObject.h"

namespace MirrorWorld {
class Crate : public StaticObject
{
public:
    Crate(unsigned int id) : StaticObject(id, false, false) { }
    ~Crate() {}
    Ogre::String name() const { return "Crate"; }
}; // End of Wall

class CrateMaker : public ObjectMaker
{
public:
    CrateMaker(){}
    ~CrateMaker(){}
    Crate* create(unsigned int id) const 
    { Crate* crate = new Crate(id); crate->setPhyMaterial(m_pPhyMatID); return crate; }
    void setupEngine(Ogre::SceneManager* sceneMgr, OgreNewt::World* world = NULL);
}; // End of WallMaker

} // End fo MirrorWorld

#endif