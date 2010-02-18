//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////

#include "MWObjectFactory.h"

template<> MirrorWorld::ObjectFactory* Ogre::Singleton<MirrorWorld::ObjectFactory>::ms_Singleton = NULL;

namespace MirrorWorld {
ObjectFactory::ObjectFactory()
{
}

ObjectFactory::~ObjectFactory()
{
    m_MakerMap.clear();
}

Object* ObjectFactory::createObj(Ogre::String type)
{
    ObjectMaker* objMaker = m_MakerMap[type];
    if (objMaker)
        return objMaker->create();
    return NULL;
}

void ObjectFactory::registerObjType(MirrorWorld::ObjectMaker *objMaker, Ogre::String type)
{
    m_MakerMap[type] = objMaker;
}
}