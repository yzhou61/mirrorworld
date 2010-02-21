//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////
#ifndef _MW_OBJECT_H_
#define _MW_OBJECT_H_

#include <OgreEntity.h>
#include <OgreLogManager.h>

namespace MirrorWorld{ 
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
class Object
{
public:
    Object():m_Entity(NULL){}
    virtual ~Object(){}
    virtual Ogre::String name() = 0;
    void setEntity(Ogre::Entity* entity) { m_Entity = entity; }
private:
    Ogre::Entity* m_Entity;
}; // End of MWObject

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
class ObjectMaker
{
public:
    ObjectMaker():m_bRegistered(true){}
    virtual ~ObjectMaker(){}
    virtual Object* create() = 0;
    bool isRegistered() { return m_bRegistered; }
    void registerMaker() { m_bRegistered = true; }
private:
    bool m_bRegistered;
}; // End of MWObjectMaker
} // End of namespace
#endif