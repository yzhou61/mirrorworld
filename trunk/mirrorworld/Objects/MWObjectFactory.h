//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef _MW_OBJECT_FACTORY_H_
#define _MW_OBJECT_FACTORY_H_

#include <OgreString.h>
#include <vector>
#include "MWObject.h"
#include "MWWall.h"
#include "MWBarrel.h"
#include "MWCrate.h"
#include "MWLaser.h"
#include "MWPlayer.h"
#include "MWMirror.h"
#include "MWMirrorBall.h"

namespace MirrorWorld{
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
class ObjectFactory : public Ogre::Singleton<ObjectFactory>
{
public:
    static void create() { if (!getSingletonPtr()) new ObjectFactory(); }
    ~ObjectFactory();
    Object* createObj(Ogre::String type);
    void registerObjType(ObjectMaker* objMaker, Ogre::String type);
    void setupEngineAll(Ogre::SceneManager* sceneMgr, OgreNewt::World* world = NULL);
    OgreNewt::MaterialID* getPhyMaterial(Ogre::String type);
private:
    ObjectFactory();
    unsigned int                                            m_ObjCount;
    HashMap<Ogre::String, ObjectMaker*, Ogre::_StringHash>  m_MakerMap;
    std::vector<Object*>                                    m_ObjectList;
}; // End of ObjectFactory

} // End of MirrorWorld

#endif