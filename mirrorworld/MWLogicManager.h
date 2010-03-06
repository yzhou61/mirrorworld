//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#ifndef _MW_LOGIC_MANAGER_H_
#define _MW_LOGIC_MANAGER_H_

#include <Ogre.h>
#include <vector>
#include <queue>
#include "Objects/FPSCamera.h"
#include "Objects/MWObjectFactory.h"
#include "Scene/MWLaserModel.h"
#include "Scene/MWMirrorBallModel.h"

namespace MirrorWorld {
class LogicManager 
{
public:
    LogicManager():m_bLaserOn(false), m_pLaser(NULL), m_MirrorBallListBegin(0), m_MirrorBallListEnd(0)
    { memset(m_MirrorBallList, 0, sizeof(MirrorBall*)*MAX_MIRROR); }
    ~LogicManager() {}
    void init(Ogre::SceneManager* sceneMgr, OgreNewt::World* world, int maxMirror = 3, Ogre::Camera* camera = NULL);
    void update(double timeElapsed);
    void switchShootMode() { m_ShootMode = (m_ShootMode == LASER) ? MIRROR_BALL : LASER; }
    void switchToLaser() { m_ShootMode = LASER; }
    void switchToMirrorBall() { m_ShootMode = MIRROR_BALL; }
    void triggerOn()
    { m_ShootMode == LASER ? triggerLaser() : shootMirrorBall(); }
    void triggerOff()
    { if (m_ShootMode == LASER) triggerLaser(); }
private:
    int mirrorSize();
    void triggerLaser();
    void shootMirrorBall();
    void destroyMirrorBall(int idx);
    void calcLaserPath();
    void updateMirrorBall(int idx, double timeElasped);
    void updateMirrorBalls(double timeElasped);
private:
    static const int            MAX_MIRROR = 10;
    static float                m_RaycastDistance;
    enum ShootMode { LASER, MIRROR_BALL };
    ShootMode                   m_ShootMode;
    Ogre::SceneManager*         m_pSceneMgr;
    OgreNewt::World*            m_pWorld;
    bool                        m_bLaserOn;
    Laser*                      m_pLaser;
    int                         m_MaxMirror;
    int                         m_MirrorBallListBegin;
    int                         m_MirrorBallListEnd;
    MirrorBall*                 m_MirrorBallList[MAX_MIRROR];
    double                      m_MirrorBallVelocity;
    Player*                     m_pPlayer;

    // Temporary, should switch to player later
    Ogre::Camera*               m_pCamera;
    LaserModel*                 m_pLaserModel;
    MirrorBallModel*            m_MirrorBallModelList[MAX_MIRROR];

    // Probably switch to OgreNewt RayCast
//    Ogre::RaySceneQuery*        m_TestRayQuery;
//    Ogre::Ray                   m_TestRay;
};
}
#endif