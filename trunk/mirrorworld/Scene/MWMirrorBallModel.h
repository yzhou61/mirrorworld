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
    MirrorBallModel(Ogre::SceneManager* sceneMgr, const Ogre::Vector3& initPos);
    ~MirrorBallModel() { destroy(); }
    void update(const Ogre::Vector3& pos);
    void destroy();
private:
    Ogre::SceneManager*     m_pSceneMgr;
    Ogre::SceneNode*        m_pSceneNode;
    Ogre::Light*            m_pLight;
    Ogre::BillboardSet*     m_pBillboard;
};
}

#endif