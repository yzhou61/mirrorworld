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
#include "MWResourcePool.h"
#include "Scene/MWLaserModel.h"
#include "Scene/MWMirrorBallModel.h"
#include "Scene/MWUnattachModel.h"

namespace MirrorWorld {
class LogicManager;
class MirrorBallNode : public ResourceNode
{
public:
    MirrorBallNode(Ogre::SceneManager* sceneMgr, OgreNewt::World* world, LogicManager* logicEng);
    ~MirrorBallNode() {}
    bool update(double timeElasped);
    void active(const Ogre::Vector3& pos, const Ogre::Vector3& dir);
    void deactive();
    MirrorBall*             ball;
    MirrorBallModel*        model;
private:
    float                   m_MirrorBallVelocity;
    Ogre::SceneManager*     m_pSceneMgr;
    OgreNewt::World*        m_pWorld;
    LogicManager*           m_pLogicEngine;
};

class UnattachNode : public ResourceNode
{
public:
    UnattachNode(Ogre::SceneManager* sceneMgr, OgreNewt::World* world);
    ~UnattachNode() {}
    bool update(double timeElasped);
    void active(const Ogre::Vector3& pos, const Ogre::Vector3& nor);
    void deactive();
    UnattachModel*      model;
private:
    Ogre::SceneManager*     m_pSceneMgr;
    OgreNewt::World*        m_pWorld;
};

class LogicManager 
{
public:
    LogicManager();
    ~LogicManager() {}
	void init(Ogre::SceneManager* sceneMgr, OgreNewt::World* world, Ogre::RenderWindow *window, 
		int maxMirror = 3, Ogre::Camera* camera = NULL);
    void update(double timeElapsed);
    void switchShootMode() { m_ShootMode = (m_ShootMode == LASER) ? MIRROR_BALL : LASER; }
    void switchToLaser() { m_ShootMode = LASER; }
    void switchToMirrorBall() { m_ShootMode = MIRROR_BALL; }
    void triggerOn()
    { m_ShootMode == LASER ? triggerLaser() : shootMirrorBall(); }
    void triggerOff()
    { if (m_ShootMode == LASER) triggerLaser(); }
    void shootMirrorBall();
    void hitObstacle(const Ogre::Vector3& pos, const Ogre::Vector3& nor);

    // Mirror related.
    size_t createMirror(Ogre::Vector3 normal, Ogre::Vector3 position, Ogre::Vector3 up);
    void updateMirrors(Ogre::Vector3 position, Ogre::Vector3 direction, Ogre::Vector3 up, Ogre::Real fLeft, Ogre::Real fRight, 
		Ogre::Real fTop, Ogre::Real fBottom, size_t mirrorID);
    void preUpdateMirrors();
    void postUpdateMirrors(double timeElapsed);

    Mirror *getMirror(size_t mirrorID) {
        if (mirrorID >= m_MirrorCount)
            return NULL;
        return m_MirrorList[mirrorID];
    }

private:
    void triggerLaser();
    void calcLaserPath();

    void setRealReflections(Ogre::Vector3 position, Ogre::Vector3 direction, Ogre::Real fLeft, Ogre::Real fRight, 
		Ogre::Real fTop, Ogre::Real fBottom);
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
    ResourcePool*               m_pMirrorBallPool;
    ResourcePool*               m_pUnattachPool;

    Player*                     m_pPlayer;

    // Temporary, should switch to player later
    Ogre::Camera*               m_pCamera;
    LaserModel*                 m_pLaserModel;

    MirrorWorld::Mirror*        m_MirrorList[MAX_MIRROR];
    Ogre::RenderWindow*         m_Window;
    size_t                      m_MirrorCount;
};

}
#endif