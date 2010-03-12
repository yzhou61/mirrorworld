//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////

#include "MWObjectFactory.h"

template<> MirrorWorld::ObjectFactory* Ogre::Singleton<MirrorWorld::ObjectFactory>::ms_Singleton = NULL;

namespace MirrorWorld {
    ObjectFactory::ObjectFactory():m_ObjCount(NULL)
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
        Object* obj = objMaker->create(m_ObjCount++);
        if (!obj->isRemovable())
            m_ObjectList.push_back(obj);
        return obj;
    }
    Ogre::LogManager::getSingleton().stream()<<"Failed to create "<<type;
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

Object* ObjectFactory::getObjectByName(const Ogre::String& name)
{
    for (std::vector<Object*>::iterator it = m_ObjectList.begin(); it != m_ObjectList.end(); it++)
    {
        if ((*it)->getUniName() == name)
            return (*it);
    }
    return NULL;
}
}