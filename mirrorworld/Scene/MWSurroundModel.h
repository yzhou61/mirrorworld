//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#ifndef _MW_SURROUND_H_
#define _MW_SURROUND_H_

#include <Ogre.h>

namespace MirrorWorld
{
class SurroundModel
{
public:
    SurroundModel(Ogre::SceneManager* sceneMgr);
    ~SurroundModel() { destroy(); }
    void active(const Ogre::Vector3& pos, const Ogre::Vector3& normal, const Ogre::Vector3& up);
    void deactive();
    bool update(double timeElasped);
    void destroy();
    static void resetCounter() { m_nUniqueID = 0; }

    Ogre::SceneNode *getNode();
private:
    bool                    m_bLive;
    static int              m_nUniqueID;
    Ogre::SceneManager*     m_pSceneMgr;
    Ogre::SceneNode*        m_pSceneNode;
    Ogre::ParticleSystem*   m_pParticleSys;
};
}

#endif
