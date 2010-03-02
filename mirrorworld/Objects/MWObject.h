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
    Object(unsigned int id, bool attachable = true, bool reflective = false):m_Identity(id), m_Attachable(attachable), 
        m_Reflective(reflective), m_Entity(NULL){}
    virtual ~Object(){}
    virtual Ogre::String name() const = 0;
    Ogre::String nameID() const { return name()+"_id"+Ogre::StringConverter::toString(m_Identity); }
    virtual void setEntity(Ogre::SceneManager* sceneMgr, OgreNewt::World* world, Ogre::SceneNode* node, Ogre::Entity* entity) 
    { m_Entity = entity; m_Entity->setUserAny(Ogre::Any(this)); }
    bool isAttachable() { return m_Attachable; }
    bool isReflective() { return m_Reflective; }
    unsigned int getID() { return m_Identity; }
protected:
    // Can attach a mirror?
    unsigned int        m_Identity;
    bool                m_Attachable;
    // Can reflect light?
    bool                m_Reflective;
    Ogre::Entity*       m_Entity;
}; // End of MWObject

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
class ObjectMaker
{
public:
    ObjectMaker():m_bRegistered(true), m_pSceneMgr(NULL), 
        m_pPhyWorld(NULL), m_pPhyMatID(NULL) {}
    virtual ~ObjectMaker(){}
    virtual Object* create(unsigned int id) const = 0;
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