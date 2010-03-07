//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#ifndef _MW_MIRROR_BALL_MODEL_H_
#define _MW_MIRROR_BALL_MODEL_H_

#include <Ogre.h>

namespace MirrorWorld
{
class MirrorBallModel
{
public:
    MirrorBallModel(Ogre::SceneManager* sceneMgr);
    ~MirrorBallModel() { destroy(); }
    void active(const Ogre::Vector3& pos);
    void deactive();
    void update(const Ogre::Vector3& pos);
    void destroy();
    static void resetCounter() { m_nUniqueID = 0; }
private:
    static int              m_nUniqueID;
    Ogre::SceneManager*     m_pSceneMgr;
    Ogre::SceneNode*        m_pBallNode;
    Ogre::SceneNode*        m_pTrailNode;
    Ogre::RibbonTrail*      m_pRibbonTrail;
    Ogre::Light*            m_pLight;
    Ogre::BillboardSet*     m_pBillboard;
};
}

#endif