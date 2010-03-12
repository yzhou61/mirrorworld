//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#ifndef _MW_MISC_H_
#define _MW_MISC_H_

#include "MWStaticObject.h"

namespace MirrorWorld {
    class Misc : public StaticObject
    {
    public:
        Misc(unsigned int id) : StaticObject(id, false, false) { }
        ~Misc() {}
        Ogre::String name() const { return "Miscellaneous"; }
    };

    class MiscMaker : public ObjectMaker
    {
    public:
        MiscMaker(){}
        ~MiscMaker(){}
        Misc* create(unsigned int id) const 
        { Misc* misc = new Misc(id); misc->setPhyMaterial(m_pPhyMatID); return misc; }
        void setupEngine(Ogre::SceneManager* sceneMgr, OgreNewt::World* world = NULL);
    };

} // End fo MirrorWorld

#endif