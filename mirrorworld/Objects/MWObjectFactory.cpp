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
    {
        Object* obj = objMaker->create();
        m_ObjectList.push_back(obj);
        return obj;
    }
    return NULL;
}

void ObjectFactory::registerObjType(MirrorWorld::ObjectMaker *objMaker, Ogre::String type)
{
    m_MakerMap[type] = objMaker;
}

void ObjectFactory::setupEngineAll(Ogre::SceneManager* sceneMgr, OgreNewt::World* world /* = NULL */)
{
    for (HashMap<Ogre::String, ObjectMaker*, Ogre::_StringHash>::iterator it = m_MakerMap.begin(); it != m_MakerMap.end(); it++)
        it->second->setupEngine(sceneMgr, world);
}

OgreNewt::MaterialID* ObjectFactory::getPhyMaterial(Ogre::String type)
{
    ObjectMaker* objMaker;
    if ((objMaker = m_MakerMap[type]) != NULL)
        return objMaker->getPhyMaterial();
    return NULL;
}
}