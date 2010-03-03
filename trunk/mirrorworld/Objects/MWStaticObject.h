//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#ifndef _MW_STATIC_OBJECT_H_
#define _MW_STATIC_OBJECT_H_

#include "MWObject.h"

namespace MirrorWorld {
class StaticObject : public Object
{
public:
    StaticObject(unsigned int id, bool attachable = true, bool reflective = false):Object(id, attachable, reflective), 
        m_pPhyMatID(NULL) {}
    virtual ~StaticObject() {}
    virtual Ogre::String name() const = 0;
    void setPhyMaterial(OgreNewt::MaterialID* matID) { m_pPhyMatID = matID; }
    virtual void setEntity(Ogre::SceneManager* sceneMgr, OgreNewt::World* world, 
            Ogre::SceneNode* node, Ogre::Entity* entity); 
private:
    OgreNewt::MaterialID* m_pPhyMatID;
}; // End of StaticObject
} //End of MirrorWorld

#endif