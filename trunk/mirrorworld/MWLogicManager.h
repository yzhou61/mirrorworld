//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#ifndef _MW_LOGIC_MANAGER_H_
#define _MW_LOGIC_MANAGER_H_

#include <Ogre.h>
#include <vector>
#include <queue>
#include "Objects/MWObjectFactory.h"

namespace MirrorWorld {
class LogicManager 
{
public:
    LogicManager():m_bLaserOn(false), m_pLaser(NULL){}
    ~LogicManager() {}
    void init();
private:
    void calcLaserPath();
    void calcMirrorBallsPath();
private:
    Ogre::SceneManager*         m_pSceneMgr;
    bool                        m_bLaserOn;
    Laser*                      m_pLaser;
    std::queue<MirrorBall*>     m_MirrorBallList;
    Player*                     m_pPlayer;

    Ogre::RaySceneQuery*        m_TestRayQuery;
    Ogre::Ray                   m_TestRay;
};
}
#endif