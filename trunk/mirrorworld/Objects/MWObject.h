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
    Object(unsigned int id, bool attachable = true, bool reflective = false, bool removable = false, bool isTrigger = false):
        m_Identity(id), m_bAttachable(attachable), m_bReflective(reflective), 
            m_bRemovable(removable), m_bTrigger(isTrigger), m_pEntity(NULL),m_pPhyBody(NULL){}
    virtual ~Object(){}
    virtual Ogre::String name() const = 0;
    Ogre::String nameID() const { return name()+"_id"+Ogre::StringConverter::toString(m_Identity); }
    virtual void setEntity(Ogre::SceneManager* sceneMgr, OgreNewt::World* world = NULL, 
        Ogre::SceneNode* node = NULL, Ogre::Entity* entity = NULL) 
    { m_pSceneMgr = sceneMgr; m_pWorld = world; m_pNode = node; m_pEntity = entity; attachUserData(); }
    bool isAttachable() { return m_bAttachable; }
    bool isReflective() { return m_bReflective; }
    bool isRemovable() { return m_bRemovable; }
    bool isTrigger() { return m_bTrigger; }
    unsigned int getID() { return m_Identity; }
    Ogre::SceneNode* getSceneNode() { return m_pNode; }
    OgreNewt::Body* getBody() { return m_pPhyBody; }
    virtual bool trigger() { return false; }
    void setUniName(const Ogre::String& name) { m_UniName = name; }
    Ogre::String& getUniName() { return m_UniName; }
protected:
    void attachUserData();
protected:
    Ogre::String        m_UniName;
    unsigned int        m_Identity;
    // Can attach a mirror?
    bool                m_bAttachable;
    // Can reflect light?
    bool                m_bReflective;
    bool                m_bRemovable;
    bool                m_bTrigger;
    Ogre::SceneManager* m_pSceneMgr;
    Ogre::SceneNode*    m_pNode;
    Ogre::Entity*       m_pEntity;
    OgreNewt::World*    m_pWorld;
    OgreNewt::Body*     m_pPhyBody;
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