//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////
#ifndef _MW_OBJECT_H_
#define _MW_OBJECT_H_

#include <Ogre.h>
#include <OgreNewt.h>

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
    virtual void setEntity(Ogre::SceneManager* sceneMgr, OgreNewt::World* world, Ogre::SceneNode* node, Ogre::Entity* entity) 
    { m_Entity = entity; m_Entity->setUserAny(Ogre::Any(this)); }
    bool isAttachable() { return m_Attachable; }
protected:
    // Can attach a mirror?
    bool                m_Attachable;
    Ogre::Entity*       m_Entity;
}; // End of MWObject

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
class ObjectMaker
{
public:
    ObjectMaker():m_bRegistered(true), m_pPhyMatID(NULL) {}
    virtual ~ObjectMaker(){}
    virtual Object* create() const = 0;
    bool isRegistered() { return m_bRegistered; }
    void registerMaker() { m_bRegistered = true; }
    virtual void setupEngine(Ogre::SceneManager* sceneMgr, OgreNewt::World* world = NULL) 
    { m_pSceneMgr = sceneMgr; m_pPhyWorld = world; }
    OgreNewt::MaterialID* getPhyMaterial() { return m_pPhyMatID; }
protected:
    bool                    m_bRegistered;
    Ogre::SceneManager*     m_pSceneMgr;
    OgreNewt::World*        m_pPhyWorld;
    OgreNewt::MaterialID*   m_pPhyMatID;
}; // End of MWObjectMaker
} // End of namespace
#endif