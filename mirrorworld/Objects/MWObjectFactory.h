//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef _MW_OBJECT_FACTORY_H_
#define _MW_OBJECT_FACTORY_H_

#include <OgreString.h>
#include <map>
#include "MWObject.h"

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
private:
    ObjectFactory();
    HashMap<Ogre::String, ObjectMaker*, Ogre::_StringHash> m_MakerMap;
}; // End of ObjectFactory

}

#endif