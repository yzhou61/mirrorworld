//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#ifndef _MW_LOGIC_MANAGER_H_
#define _MW_LOGIC_MANAGER_H_

#include <Ogre.h>
#include <vector>
#include <queue>
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
    ~LogicManager();
	void init(Ogre::SceneManager* sceneMgr, OgreNewt::World* world, Ogre::RenderWindow *window, 
		int maxMirror = 3, Ogre::Camera* camera = NULL, int numTrigger = 3);
    void update(double timeElapsed);
    void triggerLaser();
    void shootMirrorBall();
    void hitObstacle(const Ogre::Vector3& pos, const Ogre::Vector3& nor);

    // Mirror related.
    void updateMirrors(Ogre::Vector3 position, Ogre::Vector3 direction, Ogre::Vector3 up, Ogre::Real fLeft, Ogre::Real fRight, 
		Ogre::Real fTop, Ogre::Real fBottom, size_t mirrorID);
    void preUpdateMirrors();
    void postUpdateMirrors(double timeElapsed);

    void showMirror(Ogre::Vector3 position, Ogre::Vector3 normal, Ogre::Vector3 hitDirection);
    void finishedTrigger() { m_nTrigger--; if (m_nTrigger == 0) return; }

    void addTrigger(Trigger *trigger) { m_Triggers.push_back(trigger); }
private:

    void calcLaserPath();

    void setRealReflections(Ogre::Vector3 position, Ogre::Vector3 direction, Ogre::Real fLeft, Ogre::Real fRight, 
		Ogre::Real fTop, Ogre::Real fBottom);
private:
    static float                m_RaycastDistance;
    Ogre::SceneManager*         m_pSceneMgr;
    OgreNewt::World*            m_pWorld;
    bool                        m_bLaserOn;
    Laser*                      m_pLaser;
    ResourcePool*               m_pMirrorBallPool;
    ResourcePool*               m_pUnattachPool;

    Player*                     m_pPlayer;

    // Temporary, should switch to player later
    Ogre::Camera*               m_pCamera;
    LaserModel*                 m_pLaserModel;

    MirrorWorld::Mirror**       m_MirrorList;
    int                         m_CurMirrorIndex;
    int                         m_MaxMirrors;
    Ogre::RenderWindow*         m_Window;

    Ogre::Camera*               m_MirrorCheckCam;
    int                         m_nTrigger;
    std::vector<Trigger*>       m_Triggers;
};

}
#endif