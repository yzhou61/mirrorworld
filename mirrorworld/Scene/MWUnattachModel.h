//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#ifndef _MW_UNATTACH_MODEL_H_
#define _MW_UNATTACH_MODEL_H_

#include <Ogre.h>
#include <OgreNewt.h>

namespace MirrorWorld
{
class UnattachModel
{
public:
    UnattachModel(Ogre::SceneManager* sceneMgr, OgreNewt::World* world);
    ~UnattachModel() { destroy(); }
    void active(const Ogre::Vector3& pos, const Ogre::Vector3& normal);
    void deactive();
    bool update(double timeElasped);
    void destroy();
    bool isActive() { return m_bActive; }
    static void resetCounter() { m_nUniqueID = 0; }
private:
    static int              m_nUniqueID;
    Ogre::SceneManager*     m_pSceneMgr;
    OgreNewt::World*        m_pWorld;
    Ogre::SceneNode*        m_pSceneNode;
    Ogre::ParticleSystem*   m_pParticleSys;
    bool                    m_bActive;
    double                  m_LifeTime;
};
}

#endif